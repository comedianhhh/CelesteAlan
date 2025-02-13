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
  SPRITE_DICE
};

struct Sprite
{
  IVec2 atlasOffset;
  IVec2 size;
};

// #############################################################################
//                           Assets Functions
// #############################################################################
Sprite get_sprite(SpriteID spriteID)
{
  Sprite sprite = {};
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