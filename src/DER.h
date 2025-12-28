#pragma once

#include <QMatrix4x4>
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

class VideoGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  explicit VideoGLWidget(QWidget *parent = nullptr);
  ~VideoGLWidget();

  void updateTexture(unsigned char *data, int width, int height);
  void clearTexture();

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  void setupShaders();
  void setupGeometry();

  QOpenGLShaderProgram *shaderProgram = nullptr;

  GLuint textureID = 0;

  GLuint VAO, VBO, EBO;
};