
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

        if (SDL_GetStorageFileSize(title, fileNames[i], &dstLen) && dstLen > 0)
{ dst = SDL_malloc(dstLen); if (SDL_ReadStorageFile(title, fileNames[i], dst,
dstLen)) {
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
/*
int Lua_Haptic(lua_State *L) {
  SDL_Haptic *haptic{nullptr};
  int count {0};
  SDL_HapticID *haptics {SDL_GetHaptics(&count)};
  if (haptics) 
    haptic = SDL_OpenHaptic(haptics[0]);

  SDL_free(haptics);
  SDL_Log("Found %d haptic device(s).", count);

  if (haptic == NULL)
    return 1;

  if (SDL_InitHapticRumble(haptic) == false)
    return 1;
  
  if (SDL_PlayHapticRumble(haptic, 0.5, 2000) == false)
    return 1;
   
  SDL_Delay(2000);
  SDL_CloseHaptic(haptic);

  return 1; 
}*/

int Lua_Read_File(lua_State *L) {
  if (not L)
    return 0;

  const char *const filename{lua_tostring(L, 1)};

  SDL_Storage *user{
      SDL_OpenUserStorage(DEFAULT_ORGANISATION, DEFAULT_TITLE, 0)};
  if (user == nullptr) {
    SDL_Log("Couldn't open user storage: %s", SDL_GetError());
    lua_pushlstring(L, "", 1);
    return 1;
  }

  unsigned int c{0};
  while ((SDL_StorageReady(user) == false) and (c < 10)) {
    SDL_Delay(1); // wait 1 second
    c++;
  }

  void *data{nullptr};
  Uint64 length{0};
  bool success{false};

  if ((SDL_GetStorageFileSize(user, filename, &length) == true) and
      (length > 0)) {
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

  // SDL_Log("Write to pref-path: %s", SDL_GetPrefPath(DEFAULT_ORGANISATION,
  // DEFAULT_TITLE));

  SDL_Storage *user{
      SDL_OpenUserStorage(DEFAULT_ORGANISATION, DEFAULT_TITLE, 0)};
  if (user == nullptr) {
    SDL_Log("Couldn't open user storage: %s", SDL_GetError());
    lua_pushboolean(L, false);
    return 1;
  }

  unsigned int c{0};
  while ((SDL_StorageReady(user) == false) and (c < 10)) {
    SDL_Delay(1);
    c++;
  }

  const char *const filename{lua_tostring(L, 1)};

  size_t length{0};
  const char *const data{lua_tolstring(L, 2, &length)};

  if ((length > 0) and
      (SDL_WriteStorageFile(user, filename, data, length) == false)) {
    SDL_Log("Couldn't write file: %s", SDL_GetError());
    lua_pushboolean(L, false);
    return 1;
  }
  SDL_CloseStorage(user);
  lua_pushboolean(L, true);
  return 1;
}
