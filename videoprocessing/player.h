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

private:
  QOpenGLVertexArrayObject *m_vao0 = nullptr, *m_vao1 = nullptr;
  // QOpenGLBuffer *m_vbo0 = nullptr, *m_vbo1 = nullptr;
  QOpenGLShaderProgram *m_shaderProgram0 = nullptr, *m_shaderProgram1 = nullptr;
  // QOpenGLShader *m_vertexShader = nullptr, *m_fragmentShader = nullptr;
  QOpenGLTexture *m_texture0 = nullptr, *m_texture1 = nullptr;
  const float screenAspectRatio = 16.0f / 9.0f;
  const float screenVerseRatio = 9.0f / 16.0f;

  struct MyImage {
    constexpr static auto location = ":/resources/waterlilies.jpg";
  };
};