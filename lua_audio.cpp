SDL_AudioSpec spec;
  spec.channels = 1;
  spec.format = SDL_AUDIO_F32;
  spec.freq = 8000;
  Stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                     NULL, NULL);
  if (not Stream) {
    SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_ResumeAudioStreamDevice(Stream);



int Lua_Get_Queue_Audio_Length(lua_State *L) {
  if (not L)
    return 0;
  lua_pushnumber(L, SDL_GetAudioStreamQueued(Stream) / sizeof(float));
  return 1;
}

int Lua_Queue_Audio(lua_State *L) {
  if (not L)
    return 0;

  const size_t a{(size_t)lua_tointeger(L, 2)};
  if (a > 1) {
    // SDL_Log("Got %d values for audio buffer", a);
    float *s{(float *)SDL_calloc(a, sizeof(float))};
    for (auto i = 1; i <= a; i++) {
      lua_rawgeti(L, 1, i);
      s[i - 1] = (float)lua_tonumber(L, -1);
      lua_pop(L, 1);
      // SDL_Log("%f", s[i-1]);
    }

    SDL_PutAudioStreamData(Stream, s, a * sizeof(float));
    // SDL_FlushAudioStream(Stream);
    SDL_free(s);
  }
  return Lua_Get_Queue_Audio_Length(L);
}

int Lua_Dequeue_Audio(lua_State *L) {
  SDL_ClearAudioStream(Stream);
  return 0;
}
