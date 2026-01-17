#pragma once
#include <QElapsedTimer>
#include <QImage>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QPixmap>
#include <QTimer>

class MyPlayer : public QOpenGLWidget, protected QOpenGLExtraFunctions {
  Q_OBJECT
public:
  explicit MyPlayer(QWidget *parent = nullptr) noexcept;
  ~MyPlayer();

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  // void resizeEvent(QResizeEvent *event) override;

private:
  QOpenGLVertexArrayObject *m_vao0 = nullptr, *m_vao1 = nullptr;
  // QOpenGLBuffer *m_vbo0 = nullptr, *m_vbo1 = nullptr;
  QOpenGLShaderProgram *m_shaderProgram0 = nullptr, *m_shaderProgram1 = nullptr;
  // QOpenGLShader *m_vertexShader = nullptr, *m_fragmentShader = nullptr;
  QOpenGLTexture *m_texture0 = nullptr, *m_texture1 = nullptr;
  QMatrix4x4 transform;
  const float screenAspectRatio = 16.0f / 9.0f;
  const float screenVerseRatio = 9.0f / 16.0f;

  float imageWidth = 0.0f;
  float imageHeight = 0.0f;
  float windowWidth = 0.0f;
  float windowHeight = 0.0f;

  QMatrix4x4 imageScaleMatrix(float imgWidth, float imgHeight);
  QMatrix4x4 windowScaleMatrix(float winWidth, float winHeight);
  QMatrix4x4 transformMatrix(float ww, float wh, float iw, float ih);
};