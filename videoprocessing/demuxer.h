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
  auto alloc() noexcept {
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

  auto findVSInfo(AVFormatContext *pFormatCtx) noexcept {
    if (avformat_find_stream_info(pFormatCtx, nullptr) != 0) {
      std::cout << "Can't find stream info\n" << std::endl;
      return -1;
    }

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

  auto close(AVFormatContext *pFormatCtx) noexcept {
    avformat_close_input(&pFormatCtx);
    return 0;
  }

  auto close(AVCodecContext *pCodecContext) noexcept {
    avcodec_free_context(&pCodecContext);
    return 0;
  }
};