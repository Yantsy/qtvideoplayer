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
  auto findPxFmt(const AVCodecContext *pcdCtx) noexcept {
    const auto *ppixFmt = av_get_pix_fmt_name(pcdCtx->pix_fmt);
    if (ppixFmt == nullptr) {
      std::cout << "Can't find supported pixel format\n" << std::endl;
      // exit(-1);
    }
    std::cout << "Pixel format:" << ppixFmt << "\n" << std::endl;
    return pcdCtx->pix_fmt;
  }
  auto findSprFmt(const AVCodecContext *pcdCtx) noexcept {
    const auto *psprFmt = av_get_sample_fmt_name(pcdCtx->sample_fmt);
    if (psprFmt == nullptr) {
      std::cout << "Can't find supported sample format\n" << std::endl;
      // exit(-1);
    }
    std::cout << "Sample format:" << psprFmt << "\n" << std::endl;
    return pcdCtx->sample_fmt;
  }
  auto findDec(AVFormatContext *pFormatCtx, const int pstreamIndex,
               uint8_t tgt) noexcept {
    const auto cdcPar = pFormatCtx->streams[pstreamIndex]->codecpar;
    const auto *pdecoder = avcodec_find_decoder(cdcPar->codec_id);
    if (pdecoder == nullptr) {
      std::cout << "Can't find supported decoder\n" << std::endl;
      // exit(-1);
    } else {
      switch (tgt) {
      case target::VIDEO: {
        std::cout << "Supported videostream decoder:" << pdecoder->name << "\n";
        break;
      }
      case target::AUDIO: {
        std::cout << "Supported audiostream decoder:" << pdecoder->name << "\n"
                  << std::endl;
        break;
      }
      }
    }
    return pdecoder;
  }

  auto alcCtx(const AVCodec *pdecoder, const AVFormatContext *pFormatCtx,
              const int pstreamIndex) noexcept {
    AVCodecContext *cdCtx = avcodec_alloc_context3(pdecoder);
    const auto cdcPar = pFormatCtx->streams[pstreamIndex]->codecpar;
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
      std::cout << "Open codec successfully\n";
    }

    return cdCtx;
  }
  auto findPxDpth(AVPixelFormat ppixFmt) noexcept {
    const AVPixFmtDescriptor *pDesc = av_pix_fmt_desc_get(ppixFmt);
    if (pDesc) {
      return pDesc->comp[0].depth;
    }
    return 8;
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