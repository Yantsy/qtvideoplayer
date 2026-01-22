#pragma once
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

class Demuxer {
public:
  //...

  auto demux(const char *filename) noexcept {
    // format in
    AVFormatContext *pFormatCtx = avformat_alloc_context(); // 申请内存用于存储
    // 打开文件
    if (avformat_open_input(&pFormatCtx, filename, nullptr, nullptr) != 0) {
      std::cout << "Can't open file\n" << std::endl;
      return -1;
    } else {
      std::cout << "Open file success\n"
                << "Filepath: " << filename << "\n"
                << "Estimated Duration: " << pFormatCtx->duration << "\n"
                << std::endl;
    };
    // streams out
    if (avformat_find_stream_info(pFormatCtx, nullptr) != 0) {
      std::cout << "can't find stream info\n" << std::endl;
      return -1;
    };
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
      const auto pLocalCodecParameters =
          pFormatCtx->streams[i]
              ->codecpar; // access the parameters of the stream
      const auto pLocalCodec =
          avcodec_find_decoder(pLocalCodecParameters->codec_id);
      if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
        videoStreamIndex = i;
        std::cout << "Video Resolution:" << pLocalCodecParameters->width << "x"
                  << pLocalCodecParameters->height << "\n"
                  << std::endl;

      } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
        audioStreamIndex = i;
        std::cout << "Audio sample Rate:" << pLocalCodecParameters->sample_rate
                  << "\n"
                  << std::endl;
      }
      std::cout << "Biterate:" << pLocalCodecParameters->bit_rate << std::endl;
    }

    // decode videostream
    const auto videoCdcPar = pFormatCtx->streams[videoStreamIndex]->codecpar;
    const auto videoCdc = avcodec_find_decoder(videoCdcPar->codec_id);
    if (videoCdc == nullptr) {
      std::cout << "Unsupported videostream decoder\n" << std::endl;
      return -1;
    } else {
      std::cout << "Supported videostream decoder:" << videoCdc->name << "\n"
                << std::endl;
      return 0;
    }
    auto *videoCdcCtx = avcodec_alloc_context3(videoCdc);
    if (avcodec_parameters_to_context(videoCdcCtx, videoCdcPar) != 0) {
      std::cout << "Can't copy codec parameters to codeccontext\n" << std::endl;
      return -1;
    };
    if (avcodec_open2(videoCdcCtx, videoCdc, nullptr) != 0) {
      std::cout << "Can't open videostream decoder\n" << std::endl;
      return -1;
    };

    // store demuxed video frame
    auto *videoFrameYUVOrig = av_frame_alloc();
    if (videoFrameYUVOrig == nullptr) {
      std::cout << "Can't read original video frame\n" << std::endl;
      return -1;
    }
    auto *videoFrameYUVCopy = av_frame_alloc();
    if (videoFrameYUVCopy == nullptr) {
      std::cout << "Can't copy the  original video frame\n" << std::endl;
      return -1;
    }
    // allocate buffer

    videoFrameYUVOrig->format = videoCdcCtx->pix_fmt;
    videoFrameYUVOrig->width = videoCdcCtx->width;
    videoFrameYUVOrig->height = videoCdcCtx->height;
    auto a = videoFrameYUVOrig->data[0];
    av_frame_get_buffer(videoFrameYUVOrig, 32);

    auto videoPacket = av_packet_alloc();

    while (av_read_frame(pFormatCtx, videoPacket) >= 0) {
      if (videoPacket->stream_index == videoStreamIndex) {
        avcodec_send_packet(videoCdcCtx, videoPacket);
        av_packet_unref(videoPacket);

        while (avcodec_receive_frame(videoCdcCtx, videoFrameYUVOrig) == 0) {
          av_frame_ref(videoFrameYUVCopy, videoFrameYUVOrig);
          av_frame_unref(videoFrameYUVOrig);

          // do something
        }
      }
    }

    avcodec_free_context(&videoCdcCtx);
    // av_frame_free(&videoFrameYUVOrig);
    av_frame_free(&videoFrameYUVCopy);
    av_packet_free(&videoPacket);
    avformat_close_input(&pFormatCtx);

    return 0;
  };

private:
  int videoStreamIndex = -1;
  int audioStreamIndex = -1;
  int subtitleStreamIndex = -1;
};