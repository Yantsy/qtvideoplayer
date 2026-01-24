#include "decoder.h"
#include "demuxer.h"
#include "glwidget.h"
#include <QApplication>
#include <QFileDialog>
#include <QWidget>
#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  MyGLWidget ffmpegvideowidget;
  ffmpegvideowidget.show();

  // Demuxer demuxer;
  MyDemuxer myDemuxer;
  MyDecoder myDecoder;
  const auto FormatCtx = myDemuxer.alloc();

  auto filePath = QFileDialog::getOpenFileName();
  myDemuxer.open(filePath.toStdString().c_str(), FormatCtx);

  auto vsIndex = myDemuxer.findVSInfo(FormatCtx);
  const auto decoder = myDecoder.findDec(FormatCtx, vsIndex);
  const auto deCtx = myDecoder.alcCtx(decoder, FormatCtx);
  const auto decFrame = myDecoder.alcFrm();
  const auto myFrame = myDecoder.alcFrm();
  const auto pkt = myDecoder.alcPkt();
  double timeBase = av_q2d(FormatCtx->streams[vsIndex]->time_base);
  auto startTime = std::chrono::steady_clock::now();
  int64_t firstPts = AV_NOPTS_VALUE;
  uint8_t yPlane;
  uint8_t uPlane;
  uint8_t vPlane;
  int frmWidth = 0;
  int frmHeight = 0;
  int lineSizeY = 0;
  int lineSizeUV = 0;
  int amtPkt = 0;
  int amtFrm = 0;

  while (myDecoder.axptPkt(FormatCtx, pkt)) {
    if (pkt->stream_index == vsIndex) {
      myDecoder.decPkt(deCtx, pkt);
      amtPkt++;
      while (myDecoder.rcvFrm(deCtx, decFrame)) {
        myDecoder.cpyFrm(decFrame, myFrame);
        amtFrm++;
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
        auto frmWidth = myFrame->width;
        auto frmHeight = myFrame->height;
        auto lineSize0 = myFrame->linesize[0];
        auto lineSize1 = myFrame->linesize[1];

        ffmpegvideowidget.renderWithOpenGL(yPlane, uPlane, vPlane, frmWidth,
                                           frmHeight, lineSize0, lineSize1);
        QCoreApplication::processEvents();
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - startTime).count();

        double waitTime = frameTimeSeconds - elapsed;
        if (waitTime > 0) {
          std::this_thread::sleep_for(std::chrono::duration<double>(waitTime));
        }
        av_frame_unref(myFrame);
      }
      av_frame_unref(decFrame);
    }
    av_packet_unref(pkt);
  }
  std::cout << "Total Packets: " << amtPkt << "\n";
  std::cout << "Total Frames: " << amtFrm << "\n";

  myDecoder.free(pkt);
  myDecoder.free(decFrame);
  myDecoder.free(myFrame);
  myDemuxer.close(FormatCtx);
  myDecoder.free(deCtx);

  printf("9\n");
  // demuxer.demux("/home/yantsy/Documents/videoplayer/resources/c.mp4");
  //  decoder.decode(demuxer.pLocalCodecParameters);

  return app.exec();
}