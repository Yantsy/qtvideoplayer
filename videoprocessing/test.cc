#include <QApplication>
#include <QFileDialog>
#include <QWidget>
#include <chrono>
#include <thread>
extern "C" {
#include <libavutil/pixdesc.h>
}

// #include "audiowidget.h"
#include "decoder.h"
#include "demuxer.h"
#include "glwidget.h"
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  // app.setQuitOnLastWindowClosed(true);
  MyGLWidget ffmpegvideowidget;
  ffmpegvideowidget.show();

  // Demuxer demuxer;
  MyDemuxer myDemuxer;
  MyDecoder myDecoder;
  const auto FormatCtx = myDemuxer.alcFmtCtx();

  auto filePath = QFileDialog::getOpenFileName();
  if (filePath.isEmpty()) {
    ffmpegvideowidget.close();
    return 0;
  }
  myDemuxer.open(filePath.toStdString().c_str(), FormatCtx);
  myDemuxer.findSInfo(FormatCtx);

  auto vsIndex = myDemuxer.findVSInfo(FormatCtx);
  auto asIndex = myDemuxer.findASInfo(FormatCtx);
  const auto decoder = myDecoder.findDec(FormatCtx, vsIndex, target::VIDEO);
  const auto adecoder = myDecoder.findDec(FormatCtx, asIndex, target::AUDIO);
  const auto deCtx = myDecoder.alcCtx(decoder, FormatCtx, vsIndex);
  const auto dePxFmt = myDecoder.findPxFmt(deCtx);
  const auto dePxDpth = myDecoder.findPxDpth(dePxFmt, 1);
  const auto adeCtx = myDecoder.alcCtx(adecoder, FormatCtx, asIndex);
  const auto adeSpc = myDecoder.findASInfo(FormatCtx, adeCtx, asIndex);
  const auto decFrame = myDecoder.alcFrm();
  const auto adecFrame = myDecoder.alcFrm();
  const auto myFrame = myDecoder.alcFrm();
  const auto amyFrame = myDecoder.alcFrm();
  const auto pkt = myDecoder.alcPkt();
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
  int count = 0;

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
    } else if (pkt->stream_index == asIndex) {
      myDecoder.decPkt(adeCtx, pkt);
      while (myDecoder.rcvFrm(adeCtx, adecFrame)) {
        myDecoder.cpyFrm(adecFrame, amyFrame);

        av_frame_unref(adecFrame);
      }
      av_frame_unref(amyFrame);
    }
    av_packet_unref(pkt);
  }
  // std::cout << "Pixel Format: " << pxFmt << "\n" << std::endl;
  std::cout << "Total Packets: " << amtPkt << "\n";
  std::cout << "Total Frames: " << amtFrm << "\n" << std::endl;

  myDecoder.free(pkt);
  myDecoder.free(decFrame);
  myDecoder.free(adecFrame);
  myDecoder.free(myFrame);
  myDecoder.free(amyFrame);
  myDecoder.free(deCtx);
  myDecoder.free(adeCtx);
  myDemuxer.close(FormatCtx);

  std::cout << "Decoding Done " << "\n" << std::endl;
  // demuxer.demux("/home/yantsy/Documents/videoplayer/resources/c.mp4");
  //  decoder.decode(demuxer.pLocalCodecParameters);

  return app.exec();
}