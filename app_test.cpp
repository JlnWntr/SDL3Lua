/*
g++ -o app_test app_test.cpp -lSDL3 -llua -ldl -Wl,-rpath,/usr/local/lib -Wall -std=c++17

*/

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

//#define LUA_ADAPTER_DEBUG
#include "./Lua-Adapter/LuaAdapter.hpp"

LuaAdapter lua{};
constexpr char DEFAULT_ORGANISATION[] = {"jlnwntr"};
constexpr char DEFAULT_TITLE[] = {"SDL3Lua"};
SDL_Window *Window{nullptr};
SDL_Renderer *Renderer{nullptr};
SDL_AudioStream *Stream{nullptr};

constexpr unsigned short int DEFAULT_WINDOW_WIDTH{1179};
constexpr unsigned short int DEFAULT_WINDOW_HEIGHT{2556};
int Window_width{DEFAULT_WINDOW_WIDTH};
int Window_height{DEFAULT_WINDOW_HEIGHT};
int Window_pixelwidth{DEFAULT_WINDOW_WIDTH};
int Window_pixelheight{DEFAULT_WINDOW_HEIGHT};
float Window_scale{1.0};

#include "lua_render.cpp"
#include "lua_system.cpp"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if ((lua.PushFunction(Lua_Set_Color, "Lua_Set_Color") == false) or
      (lua.PushFunction(Lua_Print, "print") == false) or
      (lua.PushFunction(Lua_Delay, "delay") == false) or
      (lua.PushFunction(Lua_Load_BMP, "Lua_Load_BMP") == false) or
      (lua.PushFunction(Lua_Render_Texture, "Lua_Render_Texture") == false) or
      (lua.PushFunction(Lua_Get_Size_Texture, "Lua_Get_Size_Texture") == false) 
  )  {
    SDL_Log("Could not push c-function(s) to Lua");
    return SDL_APP_FAILURE;
  }
  if ((argc <= 1) or (lua.Load(argv[1]) == false)) {
    SDL_Log("Could not load lua code.");
    if (argc <= 1)
      SDL_Log("\tUsage: %s \"your_app.lua\"", argv[0]);
    return SDL_APP_FAILURE;
  }

  if (SDL_Init(SDL_INIT_VIDEO /*| SDL_INIT_AUDIO */) == false) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  
  if ((lua.Get("WINDOW_WIDTH", Window_width) == false) or (lua.Get("WINDOW_HEIGHT", Window_height) == false))
    SDL_Log("Could not load 'WINDOW_WIDTH' or 'WINDOW_HEIGHT' from Lua: %s", SDL_GetError());


  if (SDL_CreateWindowAndRenderer(DEFAULT_TITLE, Window_width, Window_height, SDL_WINDOW_HIGH_PIXEL_DENSITY, &Window, &Renderer) == false) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GetWindowSize(Window, &Window_width, &Window_height);
  SDL_GetWindowSizeInPixels(Window, &Window_pixelwidth, &Window_pixelheight);
  SDL_Log("Window size: %ix%i", Window_width, Window_height);
  SDL_Log("Backbuffer size: %ix%i", Window_pixelwidth, Window_pixelheight);
  if (Window_width != Window_pixelwidth) {
    SDL_Log("\tThis looks like a high-dpi environment.");
    Window_scale = Window_pixelwidth / Window_width;
  }

  if ((lua.Set("WINDOW_WIDTH", Window_pixelwidth) == false) or
      (lua.Set("WINDOW_HEIGHT", Window_pixelheight) == false))
    SDL_Log("Could not set window-dimensions for lua: %s", SDL_GetError());

  if (SDL_SetRenderVSync(Renderer, 1) == false)
    SDL_Log("Could not activate vsync: %s", SDL_GetError());

  lua.Call("APP_INIT");
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  int result{};
  switch (event->type) {
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    return SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
    return SDL_APP_SUCCESS;
    break;
  case SDL_EVENT_KEY_UP:
    switch (event->key.key) {
    case SDLK_ESCAPE:
      return SDL_APP_SUCCESS;
      break;
    default:
      lua.Call("APP_KEY_UP", (int)event->key.key);
    }
    break;
  case SDL_EVENT_KEY_DOWN:
    lua.Call("APP_KEY_DOWN", (int)event->key.key);
    break;
 
  case SDL_EVENT_MOUSE_BUTTON_DOWN: {
    const float mouse[] = {event->button.x * Window_scale,
                           event->button.y * Window_scale};
    lua.Call("APP_MOUSE_DOWN", 2, mouse, result);
  } break;
  case SDL_EVENT_MOUSE_BUTTON_UP: {
    const float mouse[] = {event->button.x * Window_scale,
                           event->button.y * Window_scale};
    lua.Call("APP_MOUSE_UP", 2, mouse, result);
  } break;
  }
  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_SetRenderDrawColor(Renderer, 0, 12, 20, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(Renderer);
  lua.Call("APP_ITERATION");
  SDL_RenderPresent(Renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
}
