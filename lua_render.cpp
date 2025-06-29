
int Lua_Draw_Text(lua_State *L) {
  if (not L)
    return 0;
  const float x{(float)lua_tonumber(L, 1)};
  const float y{(float)lua_tonumber(L, 2)};
  const char *const text{lua_tostring(L, 3)};
  SDL_RenderDebugTextFormat(Renderer, x, y, "%s", text); // TODO format???
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

int Lua_Set_Render_Scale(lua_State *L) {
  if (not L)
    return 0;
  const float scale{(float)lua_tonumber(L, 1)};
  SDL_SetRenderScale(Renderer, scale, scale);
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

int Lua_Render_Texture(lua_State *L) {
 
  if (not L) {
    return 0;
  }

  SDL_Texture *const texture{(SDL_Texture *)lua_touserdata(L, 1)}; 

  if (not texture) {
    return 0;
  }

  const SDL_FRect blit{(float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3),
                       (float)lua_tonumber(L, 4), (float)lua_tonumber(L, 5)};
  const SDL_FRect blot{(float)lua_tonumber(L, 6), (float)lua_tonumber(L, 7),
                       (float)lua_tonumber(L, 8), (float)lua_tonumber(L, 9)};
  // TODO: I'm not happy with all these float casts in this entire file.

  SDL_RenderTexture(Renderer, texture, &blit, &blot); // SDL_RenderTextureRotated(renderer, texture, NULL, // &dst_rect, rotation, &center, SDL_FLIP_NONE);

  return 0;
}


int Lua_Destroy_Texture(lua_State *L) {
  if (not L)  return 0;
  bool result = false;
  SDL_Texture *const texture{(SDL_Texture *)lua_touserdata(L, 1)};
  if (texture){
    SDL_DestroyTexture(texture);
    result = true;
  }
  lua_pushboolean(L, result);
  return 1;
}


int Lua_Get_Size_Texture(lua_State *L) {
  if (not L)  return 0;
  SDL_Texture *const texture{(SDL_Texture *)lua_touserdata(L, 1)};
  if (not texture) return 0;
  float w, h;
  if (SDL_GetTextureSize(texture, &w, &h) == true){
    lua_pushnumber(L, w);
    lua_pushnumber(L, h);
    return 2;
  }
  return 0;
}


int Lua_Render_Texture_Rotated(lua_State *L) {
 
  if (not L) {
    return 0;
  }

  SDL_Texture *const texture{(SDL_Texture *)lua_touserdata(L, 1)}; 

  if (not texture) {
    return 0;
  }

  const SDL_FRect blit{(float)lua_tonumber(L, 2), (float)lua_tonumber(L, 3),
                       (float)lua_tonumber(L, 4), (float)lua_tonumber(L, 5)};
  const SDL_FRect blot{(float)lua_tonumber(L, 6), (float)lua_tonumber(L, 7),
                       (float)lua_tonumber(L, 8), (float)lua_tonumber(L, 9)};
  const double angle{lua_tonumber(L, 10)};
  SDL_RenderTextureRotated(Renderer, texture, &blit, &blot, angle, NULL,
                           SDL_FLIP_NONE);

  return 0;
}

int Lua_Load_BMP(lua_State *L) {
  if (not L)
    return 0;

  SDL_Texture *texture{nullptr};

  char *bmp_path;
  SDL_asprintf(&bmp_path, "%s%s", SDL_GetBasePath(), lua_tostring(L, 1));
  SDL_Surface *surface{SDL_LoadBMP(bmp_path)};
  if (not surface) {
    SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
    
  } else {
    //Texture_width = surface->w;
    //Texture_height = surface->h;
    SDL_SetSurfaceColorKey( surface, true, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 0xFF, 0x00, 0xFF)); // Pink-pixels are transparent

    texture = SDL_CreateTextureFromSurface(Renderer, surface); 
    
    if (not texture) {
      SDL_Log("Couldn't create static texture: %s", SDL_GetError());
      
    } else {
      SDL_DestroySurface(surface);
      /*SDL_SetTextureScaleMode(
          Texture_data,
          SDL_SCALEMODE_NEAREST); // Textures are scaled "pixelated".
    */}
  }
  SDL_free(bmp_path);
  lua_pushlightuserdata(L, texture);
  
  return 1;
}

