#pragma once
#include <iostream>
extern "C" {
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <libswresample/swresample.h>
}

class MyResampler {
public:
  auto alcSwrCtx() noexcept {
    auto pSwrCtx = swr_alloc();
    return pSwrCtx;
  }
  auto detectFmt(AVSampleFormat psprFmt, int pChannels) {
    if (psprFmt == AV_SAMPLE_FMT_NONE) {
      std::cout << "Can't find supported sample format\n" << std::endl;
      // exit(-1);
    } else if (psprFmt == AV_SAMPLE_FMT_U8P || psprFmt == AV_SAMPLE_FMT_S16P ||
               psprFmt == AV_SAMPLE_FMT_S32P || psprFmt == AV_SAMPLE_FMT_S64P ||
               psprFmt == AV_SAMPLE_FMT_FLTP || psprFmt == AV_SAMPLE_FMT_DBLP ||
               pChannels > 1) {
      isPlannar = true;
    } else {
    }
    std::cout << "Sample format:" << psprFmt << "\n" << std::endl;
    return 0;
  }
  auto plannarTpacked(uint8_t pChnllData) {
    uint8_t pPackedData;
    if (isPlannar) {
      std::cout << "From plannar to packed success\n" << std::endl;
    } else {
      std::cout << "No need to resample\n" << std::endl;
    }
    return pPackedData;
  }

  auto fmtNameTrans(AVSampleFormat psprFmt) {
    switch (psprFmt) {
    case AV_SAMPLE_FMT_U8: {
      return AUDIO_U8;
    } break;
    case AV_SAMPLE_FMT_U8P: {
      return AUDIO_U8;
    } break;
    case AV_SAMPLE_FMT_S16: {
      return AUDIO_S16;
    } break;
    case AV_SAMPLE_FMT_S16P: {
      return AUDIO_S16;
    } break;
    case AV_SAMPLE_FMT_S32: {
      return AUDIO_S32;
    } break;
    case AV_SAMPLE_FMT_S32P: {
      return AUDIO_S32;
    } break;
    default:
      // 对于DBLP，以及64-bit的数据，直接看作F32
      return AUDIO_F32;
    }
  }

  auto fmtNameTrans2(AVSampleFormat psprFmt) {
    switch (psprFmt) {
    case AV_SAMPLE_FMT_U8P: {
      return AV_SAMPLE_FMT_U8;
    } break;
    case AV_SAMPLE_FMT_S16P: {
      return AV_SAMPLE_FMT_S16;
    } break;
    case AV_SAMPLE_FMT_S32P: {
      return AV_SAMPLE_FMT_S32;
    } break;
    default:
      // 对于DBLP，以及64-bit的数据，直接看作F32
      return AV_SAMPLE_FMT_FLT;
    }
  }

private:
  bool isPlannar = false;
};