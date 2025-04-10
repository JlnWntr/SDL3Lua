/*This is a growing "library" of useful C(++)-functions which you can call from Lua.
Compile on mac with something like:
g++ -o app_mac SDL3Lua/app.cpp -lSDL3 -llua -ldl -Wl,-rpath,/usr/local/lib -Wall -std=c++17

*/

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define LUA_ADAPTER_DEBUG
#include "./Lua-Adapter/LuaAdapter.hpp"
LuaAdapter lua{};
constexpr char DEFAULT_ORGANISATION[] = {"jlnwntr"};
constexpr char DEFAULT_TITLE[] = {"SDL3Lua"};

SDL_Window *Window{nullptr};
SDL_Renderer *Renderer{nullptr}; 
SDL_AudioStream *Stream{nullptr}; 

int Texture_width{};
int Texture_height{};
SDL_Texture *Texture_data{nullptr};

constexpr unsigned short int DEFAULT_WINDOW_WIDTH{1179};
constexpr unsigned short int DEFAULT_WINDOW_HEIGHT{2556};
int Window_width{DEFAULT_WINDOW_WIDTH};
int Window_height{DEFAULT_WINDOW_HEIGHT};
int Window_pixelwidth{DEFAULT_WINDOW_WIDTH};
int Window_pixelheight{DEFAULT_WINDOW_HEIGHT};
float Window_scale {1.0};

/*
int Lua_Queue_Audio(lua_State *L) {
  if (not L)
    return 0;
    
  lua_len(L, -1);//assumes table, get its length
  const int a {(int)lua_tointeger(L, -1)};
  lua_pop(L, 1);
  //SDL_Log("Got %d values.", a);
  float s[a];
  for (int i=1; i<=a; i++){
    lua_rawgeti(L, -1, i);
    s[i-1] = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);
    //SDL_Log("%f", s[i-1]);
  }
  SDL_Log("%d", SDL_arraysize(s));
  SDL_Log("%d", a * sizeof(float));
  SDL_Log("%d", sizeof(s));
  SDL_PutAudioStreamData(Stream, s, sizeof(s));
  SDL_FlushAudioStream(Stream);
  lua_pushinteger(L, SDL_GetAudioStreamQueued(Stream));
  return 1;
}
*/
int Lua_Queue_Audio(lua_State *L) {
  if (not L)
    return 0;
    
  lua_len(L, -1);//assumes table, get its length
  const size_t a {(size_t)lua_tointeger(L, -1)};
  lua_pop(L, 1);
  //SDL_Log("Got %d values.", a);
  float *s = (float *)SDL_calloc(a, sizeof(float));
  for (int i=1; i<=a; i++){
    lua_rawgeti(L, -1, i);
    s[i-1] = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);
    //SDL_Log("%f", s[i-1]);
  }
  
  SDL_PutAudioStreamData(Stream, s, a * sizeof(float));
  SDL_FlushAudioStream(Stream);
  SDL_free(s);
  lua_pushinteger(L, SDL_GetAudioStreamQueued(Stream));
  return 1;
}

int Lua_Dequeue_Audio(lua_State *L) {
  SDL_ClearAudioStream(Stream);
  return 0;
}

int Lua_Draw_Text(lua_State *L) {
  if (not L)
    return 0;
  const float x{(float)lua_tonumber(L, 1)};
  const float y{(float)lua_tonumber(L, 2)};
  const char *const text{lua_tostring(L, 3)};
  SDL_RenderDebugTextFormat(Renderer, x, y, "%s",text); // TODO format???
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

int Lua_Set_Scale(lua_State *L) {
  if (not L)
    return 0;
  const float scale {(float)lua_tonumber(L, 1)};
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
  if (not Texture_data)
    return 0;

  // TODO: I'm not happy with all these float casts in this entire file.

  const SDL_FRect blit{(float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                       (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4)};
  const SDL_FRect blot{(float)lua_tonumber(L, 5), (float)lua_tonumber(L, 6),
                       (float)lua_tonumber(L, 7), (float)lua_tonumber(L, 8)};
  SDL_RenderTexture(Renderer, Texture_data, &blit, &blot); // SDL_RenderTextureRotated(renderer, texture, NULL, &dst_rect, rotation, &center, SDL_FLIP_NONE);

  return 0;
}

int Lua_Render_Texture_Rotated(lua_State *L) {
  if (not Texture_data)
    return 0;
  const SDL_FRect blit{(float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2),
                       (float)lua_tonumber(L, 3), (float)lua_tonumber(L, 4)};
  const SDL_FRect blot{(float)lua_tonumber(L, 5), (float)lua_tonumber(L, 6),
                       (float)lua_tonumber(L, 7), (float)lua_tonumber(L, 8)};
  const double angle {lua_tonumber(L, 9)};
  SDL_RenderTextureRotated(Renderer, Texture_data, &blit, &blot, angle, NULL, SDL_FLIP_NONE);

  return 0;
}

int Lua_Load_BMP(lua_State *L) {
  if (not L) return 0;

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
      /*SDL_SetTextureScaleMode(
          Texture_data,
          SDL_SCALEMODE_NEAREST); // Textures are scaled "pixelated".
    */}
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

/*
void ReadGameData(void)
{
    extern char** fileNames;
    extern size_t numFiles;

    SDL_Storage *title = SDL_OpenTitleStorage(NULL, 0);
    if (title == NULL) {
        // Something bad happened!
    }
    while (!SDL_StorageReady(title)) {
        SDL_Delay(1);
    }

    for (size_t i = 0; i < numFiles; i += 1) {
        void* dst;
        Uint64 dstLen = 0;

        if (SDL_GetStorageFileSize(title, fileNames[i], &dstLen) && dstLen > 0) {
            dst = SDL_malloc(dstLen);
            if (SDL_ReadStorageFile(title, fileNames[i], dst, dstLen)) {
                // A bunch of stuff happens here
            } else {
                // Something bad happened!
            }
            SDL_free(dst);
        } else {
            // Something bad happened!
        }
    }

    SDL_CloseStorage(title);
}
//*/

int Lua_Read_File(lua_State *L) {
  if (not L)
    return 0;
  
  const char *const filename{lua_tostring(L, 1)};

  SDL_Storage *user{
      SDL_OpenUserStorage(DEFAULT_ORGANISATION, DEFAULT_TITLE, 0)};
  if (user == nullptr) {
    SDL_Log("Couldn't open user storage: %s", SDL_GetError());
    lua_pushlstring(L, "", 1); return 1;
  }

  unsigned int c{0};
  while ((SDL_StorageReady(user) == false) and (c < 10)) {
    SDL_Delay(1);//wait 1 second
    c++;
  }

  void *data{nullptr};
  Uint64 length{0};
  bool success {false};

  if ((SDL_GetStorageFileSize(user, filename, &length) == true) and (length > 0)) {
      data = SDL_calloc(length, sizeof(char));
      if (SDL_ReadStorageFile(user, filename, data, length) == true)
        success = true;
      else
        SDL_Log("Couldn't read file: %s", SDL_GetError());
  } else
    SDL_Log("Couldn't determine user storage size: %s", SDL_GetError());
  
  if (success == true)
    lua_pushlstring(L, (char *)(data), length);
  else 
    lua_pushlstring(L, "", 1);

  SDL_CloseStorage(user);
  if (length > 0) 
    SDL_free(data);
  return 1;
}

int Lua_Write_File(lua_State *L) {
  if (not L)
    return 0;

  //SDL_Log("Write to pref-path: %s", SDL_GetPrefPath(DEFAULT_ORGANISATION, DEFAULT_TITLE));

  SDL_Storage *user{
    SDL_OpenUserStorage(DEFAULT_ORGANISATION, DEFAULT_TITLE, 0)};
  if (user == nullptr) {
    SDL_Log("Couldn't open user storage: %s", SDL_GetError());
    lua_pushboolean(L, false); return 1;
  }

  unsigned int c{0};
  while ((SDL_StorageReady(user) == false) and (c < 10)) {
    SDL_Delay(1);
    c++;
  }

  const char *const filename{lua_tostring(L, 1)};
  
  size_t length{0};
  const char *const data{lua_tolstring(L, 2, &length)};
  
  if ((length > 0) and (SDL_WriteStorageFile(user, filename, data, length) == false)) {
    SDL_Log("Couldn't write file: %s", SDL_GetError());
    lua_pushboolean(L, false); return 1;
  }
  SDL_CloseStorage(user);
  lua_pushboolean(L, true);
  return 1;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if ((lua.PushFunction(Lua_Set_Color, "Lua_Set_Color") == false) or
      (lua.PushFunction(Lua_Set_Scale, "Lua_Set_Scale") == false) or
      (lua.PushFunction(Lua_Queue_Audio, "Lua_Queue_Audio") == false) or
      (lua.PushFunction(Lua_Dequeue_Audio, "Lua_Dequeue_Audio") == false) or
      (lua.PushFunction(Lua_Print, "print") == false) or
      (lua.PushFunction(Lua_Delay, "delay") == false) or
      (lua.PushFunction(Lua_Load_BMP, "Lua_Load_BMP") == false) or
      (lua.PushFunction(Lua_Render_Texture, "Lua_Render_Texture") == false) or
      (lua.PushFunction(Lua_Render_Texture_Rotated, "Lua_Render_Texture_Rotated") == false) or
      (lua.PushFunction(Lua_Draw_Rect, "Lua_Draw_Rect") == false) or
      (lua.PushFunction(Lua_Fill_Rect, "Lua_Fill_Rect") == false) or
      (lua.PushFunction(Lua_Write_File, "Lua_Write_File") == false) or
      (lua.PushFunction(Lua_Read_File, "Lua_Read_File") == false) or
      (lua.PushFunction(Lua_Draw_Line, "Lua_Draw_Line") == false) or
      (lua.PushFunction(Lua_Draw_Text, "Lua_Draw_Text") == false)) {
    SDL_Log("Could not push c-function(s) to Lua");
    return SDL_APP_FAILURE;
  }

  if (lua.Load("lua/app.lua") == false) {
    SDL_Log("Could not load 'app.lua'");
    return SDL_APP_FAILURE;
  }

  SDL_SetAppMetadata(DEFAULT_TITLE, "1.0", "com.example.lua-sdl");

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
 //*
  SDL_AudioSpec spec;
  spec.channels = 1;
  spec.format = SDL_AUDIO_F32;
  spec.freq = 8000;
  Stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
  if (not Stream) {
      SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
      return SDL_APP_FAILURE;
  }
  SDL_ResumeAudioStreamDevice(Stream);
//*/
  if (lua.Get("WINDOW_WIDTH", Window_width) == false)
    SDL_Log("Could not load 'WINDOW_WIDTH' from Lua");
  int Window_height{DEFAULT_WINDOW_HEIGHT};
  if (lua.Get("WINDOW_HEIGHT", Window_height) == false)
    SDL_Log("Could not load 'WINDOW_HEIGHT' from Lua");

  if (SDL_CreateWindowAndRenderer(DEFAULT_TITLE, Window_width, Window_height, SDL_WINDOW_HIGH_PIXEL_DENSITY, &Window, &Renderer) == false) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GetWindowSize(Window, &Window_width, &Window_height);
  SDL_GetWindowSizeInPixels(Window, &Window_pixelwidth, &Window_pixelheight);
  SDL_Log("Window size: %ix%i", Window_width, Window_height);
  SDL_Log("Backbuffer size: %ix%i", Window_pixelwidth, Window_pixelheight);
  if (Window_width != Window_pixelwidth) {
      SDL_Log("\tThis is a high-dpi environment.");
      Window_scale = Window_pixelwidth / Window_width;
  }
  
  if ((lua.Set("WINDOW_WIDTH", Window_pixelwidth) == false) or (lua.Set("WINDOW_HEIGHT", Window_pixelheight) == false))
    SDL_Log("Could not set window-dimensions for lua");
  
  if(SDL_SetRenderVSync(Renderer, 1) == false)
    SDL_Log("Could not activate vsync.");

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
      //SDL_Log("%d", event->key.key);
      lua.Call("APP_KEY_DOWN", (int)event->key.key);
    break;
  case SDL_EVENT_FINGER_DOWN: {
    const float mouse[] = {event->tfinger.x * Window_pixelwidth, event->tfinger.y * Window_pixelheight};
    lua.Call("APP_MOUSE_DOWN", 2, mouse, result);
  } break;
  case SDL_EVENT_FINGER_UP: {
    const float mouse[] = {event->tfinger.x * Window_pixelwidth, event->tfinger.y * Window_pixelheight};
    lua.Call("APP_MOUSE_UP", 2, mouse, result);
  } break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN: {
    const float mouse[] = {event->button.x * Window_scale, event->button.y * Window_scale};
    lua.Call("APP_MOUSE_DOWN", 2, mouse, result);
  } break;
  case SDL_EVENT_MOUSE_BUTTON_UP: {
    const float mouse[] = {event->button.x * Window_scale, event->button.y * Window_scale};
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

void SDL_AppQuit(void *appstate, SDL_AppResult result){
   // SDL_free(wav_data);  
}
