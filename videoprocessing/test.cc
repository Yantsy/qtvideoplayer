#include <QApplication>
#include <QFileDialog>
#include <QWidget>
#include <chrono>

#include <mutex>
#include <queue>
#include <thread>
extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/samplefmt.h>
}

#include "audiowidget.h"
#include "decoder.h"
#include "demuxer.h"
#include "glwidget.h"

struct AudioQueue {
  SDL_AudioFormat format;
  std::queue<std::vector<uint8_t>> packets;
  std::mutex mutex;
  size_t currentIndex = 0;
  std::vector<uint8_t> currentBuffer;
};
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
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  // app.setQuitOnLastWindowClosed(true);

  // define required widgets
  MyGLWidget ffmpegvideowidget;
  ffmpegvideowidget.show();

  MyAudioWidget audioWidget;

  AudioQueue audioQueue;
  // Demuxer demuxer;
  MyDemuxer myDemuxer;
  MyDecoder myDecoder;
  MyResampler myResampler;
  // open file
  const auto FormatCtx = myDemuxer.alcFmtCtx();
  auto filePath = QFileDialog::getOpenFileName();
  if (filePath.isEmpty()) {
    ffmpegvideowidget.close();
    return 0;
  }
  myDemuxer.open(filePath.toStdString().c_str(), FormatCtx);
  myDemuxer.findSInfo(FormatCtx);
  // video stream
  auto vsIndex = myDemuxer.findVSInfo(FormatCtx);
  const auto decoder = myDecoder.findDec(FormatCtx, vsIndex, target::VIDEO);
  const auto deCtx = myDecoder.alcCtx(decoder, FormatCtx, vsIndex);
  const auto dePxFmt = myDecoder.findPxFmt(deCtx);
  const auto dePxDpth = myDecoder.findPxDpth(dePxFmt, 1);
  const auto decFrame = myDecoder.alcFrm();
  const auto myFrame = myDecoder.alcFrm();
  // audio stream
  auto asIndex = myDemuxer.findASInfo(FormatCtx);
  const auto adecoder = myDecoder.findDec(FormatCtx, asIndex, target::AUDIO);
  const auto adeCtx = myDecoder.alcCtx(adecoder, FormatCtx, asIndex);
  auto adeSpc = myDecoder.findASInfo(FormatCtx, adeCtx, asIndex);
  const auto chnlLyt = FormatCtx->streams[asIndex]->codecpar->ch_layout;
  const auto channels = chnlLyt.nb_channels;
  adeSpc.callback = callBack;
  adeSpc.userdata = &audioQueue;
  audioQueue.format = adeSpc.format;
  auto obtSpc = SDL_AudioSpec();
  SDL_AudioDeviceID audioDevice =
      SDL_OpenAudioDevice(NULL, 0, &adeSpc, &obtSpc, 0);
  if (audioDevice == 0) {
    SDL_Quit();
    return -1;
  }
  const auto reSprFmt = myResampler.fmtNameTrans2(adeCtx->sample_fmt);
  SDL_PauseAudioDevice(audioDevice, 0);
  auto adecSwrCtx =
      myResampler.alcSwrCtx(chnlLyt, adeSpc.freq, adeCtx->sample_fmt, reSprFmt);
  const auto adecFrame = myDecoder.alcFrm();
  const auto amyFrame = myDecoder.alcFrm();
  const auto pkt = myDecoder.alcPkt();
  // initialize parameters required for decoding
  double timeBase = av_q2d(FormatCtx->streams[vsIndex]->time_base);
  double atimeBase = av_q2d(FormatCtx->streams[asIndex]->time_base);
  auto startTime = std::chrono::steady_clock::now();
  int64_t firstPts = AV_NOPTS_VALUE;
  int64_t afirstPts = AV_NOPTS_VALUE;
  int pxFmt = 0;
  // int olineSizeY = 0;
  int lineSizeU = 0;
  int lineSizeV = 0;
  uint8_t yPlane;
  uint8_t uPlane;
  uint8_t vPlane;
  int frmWidth = 0;
  int frmHeight = 0;
  int lineSizeY = 0;
  int lineSizeUV = 0;
  int amtPkt = 0;
  int amtFrm = 0;
  int count(0);

  while (myDecoder.axptPkt(FormatCtx, pkt)) {

    if (pkt->stream_index == vsIndex) {
      myDecoder.decPkt(deCtx, pkt);
      amtPkt++;
      while (myDecoder.rcvFrm(deCtx, decFrame)) {
        myDecoder.cpyFrm(decFrame, myFrame);
        amtFrm++;
        count++;
        if (myFrame == nullptr) {
          printf("11\n");
          break;
        }
        int64_t pts = myFrame->pts;
        if (pts == AV_NOPTS_VALUE) {
          pts = myFrame->best_effort_timestamp;
        }
        if (firstPts == AV_NOPTS_VALUE) {
          firstPts = pts;
        }
        double frameTimeSeconds = (pts - firstPts) * timeBase;
        auto yPlane = myFrame->data[0];
        auto uPlane = myFrame->data[1];
        auto vPlane = myFrame->data[2];
        // myFrame->format = deCtx->pix_fmt;

        auto frmWidth = myFrame->width;
        auto frmHeight = myFrame->height;
        auto lineSize0 = myFrame->linesize[0];
        auto lineSize1 = myFrame->linesize[1];
        if (dePxDpth == 8) {
          ffmpegvideowidget.renderWithOpenGL8(yPlane, uPlane, vPlane, frmWidth,
                                              frmHeight, lineSize0, lineSize1,
                                              dePxFmt);
        } else if (dePxDpth == 10) {
          ffmpegvideowidget.renderWithOpenGL10(yPlane, uPlane, vPlane, frmWidth,
                                               frmHeight, lineSize0, lineSize1,
                                               dePxFmt);
        }

        // QCoreApplication::processEvents();
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime).count();

        double waitTime = frameTimeSeconds - elapsed;
        if (waitTime > 0) {
          std::this_thread::sleep_for(std::chrono::duration<double>(waitTime));
        }
        av_frame_unref(myFrame);
      }
      av_frame_unref(decFrame);
    } else if (pkt->stream_index == asIndex) {
      myDecoder.decPkt(adeCtx, pkt);
      while (myDecoder.rcvFrm(adeCtx, adecFrame)) {
        myDecoder.cpyFrm(adecFrame, amyFrame);
        int outSamples =
            adecFrame         // 一次解码出的音频片段
                ->nb_samples; // 一个声道一次填入缓冲区的样本数，用来控制音频采样的延迟
        int outBufferSize = audioWidget.buffer(channels, outSamples,
                                               reSprFmt); // 缓冲区大小
        std::vector<uint8_t> buffer(outBufferSize);
        const auto data = adecFrame->data;
        // 判断是否是plannar数据，如果data[1]为空，肯定不是，否则需要转packed数据
        if (data[1] == nullptr) {
          buffer.assign(data[0], data[0] + outBufferSize);
        } else {
          uint8_t *bufPtr = buffer.data();
          swr_convert(adecSwrCtx, &bufPtr, outSamples,
                      (const uint8_t **)adecFrame->data, adecFrame->nb_samples);
        }
        {
          std::lock_guard<std::mutex> lock(audioQueue.mutex);
          audioQueue.packets.push(std::move(buffer));
        }

        av_frame_unref(adecFrame);
      }
      av_frame_unref(amyFrame);
    }
  }

  // std::cout << "Pixel Format: " << pxFmt << "\n" << std::endl;
  std::cout << "Total Packets: " << amtPkt << "\n";
  std::cout << "Total Frames: " << amtFrm << "\n";

  myDecoder.free(pkt);
  myDecoder.free(decFrame);
  myDecoder.free(adecFrame);
  myDecoder.free(myFrame);
  myDecoder.free(amyFrame);
  myDecoder.free(deCtx);
  myDecoder.free(adeCtx);
  myResampler.free(adecSwrCtx);
  myDemuxer.close(FormatCtx);
  // demuxer.demux("/home/yantsy/Documents/videoplayer/resources/c.mp4");
  //  decoder.decode(demuxer.pLocalCodecParameters);

  return app.exec();
}