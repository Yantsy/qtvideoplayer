#pragma once

#pragma once
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

class MyDemuxer {
public:
  auto alcFmtCtx() noexcept {
  std:
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    return pFormatCtx;
  }

  auto open(const char *filename, AVFormatContext *pFormatCtx) noexcept {
    if (avformat_open_input(&pFormatCtx, filename, nullptr, nullptr) != 0) {
      std::cout << "Can't open file\n" << std::endl;
      return -1;
    } else {
      std::cout << "Open file success\n"
                << "Filepath: " << filename << "\n"
                << std::endl;
    };
    return 0;
  }

  auto findSInfo(AVFormatContext *pFormatCtx) noexcept {
    if (avformat_find_stream_info(pFormatCtx, nullptr) != 0) {
      std::cout << "Can't find video stream info\n" << std::endl;
      return -1;
    }
    return 0;
  }

  auto findVSInfo(AVFormatContext *pFormatCtx) noexcept {

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
      const auto cdcPar = pFormatCtx->streams[i]->codecpar;

      const float duration = (float)pFormatCtx->streams[i]->duration /
                             pFormatCtx->streams[i]->time_base.den;
      if (cdcPar->codec_type == AVMEDIA_TYPE_VIDEO) {
        std::cout << "Video Resolution:" << cdcPar->width << "x"
                  << cdcPar->height << "\n"
                  << "Video Duration:" << duration << "s\n"
                  << std::endl;
        return i;
      }
    }
    return 0;
  }
  auto findASInfo(AVFormatContext *pFormatCtx) noexcept {

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
      const auto cdcPar = pFormatCtx->streams[i]->codecpar;
      float duration = (float)pFormatCtx->streams[i]->duration /
                       pFormatCtx->streams[i]->time_base.den;
      if (!duration) {
        duration = 0.0f;
      };
      if (cdcPar->codec_type == AVMEDIA_TYPE_AUDIO) {

        std::cout << "Audio Sample Rate:"
                  << float(cdcPar->sample_rate / 1000.0f) << "KHz\n"
                  << "Audio Duration:" << duration << "s\n"
                  << std::endl;
        return i;
      }
    }
    return 0;
  }
  auto close(AVFormatContext *pFormatCtx) noexcept {
    avformat_close_input(&pFormatCtx);
    return 0;
  }

  auto close(AVCodecContext *pCodecContext) noexcept {
    avcodec_free_context(&pCodecContext);
    return 0;
  }
};