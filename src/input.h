#pragma once
#include "Alan_lib.h"
// #############################################################################
//                           Input Struct
// #############################################################################
struct Input
{
  int screenSizeX;
  int screenSizeY;

  //IVec2 screenSize;

  // Screen
  // IVec2 prevMousePos;
  // IVec2 mousePos;
  // IVec2 relMouse;

  // World
  // IVec2 prevMousePosWorld;
  // IVec2 mousePosWorld;
  // IVec2 relMouseWorld;

};

// #############################################################################
//                           Input Globals
// #############################################################################
static Input* input;