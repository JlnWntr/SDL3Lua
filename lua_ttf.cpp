#include <SDL3_ttf/SDL_ttf.h>

TTF_Font *Font;

int Lua_Create_Text_Label(lua_State *L) {
  if (not Renderer or not Font or not L) {
    lua_pushlightuserdata(L, nullptr);
    return 1;
  }

  SDL_Color Color_foreground{255, 255, 255, 255};

  SDL_Surface *const surface{
    TTF_RenderText_Blended_Wrapped(Font, lua_tostring(L, 1), 0, Color_foreground, lua_tointeger(L, 2))
  };
  
  if (not surface) {
    SDL_Log("Could not create font surface: %s", SDL_GetError());
    lua_pushlightuserdata(L, nullptr);
    return 1;
  }
  SDL_Texture *texture {SDL_CreateTextureFromSurface(Renderer, surface)};
  SDL_DestroySurface(surface);

  if (not texture) {
    SDL_Log("Could not create font texture: %s", SDL_GetError());
  
    lua_pushlightuserdata(L, nullptr);
  }
  else
    lua_pushlightuserdata(L, texture);
  return 1;
}


int Lua_Load_Font(lua_State *L) {
  bool result{false};
  if (L) {
    size_t size{(size_t)lua_tointeger(L, 2)};
    if (size < 1) size = 1;
  
    Font = TTF_OpenFont(lua_tostring(L, 1), size);
    if (not Font) 
      SDL_Log("Could not load font: %s", SDL_GetError());
    else  
      result = true;
  }
  lua_pushboolean(L, result);
  return 1;
}
