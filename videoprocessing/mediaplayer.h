#pragma once

#include <QApplication>
#include <QFileDialog>
#include <QWidget>
#include <chrono>
#include <memory>

#include <mutex>
#include <queue>
#include <thread>
extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include "audiowidget.h"
#include "decoder.h"
#include "demuxer.h"
#include "glwidget.h"
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
class MediaPlayer {
public:
  MediaPlayer() {
    MyDemuxer myDemuxer;
    MyDecoder myDecoder;
    MyResampler myResampler;
    MyGLWidget videowidget;
    MyAudioWidget audioWidget;
  };
  ~MediaPlayer() {};

  auto openFile(const char *filePath) {
    std::cout << "Open File:" << *filePath << std::endl;
  };

private:
  // media
  struct MediaInfo {
    char *filePath;
  };

  struct CtxPtr {
    std::unique_ptr<AVFormatContext, CtxFree<AVFormatContext>> pFormatCtx;
    std::unique_ptr<AVCodecContext, CtxFree<AVCodecContext>> pVideoDecCtx;
    std::unique_ptr<AVCodecContext, CtxFree<AVCodecContext>> pAudioDecCtx;
    std::unique_ptr<SwrContext, CtxFree<SwrContext>> pVideoRsplCtx;
    std::unique_ptr<SwrContext, CtxFree<SwrContext>> pAudioRsplCtx;
  };
  auto outputInfo() {};

  // video
  // audio
  struct AudioQueue {
    SDL_AudioFormat format;
    std::queue<std::vector<uint8_t>> packets;
    std::mutex mutex;
    size_t currentIndex = 0;
    std::vector<uint8_t> currentBuffer;
  };
  void callBack(void *puserData, uint8_t *pstream, int plen) {};
};