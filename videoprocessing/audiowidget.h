#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

class MyAudioWidget {
public:
  MyAudioWidget() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      std::cout << "SDL_Init error:" << SDL_GetError() << std::endl;
    }
  };
  ~MyAudioWidget() { SDL_AudioQuit(); };
  auto rcvInfo(int pFreq, Uint16 pFormat, Uint8 pChannels, Uint16 pSamples,
               Uint32 pBytes, Uint32 pPadding, Uint32 pSize) {}

  auto play() { SDL_OpenAudio(&audioSpec, &spec); }

private:
  SDL_AudioSpec audioSpec, spec;
};