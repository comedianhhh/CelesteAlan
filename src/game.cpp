#include "game.h"

#include "assets.h"
#include "texts.h"

// #############################################################################
//                           Game Constants
// #############################################################################

// #############################################################################
//                           Game Structs
// #############################################################################

// #############################################################################
//                           Game Functions
// #############################################################################
bool just_pressed(GameInputType type)
{
  KeyMapping mapping = gameState->keyMappings[type];
  for(int idx = 0; idx < mapping.keys.count; idx++)
  {
    if(input->keys[mapping.keys[idx]].justPressed)
    {
      return true;
    }
  }

  return false;
}

bool is_down(GameInputType type)
{
  KeyMapping mapping = gameState->keyMappings[type];
  for(int idx = 0; idx < mapping.keys.count; idx++)
  {
    if(input->keys[mapping.keys[idx]].isDown)
    {
      return true;
    }
  }

  return false;
}

IVec2 get_grid_pos(IVec2 worldPos)
{
  return {worldPos.x / TILESIZE, worldPos.y / TILESIZE};
}

Tile* get_tile(int x, int y)
{
  Tile* tile = nullptr;

  if(x >= 0  && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y)
  {
    tile = &gameState->worldGrid[x][y];
  }

  return tile;
}

Tile* get_tile(IVec2 worldPos)
{
  IVec2 gridPos = get_grid_pos(worldPos);
  return get_tile(gridPos.x, gridPos.y);
}

IRect get_player_rect()
{  
  return 
  {
    gameState->player.pos.x - 4, 
    gameState->player.pos.y - 8, 
    8, 
    16
  };
}

IVec2 get_tile_pos(int x, int y)
{
  return {x * TILESIZE, y * TILESIZE};
}

IRect get_tile_rect(int x, int y)
{
  return {get_tile_pos(x, y), 8, 8};
}

IRect get_solid_rect(Solid solid)
{
  Sprite sprite = get_sprite(solid.spriteID);
  return {solid.pos - sprite.size / 2, sprite.size};
}

void update_level(float dt)
{

  // Update Player
  {
    Player& player = gameState->player;
    player.prevPos = player.pos;

    
    static Vec2 remainder = {};
    static bool grounded = false;
    constexpr float runSpeed = 2.0f;
    constexpr float runAcceleration = 10.0f;
    constexpr float runReduce = 22.0f; 
    constexpr float flyReduce = 12.0f;    
    constexpr float gravity = 13.0f;
    constexpr float fallSpeed = 3.6f;
    constexpr float jumpSpeed = -3.0f;

    // Move X
    {
        IRect playerRect = get_player_rect();
  
        remainder.x += player.speed.x;
        int moveX = round(remainder.x);
        if(moveX != 0)
        {
          remainder.x -= moveX;
          int moveSign = sign(moveX);
          bool collisionHappened = false;
  
          // Move the player in Y until collision or moveY is exausted
          auto movePlayerX = [&]
          {
            while(moveX)
            {
              playerRect.pos.x += moveSign;
  
              // Test collision against Solids
              {
                for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
                {
                  Solid& solid = gameState->solids[solidIdx];
                  IRect solidRect = get_solid_rect(solid);
  
                  if(rect_collision(playerRect, solidRect))
                  {
                    player.speed.x = 0;
                    return;
                  }
                }
              }
  
              // Loop through local Tiles
              IVec2 playerGridPos = get_grid_pos(player.pos);
              for(int x = playerGridPos.x - 1; x <= playerGridPos.x + 1; x++)
              {
                for(int y = playerGridPos.y - 2; y <= playerGridPos.y + 2; y++)
                {
                  Tile* tile = get_tile(x, y);
  
                  if(!tile || !tile->isVisible)
                  {
                    continue;
                  }
  
                  IRect tileRect = get_tile_rect(x, y);
                  if(rect_collision(playerRect, tileRect))
                  {
                    player.speed.x = 0;
                    return;
                  }
                }
              }
  
              // Move the Player
              player.pos.x += moveSign;
              moveX -= moveSign;
            }
          };
          movePlayerX();
        }
    }
    // Move Y
    {
        IRect playerRect = get_player_rect();
  
        remainder.y += player.speed.y;
        int moveY = round(remainder.y);
        if(moveY != 0)
        {
          remainder.y -= moveY;
          int moveSign = sign(moveY);
          bool collisionHappened = false;
  
          // Move the player in Y until collision or moveY is exausted
          auto movePlayerY = [&]
          {
            while(moveY)
            {
              playerRect.pos.y += moveSign;
  
              // Test collision against Solids
              {
                for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
                {
                  Solid& solid = gameState->solids[solidIdx];
                  IRect solidRect = get_solid_rect(solid);
  
                  if(rect_collision(playerRect, solidRect))
                  {
                    // Moving down/falling
                    if(player.speed.y > 0.0f)
                    {
                      grounded = true;
                    }
  
                    player.speed.y = 0;
                    return;
                  }
                }
              }
  
              // Loop through local Tiles
              IVec2 playerGridPos = get_grid_pos(player.pos);
              for(int x = playerGridPos.x - 1; x <= playerGridPos.x + 1; x++)
              {
                for(int y = playerGridPos.y - 2; y <= playerGridPos.y + 2; y++)
                {
                  Tile* tile = get_tile(x, y);
  
                  if(!tile || !tile->isVisible)
                  {
                    continue;
                  }
  
                  IRect tileRect = get_tile_rect(x, y);
                  if(rect_collision(playerRect, tileRect))
                  {
                    // Moving down/falling
                    if(player.speed.y > 0.0f)
                    {
                      grounded = true;
                    }
  
                    player.speed.y = 0;
                    return;
                  }
                }
              }
  
              // Move the Player
              player.pos.y += moveSign;
              moveY -= moveSign;
            }
          };
          movePlayerY();
        }
      }


  }

  

  bool updateTiles = false;
  if(is_down(MOUSE_LEFT) && !ui_is_hot() && !ui_is_active())
  {
    IVec2 worldPos = screen_to_world(input->mousePos);
    IVec2 mousePosWorld = input->mousePosWorld;
    Tile* tile = get_tile(worldPos);
    if(tile)
    {
      tile->isVisible = true;
      updateTiles = true;
    }
  }

  if(is_down(MOUSE_RIGHT))
  {
    IVec2 worldPos = screen_to_world(input->mousePos);
    IVec2 mousePosWorld = input->mousePosWorld;
    Tile* tile = get_tile(worldPos);
    if(tile)
    {
      tile->isVisible = false;
      updateTiles = true;
    }
  }

  if(updateTiles)
  {
    // Neighbouring Tiles        Top    Left      Right       Bottom  
    int neighbourOffsets[24] = { 0,-1,  -1, 0,     1, 0,       0, 1,   
    //                          Topleft Topright Bottomleft Bottomright
                                -1,-1,   1,-1,    -1, 1,       1, 1,
    //                           Top2   Left2     Right2      Bottom2
                                 0,-2,  -2, 0,     2, 0,       0, 2};

    // Topleft     = BIT(4) = 16
    // Toplright   = BIT(5) = 32
    // Bottomleft  = BIT(6) = 64
    // Bottomright = BIT(7) = 128

    for(int y = 0; y < WORLD_GRID.y; y++)
    {
      for(int x = 0; x < WORLD_GRID.x; x++)
      {
        Tile* tile = get_tile(x, y);

        if(!tile->isVisible)
        {
          continue;
        }

        tile->neighbourMask = 0;
        int neighbourCount = 0;
        int extendedNeighbourCount = 0;
        int emptyNeighbourSlot = 0;

        // Look at the sorrounding 12 Neighbours
        for(int n = 0; n < 12; n++)
        {
          Tile* neighbour = get_tile(x + neighbourOffsets[n * 2],
                                     y + neighbourOffsets[n * 2 + 1]);

          // No neighbour means the edge of the world
          if(!neighbour || neighbour->isVisible)
          {
            tile->neighbourMask |= BIT(n);
            if(n < 8) // Counting direct neighbours
            {
              neighbourCount++;
            }
            else // Counting neighbours 1 Tile away
            {
              extendedNeighbourCount++;
            }
          }
          else if(n < 8)
          {
            emptyNeighbourSlot = n;
          }
        }

        if(neighbourCount == 7 && emptyNeighbourSlot >= 4) // We have a corner
        {
          tile->neighbourMask = 16 + (emptyNeighbourSlot - 4);
        }
        else if(neighbourCount == 8 && extendedNeighbourCount == 4)
        {
          tile->neighbourMask = 20;
        }
        else
        {
          tile->neighbourMask = tile->neighbourMask & 0b1111;
        }
      }
    }
  }
}

void update_main_menu(float dt)
{

}

void simulate()
{
    float dt = UPDATE_DELAY;

    update_level(dt);

}

// #############################################################################
//                           Game Functions(exposed)
// #############################################################################
EXPORT_FN void update_game(GameState* gameStateIn, 
                           RenderData* renderDataIn, 
                           Input* inputIn, 
                           SoundState* soundStateIn,
                           UIState* uiStateIn,
                           float dt)
{
  if(renderData != renderDataIn)
  {
    gameState = gameStateIn;
    renderData = renderDataIn;
    input = inputIn;
    soundState = soundStateIn;
    uiState = uiStateIn;

    init_strings();
  }

  if(!gameState->initialized)
  {
    // play_sound("First Steps", SOUND_OPTION_LOOP);
    renderData->gameCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
    renderData->gameCamera.position.x = 160;
    renderData->gameCamera.position.y = -90;

    renderData->uiCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
    renderData->uiCamera.position.x = 160;
    renderData->uiCamera.position.y = -90;

    // Player
    {
      Player& player = gameState->player;
      player.animationSprites[PLAYER_ANIM_IDLE] = SPRITE_CELESTE;
      player.animationSprites[PLAYER_ANIM_JUMP] = SPRITE_CELESTE_JUMP;
      player.animationSprites[PLAYER_ANIM_RUN] = SPRITE_CELESTE_RUN;
    }

    // Tileset
    {
      IVec2 tilesPosition = {48, 0};

      for(int y = 0; y < 5; y++)
      {
        for(int x = 0; x < 4; x++)
        {
          gameState->tileCoords.add({tilesPosition.x +  x * 8, tilesPosition.y + y * 8});
        }
      }

      // Black inside
      gameState->tileCoords.add({tilesPosition.x, tilesPosition.y + 5 * 8});
    }

    // Key Mappings
    {
      gameState->keyMappings[MOVE_UP].keys.add(KEY_W);
      gameState->keyMappings[MOVE_UP].keys.add(KEY_UP);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_A);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_LEFT);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_S);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_DOWN);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_D);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_RIGHT);
      gameState->keyMappings[MOUSE_LEFT].keys.add(KEY_MOUSE_LEFT);
      gameState->keyMappings[MOUSE_RIGHT].keys.add(KEY_MOUSE_RIGHT);
      gameState->keyMappings[JUMP].keys.add(KEY_SPACE);
      gameState->keyMappings[PAUSE].keys.add(KEY_ESCAPE);
    }

    // Solids
    {
      Solid solid = {};
      solid.spriteID = SPRITE_SOLID_01;
      solid.keyframes.add({8 * 2,  8 * 10});
      solid.keyframes.add({8 * 10, 8 * 10});
      solid.pos = {8 * 2, 8 * 10};
      solid.speed.x = 50.0f;
      gameState->solids.add(solid);

      solid = {};
      solid.spriteID = SPRITE_SOLID_02;
      solid.keyframes.add({12 * 20, 8 * 10});
      solid.keyframes.add({12 * 20, 8 * 20});
      solid.pos = {12 * 20, 8 * 10};
      solid.speed.y = 50.0f;
      gameState->solids.add(solid);
    }

    gameState->initialized = true;
  }

  // Fixed Update Loop
  {
    gameState->updateTimer += dt;
    while(gameState->updateTimer >= UPDATE_DELAY)
    {
      gameState->updateTimer -= UPDATE_DELAY;
      update_ui();
      simulate();

      // Relative Mouse here, because more frames than simulations
      input->relMouse = input->mousePos - input->prevMousePos;
      input->prevMousePos = input->mousePos;

      // Clear the transitionCount for every key
      {
        for (int keyCode = 0; keyCode < KEY_COUNT; keyCode++)
        {
          input->keys[keyCode].justReleased = false;
          input->keys[keyCode].justPressed = false;
          input->keys[keyCode].halfTransitionCount = 0;
        }
      }
    }
  }

  float interpolatedDT = (float)(gameState->updateTimer / UPDATE_DELAY);


  // Draw Player
  {
    Player& player = gameState->player;
    IVec2 playerPos = lerp(player.prevPos, player.pos, interpolatedDT);
    draw_sprite(SPRITE_CELESTE, playerPos);
    
  }

  // Drawing Tileset
  {
    for(int y = 0; y < WORLD_GRID.y; y++)
    {
      for(int x = 0; x < WORLD_GRID.x; x++)
      {
        Tile* tile = get_tile(x, y);

        if(!tile->isVisible)
        {
          continue;
        }

        // Draw Tile
        Transform transform = {};
        // Draw the Tile around the center
        transform.materialIdx = get_material_idx({.color  = COLOR_WHITE});
        transform.pos = {x * (float)TILESIZE, y * (float)TILESIZE};
        transform.size = {8, 8};
        transform.spriteSize = {8, 8};
        transform.atlasOffset = gameState->tileCoords[tile->neighbourMask];
        transform.layer = get_layer(LAYER_GAME, 0);
        draw_quad(transform);
      }
    }
  }
}