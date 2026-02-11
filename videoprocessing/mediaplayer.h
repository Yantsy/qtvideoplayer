#pragma once

#include <chrono>
#include <memory>

#include <mutex>
#include <queue>
#include <thread>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixdesc.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include "audiowidget.h"
#include "glwidget.h"
#include "resampler.h"
// resource management
template <typename T> struct CtxFree;
template <> struct CtxFree<AVFormatContext> {
  auto operator()(AVFormatContext *ctx) const noexcept {
    avformat_close_input(&ctx);
  };
};
template <> struct CtxFree<AVCodecContext> {
  auto operator()(AVCodecContext *ctx) const noexcept {
    avcodec_free_context(&ctx);
  }
};
template <> struct CtxFree<SwrContext> {
  auto operator()(SwrContext *ctx) const noexcept { swr_free(&ctx); }
};
template <> struct CtxFree<AVFrame> {
  auto operator()(AVFrame *ctx) const noexcept { av_frame_free(&ctx); }
};
template <> struct CtxFree<AVPacket> {
  auto operator()(AVPacket *ctx) const noexcept { av_packet_free(&ctx); }
};
class MediaPlayer {
  int a = 0;

public:
  MediaPlayer() {
    MyResampler myResampler;
    MyGLWidget videowidget;
    MyAudioWidget audioWidget;
    audioQueue.format = myResampler.fmtNameTrans(mediaInfo.sampleFmt);
  };
  ~MediaPlayer() {};

  auto openFile(const char *pfilePath) {
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, pfilePath, nullptr, nullptr) != 0) {
      std::cerr << "Can't open file\n" << std::flush;
    }
    if (avformat_find_stream_info(pFormatCtx, nullptr) != 0) {
      std::cout << "Can't find video stream info\n";
    }
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
      const auto cdcPar = pFormatCtx->streams[i]->codecpar;
      const auto *decoder = avcodec_find_decoder(cdcPar->codec_id);
      if (decoder == nullptr) {
        std::cerr << "Can't find supported decoder\n" << std::flush;
      }
      auto containerDuration = (float)pFormatCtx->duration;
      auto base = pFormatCtx->streams[i]->time_base.den;
      auto duration = (float)pFormatCtx->streams[i]->duration / base;
      if (duration <= 0.0f) {
        duration = containerDuration / 1000 / base;
      };
      if (cdcPar->codec_type == AVMEDIA_TYPE_AUDIO) {
        ptrSet.audioDecCtx.reset(avcodec_alloc_context3(decoder));
        mediaInfo.asIndex = i;
        mediaInfo.adecoderName = const_cast<char *>(decoder->name);
        mediaInfo.atimeBase = av_q2d(pFormatCtx->streams[i]->time_base);
        mediaInfo.aduration = duration;
        mediaInfo.sampleFmt = ptrSet.audioDecCtx->sample_fmt;
        mediaInfo.splFmtName =
            const_cast<char *>(av_get_sample_fmt_name(mediaInfo.sampleFmt));
        mediaInfo.splRate = (float)cdcPar->sample_rate / 1000;
        mediaInfo.channels = cdcPar->ch_layout.nb_channels;
        av_channel_layout_describe(&cdcPar->ch_layout, mediaInfo.channelLayout,
                                   sizeof(mediaInfo.channelLayout));

      } else if (cdcPar->codec_type == AVMEDIA_TYPE_VIDEO) {
        ptrSet.videoDecCtx.reset(avcodec_alloc_context3(decoder));
        mediaInfo.vsIndex = i;
        mediaInfo.vdecoderName = const_cast<char *>(decoder->name);
        mediaInfo.vtimeBase = av_q2d(pFormatCtx->streams[i]->time_base);
        mediaInfo.resolution[0] = cdcPar->width;
        mediaInfo.resolution[1] = cdcPar->height;
        mediaInfo.vduration = duration;
        mediaInfo.pxFmt = ptrSet.videoDecCtx->pix_fmt;
        mediaInfo.pxFmtName =
            const_cast<char *>(av_get_pix_fmt_name(mediaInfo.pxFmt));
      }
      const AVPixFmtDescriptor *pDesc = av_pix_fmt_desc_get(mediaInfo.pxFmt);
      mediaInfo.pxFmtDpth = pDesc->comp[1].depth;
      if (cdcPar->codec_type == AVMEDIA_TYPE_VIDEO) {
        std::cout << "Stream[" << i << "]:Video\n"
                  << "__resolution:" << cdcPar->width << "x" << cdcPar->height
                  << "\n"
                  << "__duration:" << duration << "s\n";
      }
    }
    mediaInfo.filePath = const_cast<char *>(pfilePath);
    ptrSet.formatCtx.reset(pFormatCtx);
    ptrSet.vdecFrm.reset(av_frame_alloc());
    ptrSet.adecFrm.reset(av_frame_alloc());
    ptrSet.vmyFrm.reset(av_frame_alloc());
    ptrSet.amyFrm.reset(av_frame_alloc());
    ptrSet.paket.reset(av_packet_alloc());
  };

private:
  // media
  struct MediaInfo {
    char *filePath;
    int vsIndex;
    char *vdecoderName;
    int resolution[2];
    double vtimeBase;
    float vduration;
    AVPixelFormat pxFmt;
    char *pxFmtName;
    int pxFmtDpth;
    int asIndex;
    char *adecoderName;
    double atimeBase;
    int samples;
    int silence;
    int splRate;
    float aduration;
    AVSampleFormat sampleFmt;
    char *splFmtName;
    int channels;
    char channelLayout[64];
    int ssIndex;
  } mediaInfo;

  struct PtrSet {
    std::unique_ptr<AVFormatContext, CtxFree<AVFormatContext>> formatCtx =
        nullptr;
    std::unique_ptr<AVCodecContext, CtxFree<AVCodecContext>> videoDecCtx =
        nullptr;
    std::unique_ptr<AVCodecContext, CtxFree<AVCodecContext>> audioDecCtx =
        nullptr;
    std::unique_ptr<SwrContext, CtxFree<SwrContext>> videoRsplCtx = nullptr;
    std::unique_ptr<SwrContext, CtxFree<SwrContext>> audioRsplCtx = nullptr;
    std::unique_ptr<AVPacket, CtxFree<AVPacket>> paket = nullptr;
    std::unique_ptr<AVFrame, CtxFree<AVFrame>> vdecFrm = nullptr;
    std::unique_ptr<AVFrame, CtxFree<AVFrame>> vmyFrm = nullptr;
    std::unique_ptr<AVFrame, CtxFree<AVFrame>> adecFrm = nullptr;
    std::unique_ptr<AVFrame, CtxFree<AVFrame>> amyFrm = nullptr;
  } ptrSet;

  const auto getInfo(AVFormatContext *pFormatCtx,
                     AVCodecContext *pCodecCtx) noexcept {};
  struct AudioQueue {
    SDL_AudioFormat format;
    std::queue<std::vector<uint8_t>> packets;
    std::mutex mutex;
    size_t currentIndex = 0;
    std::vector<uint8_t> currentBuffer;
  } audioQueue;
  void callBack(void *puserData, uint8_t *pstream, int plen) {
    auto *audioState = static_cast<AudioQueue *>(puserData);
    SDL_memset(pstream, 0, plen);

    int remaining = plen;
    uint8_t *out = pstream;

    std::lock_guard<std::mutex> lock(audioState->mutex);
    while (remaining > 0) {
      if (audioState->currentIndex >= audioState->currentBuffer.size()) {
        if (audioState->packets.empty()) {
          return;
        }
        audioState->currentBuffer = std::move(audioState->packets.front());
        audioState->packets.pop();
        audioState->currentIndex = 0;
      }
      size_t available =
          audioState->currentBuffer.size() - audioState->currentIndex;
      size_t tocopy = std::min(available, (size_t)remaining);

      SDL_MixAudioFormat(
          out, audioState->currentBuffer.data() + audioState->currentIndex,
          audioState->format, tocopy, SDL_MIX_MAXVOLUME);
      audioState->currentIndex += tocopy;
      out += tocopy;
      remaining -= tocopy;
    }
  };
};