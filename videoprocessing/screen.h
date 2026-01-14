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
class screen : public QOpenGLWidget, protected QOpenGLExtraFunctions {
  Q_OBJECT
public:
  screen(QWidget *parent = nullptr);
  ~screen();

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  QMatrix4x4 projection;
  QOpenGLShader *vertexShader = nullptr, *fragmentShader = nullptr,
                *fragmentShader1 = nullptr;
  QOpenGLBuffer *VBO = nullptr, *EBO = nullptr, *VBO1 = nullptr,
                *VBO2 = nullptr, *VBO3 = nullptr;
  QOpenGLVertexArrayObject *VAO = nullptr, *VAO1 = nullptr, *VAO2 = nullptr,
                           *VAO3 = nullptr;
  QOpenGLShaderProgram *shaderProgram = nullptr, *shaderProgram1 = nullptr,
                       *shaderProgram2 = nullptr;
  QOpenGLTexture *texture = nullptr;

  QElapsedTimer timer;
};