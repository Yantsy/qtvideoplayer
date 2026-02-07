#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}
#include "resampler.h"
#include "target.h"
class MyDecoder {
public:
  auto findPxFmt(const AVCodecContext *pcdCtx) noexcept {
    const auto *ppixFmt = av_get_pix_fmt_name(pcdCtx->pix_fmt);
    if (ppixFmt == nullptr) {
      std::cerr << "Can't find supported pixel format\n";
      // exit(-1);
    }
    std::cout << "__pixel format:" << ppixFmt << "\n";
    return pcdCtx->pix_fmt;
  }
  auto findSprFmt(const AVCodecContext *pcdCtx) noexcept {
    const auto *psprFmt = av_get_sample_fmt_name(pcdCtx->sample_fmt);
    auto a = pcdCtx->sample_fmt;
    if (psprFmt == nullptr) {
      std::cerr << "Can't find supported sample format\n";
      // exit(-1);
    }
    std::cout << "__sample format:" << psprFmt << "\n";
    return pcdCtx->sample_fmt;
  }
  auto findASInfo(const AVFormatContext *pFormatCtx,
                  const AVCodecContext *pcdCtx,
                  const int pastreamIndex) noexcept {
    SDL_AudioSpec pSpec;
    MyResampler pResampler;
    const auto cdcPar = pFormatCtx->streams[pastreamIndex]->codecpar;
    char layout_name[64];
    const auto *psprFmt = av_get_sample_fmt_name(pcdCtx->sample_fmt);
    auto a = pcdCtx->sample_fmt;
    if (psprFmt == nullptr) {
      std::cerr << "Can't find supported sample format\n";
      // exit(-1);
    }
    av_channel_layout_describe(&cdcPar->ch_layout, layout_name,
                               sizeof(layout_name));
    auto containerDuration = (float)pFormatCtx->duration;
    auto base = pFormatCtx->streams[pastreamIndex]->time_base.den;
    auto duration = (float)pFormatCtx->streams[pastreamIndex]->duration / base;
    if (duration <= 0) {
      duration = containerDuration / 1000 / base;
    };

    std::cout << "__sample rate:" << float(cdcPar->sample_rate / 1000.0f)
              << "KHz\n"
              << "__channel layout: " << layout_name << "\n"
              << "__channels:" << cdcPar->ch_layout.nb_channels << "\n"
              << "__duration:" << duration << "s\n"
              << "__sample format:" << psprFmt << "\n";
    pSpec.freq = cdcPar->sample_rate;
    pSpec.format = pResampler.fmtNameTrans(pcdCtx->sample_fmt);
    pSpec.channels = cdcPar->ch_layout.nb_channels;
    pSpec.samples = 2048;
    pSpec.silence = 0;
    return pSpec;
  }
  auto findDec(AVFormatContext *pFormatCtx, const int pstreamIndex,
               const uint8_t tgt) noexcept {
    const auto cdcPar = pFormatCtx->streams[pstreamIndex]->codecpar;
    const auto *pdecoder = avcodec_find_decoder(cdcPar->codec_id);
    if (pdecoder == nullptr) {
      std::cerr << "Can't find supported decoder\n" << std::endl;
      // exit(-1);
    } else {
      switch (tgt) {
      case target::VIDEO: {
        std::cout << "__decoder:" << pdecoder->name << "\n";
        break;
      }
      case target::AUDIO: {
        std::cout << "__decoder:" << pdecoder->name << "\n";
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
      std::cerr << "Can't copy codec parameters\n";
      // exit(-1);
    }
    if (avcodec_open2(cdCtx, pdecoder, nullptr) != 0) {
      std::cerr << "Can't open codec\n";
      // exit(-1);
    } else {
      // std::cout << "Open codec successfully\n";
    }

    return cdCtx;
  }
  auto findPxDpth(AVPixelFormat ppixFmt, int out) noexcept {
    // AVPixFmtDescriptor:Descriptor that unambiguously describes how the bits
    // of a pixel are stored in the up to 4 data planes of an image.
    const AVPixFmtDescriptor *pDesc = av_pix_fmt_desc_get(ppixFmt);
    if (pDesc) {
      if (out == 1) {
        std::cout << "__pixel depth:" << pDesc->comp[1].depth << "\n";
        // std::cout << "Pixel format:" << pDesc->name << "\n" << std::endl;
        // std::cout << "Pixel flags:" << pDesc->flags << "\n" << std::endl;
      }
      // std::cout << "Pixel log2_chroma_w and log2_chroma_h:"
      //<< pDesc->log2_chroma_w << ":" << pDesc->log2_chroma_h << "\n"
      //<< std::endl;
      // comp:Parameters that describe how pixels are packed by its FourCC
      // components
      return pDesc->comp[1].depth;
    }
    return 8;
  }
  auto alcFrm() noexcept {
    AVFrame *frame = av_frame_alloc();
    if (frame == nullptr) {
      std::cerr << "Can't allocate frame\n";
    } else {
      std::cout << "Allocate frame successfully\n";
    }
    return frame;
  }
  // needed when you want to encode a frame while decoding
  auto alcFrmBuf(AVCodecContext *pcdCtx) noexcept {
    AVFrame *pFrame = av_frame_alloc();
    if (pFrame == nullptr) {
      std::cerr << "Can't allocate frame\n";
    } else {
      std::cout << "Allocate frame successfully\n";
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
      std::cerr << "Can't allocate packet\n";
    } else {
      std::cout << "Allocate packet successfully\n";
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