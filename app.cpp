#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define LUA_ADAPTER_DEBUG
#include "./Lua-Adapter/LuaAdapter.hpp"
LuaAdapter lua{};

SDL_Window *Window{nullptr};
SDL_Renderer *Renderer{nullptr};

int Texture_width{};
int Texture_height{};
SDL_Texture *Texture_data{nullptr};

constexpr unsigned short int DEFAULT_WINDOW_WIDTH{1179};
constexpr unsigned short int DEFAULT_WINDOW_HEIGHT{2556};

int Lua_Draw_Text(lua_State *L) {
  if (not L)
    return 0;
  const float x{(float)lua_tonumber(L, 1)};
  const float y{(float)lua_tonumber(L, 2)};
  const char *const text{lua_tostring(L, 3)};
  SDL_RenderDebugTextFormat(Renderer, x, y, text);
  return 0;
}

int Lua_Set_Color(lua_State *L) {
  if (not L)
    return 0;
  const unsigned char r{(unsigned char)(lua_tointeger(L, 1))};
  const unsigned char g{(unsigned char)(lua_tointeger(L, 2))};
  const unsigned char b{(unsigned char)(lua_tointeger(L, 3))};
  const unsigned char a{(unsigned char)(lua_tointeger(L, 4))};
  SDL_SetRenderDrawColor(Renderer, r, g, b, a);
  return 0;
}

int Lua_Fill_Rect(lua_State *L) {
  if (not L)
    return 0;
  const SDL_FRect rect{(float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                       (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4)};
  SDL_RenderFillRect(Renderer, &rect);
  return 0;
}

int Lua_Draw_Line(lua_State *L) {
  if (not L)
    return 0;
  SDL_RenderLine(Renderer, (float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                 (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4));
  return 0;
}

int Lua_Draw_Rect(lua_State *L) {
  if (not L)
    return 0;
  const SDL_FRect rect{(float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                       (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4)};
  SDL_RenderRect(Renderer, &rect);
  return 0;
}

int Lua_Render_Sprite(lua_State *L) {
  if (not Texture_data)
    return 0;

  const SDL_FRect blit{(float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                       (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4)};
  const SDL_FRect blot{(float)lua_tonumber(L, 5), (float)lua_tonumber(L, 6),
                       (float)lua_tonumber(L, 7), (float)lua_tonumber(L, 8)};
  SDL_RenderTexture(Renderer, Texture_data, &blit, &blot);
  return 0;
}

int Lua_Load_Spritelibrary(lua_State *L) {
  if (not L)
    return 0;

  bool result = true;
  char *bmp_path;
  SDL_asprintf(&bmp_path, "%s%s", SDL_GetBasePath(), lua_tostring(L, 1));
  SDL_Surface *surface{SDL_LoadBMP(bmp_path)};
  if (not surface) {
    SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
    result = false;
  } else {
    Texture_width = surface->w;
    Texture_height = surface->h;
    SDL_SetSurfaceColorKey(
        surface, true,
        SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 0xFF, 0x00,
                   0xFF)); // Pink-pixels are transparent

    Texture_data = SDL_CreateTextureFromSurface(Renderer, surface);
    if (not Texture_data) {
      SDL_Log("Couldn't create static texture: %s", SDL_GetError());
      result = false;
    } else {
      SDL_DestroySurface(surface);
      SDL_SetTextureScaleMode(
          Texture_data,
          SDL_SCALEMODE_NEAREST); // Textures are scaled "pixelated".
    }
  }
  SDL_free(bmp_path);
  lua_pushboolean(L, result);
  return 1;
}

int Lua_Delay(lua_State *L) {
  if (!L)
    return 0;
  struct timespec time_spec;
  time_spec.tv_sec = 0;
  time_spec.tv_nsec = lua_tointeger(L, 1) * 1000000;
  nanosleep(&time_spec, nullptr);
  return 0;
}

int Lua_Print(lua_State *L) {
  if (!L)
    return 0;
  const char *const text{lua_tostring(L, 1)};
  SDL_Log("(lua) %s", text);
  return 0;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if ((lua.PushFunction(Lua_Set_Color, "Lua_Set_Color") == false) or
      (lua.PushFunction(Lua_Print, "print") == false) or
      (lua.PushFunction(Lua_Delay, "delay") == false) or
      (lua.PushFunction(Lua_Load_Spritelibrary, "Lua_Load_Spritelibrary") ==
       false) or
      (lua.PushFunction(Lua_Render_Sprite, "Lua_Render_Sprite") == false) or
      (lua.PushFunction(Lua_Draw_Rect, "Lua_Draw_Rect") == false) or
      (lua.PushFunction(Lua_Draw_Line, "Lua_Draw_Line") == false) or
      (lua.PushFunction(Lua_Draw_Text, "Lua_Draw_Text") == false)) {
    SDL_Log("Could not push c-function(s) to Lua");
    return SDL_APP_FAILURE;
  }

  if (lua.Load("app.lua") == false) {
    SDL_Log("Could not load 'app.lua'");
    return SDL_APP_FAILURE;
  }

  SDL_SetAppMetadata("Example Renderer Debug Texture", "1.0",
                     "com.example.renderer-debug-text");

  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  int window_width{DEFAULT_WINDOW_WIDTH};
  if (lua.Get("WINDOW_WIDTH", window_width) == false)
    SDL_Log("Could not load 'WINDOW_WIDTH' from Lua");
  int window_height{DEFAULT_WINDOW_HEIGHT};
  if (lua.Get("WINDOW_HEIGHT", window_height) == false)
    SDL_Log("Could not load 'WINDOW_HEIGHT' from Lua");

  if (SDL_CreateWindowAndRenderer(
          "examples/renderer/debug-text", window_width, window_height,
          0, &Window,
          &Renderer) == false) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

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
    }
    break;
  case SDL_EVENT_KEY_DOWN:
    break;
  case SDL_EVENT_FINGER_DOWN: {
    const float mouse[] = {event->tfinger.x, event->tfinger.y};
    lua.Call("APP_MOUSE_DOWN", 2, mouse, result);
  } break;
  case SDL_EVENT_FINGER_UP: {
    const float mouse[] = {event->tfinger.x, event->tfinger.y};
    lua.Call("APP_MOUSE_UP", 2, mouse, result);
  } break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN: {
    const float mouse[] = {event->button.x, event->button.y};
    lua.Call("APP_MOUSE_DOWN", 2, mouse, result);
  } break;
  case SDL_EVENT_MOUSE_BUTTON_UP: {
    const float mouse[] = {event->button.x, event->button.y};
    lua.Call("APP_MOUSE_UP", 2, mouse, result);
  } break;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  SDL_SetRenderDrawColor(Renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(Renderer);
  lua.Call("APP_ITERATION");
  SDL_RenderPresent(Renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
