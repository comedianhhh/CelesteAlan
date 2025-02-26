#pragma once

#include "input.h"
#include "Alan_lib.h"
#include "sound.h"
#include "render_interface.h"
#include "ui.h"

// #############################################################################
//                           Game Globals
// #############################################################################
constexpr int UPDATES_PER_SECOND = 60;
constexpr double UPDATE_DELAY = 1.0 / UPDATES_PER_SECOND;
constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILESIZE = 8;
constexpr IVec2 WORLD_GRID = {WORLD_WIDTH / TILESIZE, WORLD_HEIGHT / TILESIZE};

// #############################################################################
//                           Player Constants
// #############################################################################


// #############################################################################
//                           Game Structs
// #############################################################################
enum GameInputType
{
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  JUMP,

  MOUSE_LEFT,
  MOUSE_RIGHT,

  PAUSE,

  GAME_INPUT_COUNT
};

struct KeyMapping
{
  Array<KeyCodeID, 3> keys;
};

class Tile
{
  public:
  int neighbourMask;
  bool isVisible;
}; 

enum PlayerAnimState
{
  PLAYER_ANIM_IDLE,
  PLAYER_ANIM_RUN,
  PLAYER_ANIM_JUMP,

  PLAYER_ANIM_COUNT
};

class Player
{
  public:
  IVec2 pos;
  IVec2 prevPos;
  Vec2 speed;
  Vec2 solidSpeed;
  int renderOptions;
  float runAnimTime;
  PlayerAnimState animationState;
  SpriteID animationSprites[PLAYER_ANIM_COUNT];

  void UpdatePlayer(float dt);
};
class Solid
{
  public:
  SpriteID spriteID;
  IVec2 pos;
  IVec2 prevPos;
  Vec2 remainder;
  Vec2 speed;
  int keyframeIdx;
  Array<IVec2, 2> keyframes;
  
  void UpdateSolid(float dt, Player& player);

  IRect get_solid_rect()
  {
    Sprite sprite = get_sprite(spriteID);
    return {pos - sprite.size / 2, sprite.size};
  }

};

enum GameStateID
{
  GAME_STATE_MAIN_MENU,
  GAME_STATE_IN_LEVEL,
};

class GameState
{
  public:
  GameStateID state;
  float updateTimer;
  bool initialized = false;

  Player player;
  Array<Solid, 20> solids;
  
  Array<IVec2, 21> tileCoords;
  Tile worldGrid[WORLD_GRID.x][WORLD_GRID.y];
  KeyMapping keyMappings[GAME_INPUT_COUNT];
};

// #############################################################################
//                           Game Globals
// #############################################################################
static GameState* gameState;

// #############################################################################
//                           Game Functions (Exposed)
// #############################################################################
extern "C"
{
  EXPORT_FN void update_game(GameState* gameStateIn, 
                             RenderData* renderDataIn, 
                             Input* inputIn, 
                             SoundState* soundStateIn,
                             UIState* uiStateIn,
                             float dt);
}

// Add these declarations in the Game Functions section
void initialize_game();
void initialize_player();
void initialize_tileset();
void update_game_state(float dt);
void update_input_state();
void fixed_update(float dt);
void render_game(float interpolatedDT);
void render_solids(float interpolatedDT);
void render_player(float interpolatedDT);
void initialize_key_mappings();
void initialize_solids();
void render_tileset();