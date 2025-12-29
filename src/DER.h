#pragma once

#include "yslider.h"
#include <GL/gl.h>
#include <QComboBox>
#include <QLabel>
#include <QMatrix4x4>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QTimer>
#include <QWidget>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

class VideoGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
  Q_OBJECT

public:
  explicit VideoGLWidget(QWidget *parent = nullptr);
  ~VideoGLWidget();

  void updateTexture(unsigned char *data, int width, int height);
  void clearTexture();
  // void setSource(const QUrl url);

  double value = 0, maximum = 0;
  int vol = 0;

signals:
  // void positionChanged(qint64 pos);
  // void durationChanged(qint64 duration);
  // void finished();

public slots:

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  void setupShaders();
  void setupGeometry();

  QOpenGLShaderProgram *shaderProgram = nullptr;
  GLuint textureID = 0;
  GLuint VAO = 0, VBO = 0, EBO = 0;

  int videoWidth = 0, videoHeight = 0;

  bool hasFrame = false;

  QMatrix4x4 projection;
};

class VideoWidget : public QWidget {
  Q_OBJECT

public:
  explicit VideoWidget(QWidget *parent = nullptr);
  ~VideoWidget();
  void openFile(QUrl url);
  QComboBox *audioTrackCombo = nullptr;
  QComboBox *subtitleCombo = nullptr;

private slots:

  void playPause();
  // void setPosition(qint64 pos);
  // void setVolume(int value);
  // void setFullscreen(bool fullscreen);
  void updateFrame();
  void onAudioTrackChanged(int index);
  void onSubtitleTrackChanged(int index);

private:
  yslider *slider = nullptr;

  void initFFmpeg();
  void cleanupFFmpeg();
  void loadStreams();
  bool decodeNextFrame();
  void renderFrame(AVFrame *frame);
  void renderSubtitle();

  VideoGLWidget *glWidget = nullptr;

  QLabel *subtitleLabel = nullptr;
  QTimer *timer = nullptr;

  AVFormatContext *formatCtx = nullptr;
  AVCodecContext *videoCodecCtx = nullptr;
  AVCodecContext *audioCodecCtx = nullptr;
  AVCodecContext *subtitleCodecCtx = nullptr;

  SwsContext *swsCtx = nullptr;

  int videoStreamIndex = -1;
  int currentAudioStreamIndex = -1;
  int currentSubtitleStreamIndex = -1;

  bool isPlaying = false;

  AVPacket *packet = nullptr;
  AVFrame *frame = nullptr;
  AVFrame *rgbFrame = nullptr;

  uint8_t *buffer = nullptr;
};