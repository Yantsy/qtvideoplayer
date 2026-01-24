#pragma once
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}
#include "target.h"
class MyDecoder {
public:
  auto findDec(AVFormatContext *pFormatCtx, const int pstreamIndex,
               uint8_t tgt) noexcept {
    const auto cdcPar = pFormatCtx->streams[pstreamIndex]->codecpar;
    const auto *decoder = avcodec_find_decoder(cdcPar->codec_id);
    if (decoder == nullptr) {
      std::cout << "Can't find supported decoder\n" << std::endl;
      // exit(-1);
    } else {
      switch (tgt) {
      case target::VIDEO: {
        std::cout << "Supported videostream decoder:" << decoder->name << "\n"
                  << std::endl;
        break;
      }
      case target::AUDIO: {
        std::cout << "Supported audiostream decoder:" << decoder->name << "\n"
                  << std::endl;
        break;
      }
      }
    }
    return decoder;
  }

  auto alcCtx(const AVCodec *pdecoder,
              const AVFormatContext *pFormatCtx) noexcept {
    AVCodecContext *cdCtx = avcodec_alloc_context3(pdecoder);
    const auto cdcPar = pFormatCtx->streams[0]->codecpar;
    // ignorance of this step makes the codec fail to find the start code,
    // and causes error splitting the input into NAL units
    if (avcodec_parameters_to_context(cdCtx, cdcPar) != 0) {
      std::cout << "Can't copy codec parameters\n" << std::endl;
      // exit(-1);
    }
    if (avcodec_open2(cdCtx, pdecoder, nullptr) != 0) {
      std::cout << "Can't open codec\n" << std::endl;
      // exit(-1);
    } else {
      std::cout << "Open codec successfully\n" << std::endl;
    }

    return cdCtx;
  }

  auto alcFrm() noexcept {
    AVFrame *frame = av_frame_alloc();
    if (frame == nullptr) {
      std::cout << "Can't allocate frame\n" << std::endl;
    } else {
      std::cout << "Allocate frame successfully\n" << std::endl;
    }
    return frame;
  }
  // needed when you want to encode a frame while decoding
  auto alcFrmBuf(AVCodecContext *pcdCtx) noexcept {
    AVFrame *pFrame = av_frame_alloc();
    if (pFrame == nullptr) {
      std::cout << "Can't allocate frame\n" << std::endl;
    } else {
      std::cout << "Allocate frame successfully\n" << std::endl;
    }
    pFrame->width = pcdCtx->width;
    pFrame->height = pcdCtx->height;
    pFrame->format = pcdCtx->pix_fmt;
    av_frame_get_buffer(pFrame, 32);
    return pFrame;
  }

  auto alcPkt() noexcept {
    AVPacket *ppkt = av_packet_alloc();
    if (ppkt == nullptr) {
      std::cout << "Can't allocate packet\n" << std::endl;
    } else {
      std::cout << "Allocate packet successfully\n" << std::endl;
    }
    return ppkt;
  }

  bool axptPkt(AVFormatContext *pFormatCtx, AVPacket *ppkt) noexcept {
    return av_read_frame(pFormatCtx, ppkt) >= 0;
  }

  auto decPkt(AVCodecContext *pcdCtx, AVPacket *ppkt) noexcept {
    avcodec_send_packet(pcdCtx, ppkt);
    av_packet_unref(ppkt);
  }

  bool rcvFrm(AVCodecContext *pcdCtx, AVFrame *pdecFrame) noexcept {
    return avcodec_receive_frame(pcdCtx, pdecFrame) >= 0;
  }

  auto cpyFrm(AVFrame *pdecFrame, AVFrame *pmyFrame) {
    av_frame_ref(pmyFrame, pdecFrame);
    // av_frame_unref(decFrame);
  };
  auto free(AVCodecContext *pcdCtx) noexcept {
    avcodec_free_context(&pcdCtx);
    return 0;
  }

  auto free(AVFrame *pFrame) noexcept {
    av_frame_free(&pFrame);
    return 0;
  }

  auto free(AVPacket *ppkt) noexcept {
    av_packet_free(&ppkt);
    return 0;
  }
};