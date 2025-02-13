#include "Alan_lib.h"
#include "input.h"
#include "game.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include"glcorearb.h"

// #############################################################################
//                           Platform Includes
// #############################################################################
#include "platform.h"
#ifdef _WIN32
#include "win32_platform.cpp"
const char* gameLibName = "game.dll";
const char* gameLoadLibName = "game_load.dll";
#endif
#include"gl_renderer.cpp"

// #############################################################################
//                           Game DLL Stuff
// #############################################################################
// This is the function potiner to the update_game function
typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr;

// #############################################################################
//                           Cross Platform fuctions
// #############################################################################
void reload_game_dll(BumpAllocator* transientStorage);


int main()
{
    BumpAllocator transientStorage = make_bump_allocator(MB(50));
    BumpAllocator persistentStorage = make_bump_allocator(MB(256));

    input = (Input*)bump_alloc(&persistentStorage, sizeof(Input));
    if(!input)
    {
        SM_ERROR("Failed to allocate input");
        return -1;
    }

    renderData = (RenderData*)bump_alloc(&persistentStorage, sizeof(RenderData));
    if(!renderData)
    {
        SM_ERROR("Failed to allocate renderData");
        return -1;
    }

    gameState= (GameState*)bump_alloc(&persistentStorage, sizeof(GameState));
    if(!gameState)
    {
        SM_ERROR("Failed to allocate gameState");
        return -1;
    }

    platform_create_window(1280,720, "ALAN GAME");

    gl_init(&transientStorage);
 
    while(running)
    {
        reload_game_dll(&transientStorage);
        //update
        platform_update_window();
        update_game(gameState,renderData,input);
        gl_render(&transientStorage);

        platform_sawp_buffers();
        transientStorage.used = 0;
    }
    return 0;
}

void update_game(GameState* gameStateIn,RenderData* renderDataIn, Input* inputIn)
{
    update_game_ptr(gameStateIn,renderDataIn,inputIn);
}

void reload_game_dll(BumpAllocator* transientStorage)
{
    static void* gameDLL;
    static long long lastEditTimestampGameDLL;
  
    long long currentTimestampGameDLL = get_timestamp(gameLibName);
    if(currentTimestampGameDLL > lastEditTimestampGameDLL)
    {
      if(gameDLL)
      {
        bool freeResult = platform_free_dynamic_library(gameDLL);
        SM_ASSERT(freeResult, "Failed to free %s", gameLibName);
        gameDLL = nullptr;
        SM_TRACE("Freed %s", gameLibName);
      }
  
      while(!copy_file(gameLibName, gameLoadLibName, transientStorage))
      {
        platform_sleep(10);
      }
      SM_TRACE("Copied %s into %s", gameLibName, gameLoadLibName);
  
      gameDLL = platform_load_dynamic_library(gameLoadLibName);
      SM_ASSERT(gameDLL, "Failed to load %s", gameLoadLibName);
  
      update_game_ptr = (update_game_type*)platform_load_dynamic_function(gameDLL, "update_game");
      SM_ASSERT(update_game_ptr, "Failed to load update_game function");
      lastEditTimestampGameDLL = currentTimestampGameDLL;
    }
}