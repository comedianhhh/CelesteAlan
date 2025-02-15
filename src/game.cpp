#include "Header/game.h"
#include "Header/assets.h"
#include "Header/texts.h"

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



void Player::UpdatePlayer(float dt)
{
  static Vec2 remainder = {};
  static bool grounded = false;
  constexpr float runSpeed = 2.0f;
  constexpr float runAcceleration = 10.0f;
  constexpr float runReduce = 22.0f; 
  constexpr float flyReduce = 12.0f;    
  constexpr float gravity = 13.0f;
  constexpr float fallSpeed = 3.6f;
  constexpr float jumpSpeed = -3.0f;

  // Facing the Player in the right direction
  if(speed.x > 0)
  {
      renderOptions = 0;
  }
  if(speed.x < 0)
  {
      renderOptions = RENDER_OPTION_FLIP_X;
  }

  // Jump
  if(just_pressed(JUMP) && grounded)
  {
    speed.y = jumpSpeed;
    speed.x += solidSpeed.x;
    speed.y += solidSpeed.y;
    play_sound("jump");
    grounded = false;
  }

  if(!grounded)
  {
    animationState = PLAYER_ANIM_JUMP;
  }

  if(is_down(MOVE_LEFT))
  {
    if(grounded)
    {
      animationState = PLAYER_ANIM_RUN;
    }

    float mult = 1.0f;
    if(speed.x > 0.0f)
    {
      mult = 3.0f;
    }
    runAnimTime += dt;
    speed.x = approach(speed.x, -runSpeed, runAcceleration * mult * dt);
  }

  if(is_down(MOVE_RIGHT))
  {
    if(grounded)
    {
      animationState = PLAYER_ANIM_RUN;
    }

    float mult = 1.0f;
    if(speed.x < 0.0f)
    {
      mult = 3.0f;
    }
    runAnimTime += dt;
    speed.x = approach(speed.x, runSpeed, runAcceleration * mult * dt);
  }

  // Friction
  if(!is_down(MOVE_LEFT) &&
    !is_down(MOVE_RIGHT))
  {
    if(grounded)
    {
      speed.x = approach(speed.x, 0, runReduce * dt);
    }
    else
    {
      speed.x = approach(speed.x, 0, flyReduce * dt);
    }
  }

  // Gravity
  speed.y = approach(speed.y, fallSpeed, gravity * dt);


  if(is_down(MOVE_UP))
  {
    pos = {};
  }

  // Move X
  {
      IRect playerRect = get_player_rect();

      remainder.x += speed.x;
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
                IRect solidRect = solid.get_solid_rect();

                if(rect_collision(playerRect, solidRect))
                {
                  speed.x = 0;
                  return;
                }
              }
            }

            // Loop through local Tiles
            IVec2 playerGridPos = get_grid_pos(pos);
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
                  speed.x = 0;
                  return;
                }
              }
            }

            // Move the Player
            pos.x += moveSign;
            moveX -= moveSign;
          }
        };
        movePlayerX();
      }
  }
  // Move Y
  {
      IRect playerRect = get_player_rect();

      remainder.y += speed.y;
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
                IRect solidRect = solid.get_solid_rect();

                if(rect_collision(playerRect, solidRect))
                {
                  // Moving down/falling
                  if(speed.y > 0.0f)
                  {
                    grounded = true;
                  }

                  speed.y = 0;
                  return;
                }
              }
            }

            // Loop through local Tiles
            IVec2 playerGridPos = get_grid_pos(pos);
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
                  if(speed.y > 0.0f)
                  {
                    grounded = true;
                  }

                  speed.y = 0;
                  return;
                }
              }
            }

            // Move the Player
            pos.y += moveSign;
            moveY -= moveSign;
          }
        };
        movePlayerY();
      }
    }
}



void Solid::UpdateSolid(float dt, Player& player)
{
  IRect solidRect = get_solid_rect();
    solidRect.pos -= 1;
    solidRect.size += 2;

    int nextKeyframeIdx = keyframeIdx + 1;
    nextKeyframeIdx %= keyframes.count;

    // Move X
    {
      remainder.x += speed.x * dt;
      int moveX = round(remainder.x);
      if(moveX != 0)
      {
        remainder.x -= moveX;
        int moveSign = sign(keyframes[nextKeyframeIdx].x - 
                            keyframes[keyframeIdx].x);

        // Move the player in Y until collision or moveY is exausted
        auto moveSolidX = [&]
        {
          while(moveX)
          {
            IRect playerRect = get_player_rect();
            bool standingOnTop = 
              playerRect.pos.y - 1 + playerRect.size.y == solidRect.pos.y;

            solidRect.pos.x += moveSign;

            // Collision happend on left or right, push the player
            bool tileCollision = false;
            if(rect_collision(playerRect, solidRect))
            {
              // Move the player rect
              playerRect.pos.x += moveSign;
              player.solidSpeed.x = speed.x * (float)moveSign / 20.0f;

              // Check for collision, if yes, destroy the player
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
                    tileCollision = true;

                    if(!standingOnTop)
                    {
                      // Death
                      player.pos = {WORLD_WIDTH / 2,  WORLD_HEIGHT - 24};
                    }
                  }
                }
              }

              if(!tileCollision)
              {
                // Actually move the player
                player.pos.x += moveSign;
              }
            }

            // Move the Solid
            pos.x += moveSign;
            moveX -= 1;
            // way point reached
            if(pos.x == keyframes[nextKeyframeIdx].x)
            {
              keyframeIdx = nextKeyframeIdx;
              nextKeyframeIdx++;
              nextKeyframeIdx %= keyframes.count;
            }
          }
        };
        moveSolidX();
      }
    }

    // Move Y
    {
      remainder.y += speed.y * dt;
      int moveY = round(remainder.y);
      if(moveY != 0)
      {
        remainder.y -= moveY;
        int moveSign = sign(keyframes[nextKeyframeIdx].y - 
                            keyframes[keyframeIdx].y);

        // Move the player in Y until collision or moveY is exausted
        auto moveSolidY = [&]
        {
          while(moveY)
          {
            IRect playerRect = get_player_rect();
            solidRect.pos.x += moveSign;

            // Collision happend on bottom, push the player
            if(rect_collision(playerRect, solidRect))
            {
              // Move the player
              player.pos.y += moveSign;
              player.solidSpeed.y = speed.y * (float)moveSign / 40.0f;

              // Check for collision, if yes, destroy the player
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
                    player.pos = {WORLD_WIDTH / 2,  WORLD_HEIGHT - 24};
                  }
                }
              }
            }

            // Move the Solid
            pos.y += moveSign;
            moveY -= 1;

            if(pos.y == keyframes[nextKeyframeIdx].y)
            {
              keyframeIdx = nextKeyframeIdx;
              nextKeyframeIdx++;
              nextKeyframeIdx %= keyframes.count;
            }
          }
        };
        moveSolidY();
      }
    }
  
}


void update_level(float dt)
{
  // Update Player
  Player& player = gameState->player;
  player.prevPos = player.pos;
  player.animationState = PLAYER_ANIM_IDLE;
  player.UpdatePlayer(dt);

  // Update Solids
  {
    Player& player = gameState->player;
    player.solidSpeed = {};

    for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
    {
      Solid& solid = gameState->solids[solidIdx];
      solid.prevPos = solid.pos;

      solid.UpdateSolid(dt,player);

      
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


void update_game_state(float dt) 
{
    switch(gameState->state) 
    {
        case GAME_STATE_MAIN_MENU:
            update_main_menu(dt);
            break;
            
        case GAME_STATE_IN_LEVEL:
            update_level(dt);
            break;
    }
}

void update_input_state() 
{
    // Clear the transitionCount for every key
    for (int keyCode = 0; keyCode < KEY_COUNT; keyCode++)
    {
        input->keys[keyCode].justReleased = false;
        input->keys[keyCode].justPressed = false;
        input->keys[keyCode].halfTransitionCount = 0;
    }

    // Update relative mouse
    input->relMouse = input->mousePos - input->prevMousePos;
    input->prevMousePos = input->mousePos;
}

void fixed_update(float dt) 
{
    gameState->updateTimer += dt;
    while(gameState->updateTimer >= UPDATE_DELAY)
    {
        gameState->updateTimer -= UPDATE_DELAY;
        update_ui();
        update_game_state(UPDATE_DELAY);
        update_input_state();
    }
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
    play_sound("First Steps", SOUND_OPTION_LOOP);
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

    initialize_key_mappings();
    initialize_solids();

    gameState->initialized = true;
    gameState->state = GAME_STATE_IN_LEVEL;
  }

  // Fixed Update Loop
  fixed_update(dt);

  // Render
  float interpolatedDT = (float)(gameState->updateTimer / UPDATE_DELAY);

  // Draw Solids
  {
    for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
    {
      Solid& solid = gameState->solids[solidIdx];
      IVec2 solidPos = lerp(solid.prevPos, solid.pos, interpolatedDT);
      draw_sprite(solid.spriteID, solidPos, {.layer = get_layer(LAYER_GAME, 0)});
    }
  }

  // Draw Player
  {
    Player& player = gameState->player;
    IVec2 playerPos = lerp(player.prevPos, player.pos, interpolatedDT);

    Sprite sprite = get_sprite(player.animationSprites[player.animationState]);
    int animationIdx = animate(&player.runAnimTime, sprite.frameCount, 0.6f);
    draw_sprite(player.animationSprites[player.animationState], playerPos, 
            {
              .animationIdx = animationIdx,
              .renderOptions = player.renderOptions,
              .layer = get_layer(LAYER_GAME, 0)
            });
  }

  render_tileset();
}

void initialize_key_mappings()
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

void initialize_solids()
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

void render_tileset()
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
            transform.materialIdx = get_material_idx({.color = COLOR_WHITE});
            transform.pos = {x * (float)TILESIZE, y * (float)TILESIZE};
            transform.size = {8, 8};
            transform.spriteSize = {8, 8};
            transform.atlasOffset = gameState->tileCoords[tile->neighbourMask];
            transform.layer = get_layer(LAYER_GAME, 0);
            draw_quad(transform);
        }
    }
}