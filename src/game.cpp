#include "game.h"
#include "assets.h"
#include "Alan_lib.h"


// #############################################################################
//                           Game Constants
// #############################################################################

// #############################################################################
//                           Game Structs
// #############################################################################


// #############################################################################
//                           Game Functions
// #############################################################################

//https://learn.microsoft.com/en-us/cpp/cpp/declspec?view=msvc-170
extern "C" EXPORT_FN void update_game(RenderData* renderDataIn, Input* inputIn)
{
    if(renderData != renderDataIn)
    {
        renderData = renderDataIn;
        input = inputIn;
    }

    draw_sprite(SPRITE_DICE,{100.0f,100.0f},{100.0f,100.0f});
}
