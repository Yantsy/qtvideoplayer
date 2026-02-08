#pragma once
#include <iostream>
extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <libswresample/swresample.h>
}

class MyAudioWidget {
public:
  auto init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      std::cerr << "Can't init SDL\n";
    }
    std::cout << "Init SDL_Audio success\n";
    return 0;
  };
  auto open(SDL_AudioSpec *pwanted, SDL_AudioSpec *pobtained) {
    if (SDL_OpenAudio(pwanted, pobtained) < 0) {
      std::cerr << "Can't open audio\n";
      return -1;
    }
    std::cout << "Open audio success\n";
    return 0;
  };
  auto pause();
  auto callBack(void *puserData, Uint8 *pstream, int plen) {
    SDL_AudioCallback paudioCallback;
    paudioCallback(puserData, pstream, plen);
    return paudioCallback;
  };
  auto buffer(int pchannels, int psamples, AVSampleFormat psprFmt) {
    auto pBufSize = psamples * pchannels;
    switch (psprFmt) {
    case AV_SAMPLE_FMT_S16: {
      pBufSize *= 2;
    } break;
    case AV_SAMPLE_FMT_S32: {
      pBufSize *= 4;
    } break;
    case AV_SAMPLE_FMT_FLT: {
      pBufSize *= 4;
    } break;
    default:
      pBufSize *= 1;
      break;
    }
    return pBufSize;
  };
  auto close();
};
