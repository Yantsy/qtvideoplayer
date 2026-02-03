#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

class MyAudioWidget {
public:
  auto init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      std::cerr << "Can't init SDL\n" << std::endl;
    }
    std::cout << "Init SDL_Audio success\n" << std::endl;
    return 0;
  };
  auto open(SDL_AudioSpec *pwanted, SDL_AudioSpec *pobtained) {
    if (SDL_OpenAudio(pwanted, pobtained) < 0) {
      std::cerr << "Can't open audio\n" << std::endl;
      return -1;
    }
    std::cout << "Open audio success\n" << std::endl;
    return 0;
  };
  auto pause();
  auto callBack(void *puserData, Uint8 *pstream, int plen) {
    SDL_AudioCallback paudioCallback;
    paudioCallback(puserData, pstream, plen);
    return paudioCallback;
  };
  auto close();
};
