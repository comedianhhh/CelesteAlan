#pragma once

#include "Alan_lib.h"

// #############################################################################
//                           Assets Constants
// #############################################################################

// #############################################################################
//                           Assets Structs
// #############################################################################
enum SpriteID
{
  SPRITE_WHITE,
  SPRITE_DICE,
  SPRITE_CELESTE,
  SPRITE_CELESTE_RUN,
  SPRITE_CELESTE_JUMP,
  SPRITE_SOLID_01,
  SPRITE_SOLID_02,  
  SPRITE_BUTTON_PLAY,
  SPRITE_BUTTON_SAVE,

  SPRITE_COUNT
};

struct Sprite
{
  IVec2 atlasOffset;
  IVec2 size;
  int frameCount = 1;
};

// #############################################################################
//                           Assets Functions
// #############################################################################
Sprite get_sprite(SpriteID spriteID)
{
  Sprite sprite = {};
  sprite.frameCount = 1;

  switch(spriteID)
  {
    case SPRITE_WHITE:
    {
      sprite.atlasOffset = {0, 0};
      sprite.size = {1, 1};
      break;
    }

    case SPRITE_DICE:
    {
      sprite.atlasOffset = {16, 0};
      sprite.size = {16, 16};
      break;
    }
  }

  return sprite;
}