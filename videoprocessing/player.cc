#include "player.h"

MyPlayer::MyPlayer(QWidget *parent) noexcept : QOpenGLWidget(parent) {
  this->setContentsMargins(0, 0, 0, 0);
  this->setMinimumSize(600, 500);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

MyPlayer::~MyPlayer() {}

void MyPlayer::initializeGL() {
  constexpr auto m_imageSource = ":/resources/aichan.jpg";
  imageWidth = QImage(m_imageSource).width();
  imageHeight = QImage(m_imageSource).height();

  const float m_vertices[] = {1.0f, 1,     0.0f, 1.0f,  1.0f, 1.0f, -1,
                              0.0f, 1.0f,  0.0f, -1.0f, -1,   0.0f, 0.0f,
                              0.0f, -1.0f, 1,    0.0f,  0.0f, 1.0f};

  const unsigned int m_indices[] = {0, 1, 2, 0, 2, 3};

  const auto m_vertexShaderSource = R"(
  #version 330 core 
  layout (location =0) in vec3 aPos;
  layout (location =1) in vec2 aTexCoord;
  out vec2 TexCoord;
  uniform mat4 transform;
  void main(){
  gl_Position =transform*vec4(aPos,1.0);
  TexCoord =aTexCoord;})";
  const auto m_fragmentShaderSource = R"(
  #version 330 core
  out vec4 FragColor;
  in vec2 TexCoord;
  uniform sampler2D texture0;
  void main(){
  FragColor=texture(texture0,TexCoord);})";

  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  m_vao0 = new QOpenGLVertexArrayObject(this);
  m_vao0->create();
  m_vao0->bind();

  const std::unique_ptr<QOpenGLBuffer> m_vbo0 =
      std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
  m_vbo0->create();
  m_vbo0->bind();
  m_vbo0->allocate(m_vertices, sizeof(m_vertices));

  const std::unique_ptr<QOpenGLBuffer> m_ebo0 =
      std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
  m_ebo0->create();
  m_ebo0->bind();
  m_ebo0->allocate(m_indices, sizeof(m_indices));

  const auto m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
  m_vertexShader->compileSourceCode(m_vertexShaderSource);
  const auto m_fragmentShader =
      new QOpenGLShader(QOpenGLShader::Fragment, this);
  m_fragmentShader->compileSourceCode(m_fragmentShaderSource);

  m_shaderProgram0 = new QOpenGLShaderProgram(this);
  m_shaderProgram0->addShader(m_vertexShader);
  m_shaderProgram0->addShader(m_fragmentShader);
  m_shaderProgram0->link();
  m_shaderProgram0->bind();
  m_shaderProgram0->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(float));
  m_shaderProgram0->enableAttributeArray(0);
  m_shaderProgram0->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2,
                                       5 * sizeof(float));
  m_shaderProgram0->enableAttributeArray(1);

  m_texture0 = new QOpenGLTexture(QImage(m_imageSource));
  m_texture0->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  m_texture0->setMagnificationFilter(QOpenGLTexture::Linear);
  m_texture0->setWrapMode(QOpenGLTexture::ClampToEdge);

  m_vao0->release();
  m_shaderProgram0->release();
}
void MyPlayer::resizeGL(const int w, const int h) { glViewport(0, 0, w, h); }
void MyPlayer::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);
  m_shaderProgram0->bind();
  m_vao0->bind();
  m_texture0->bind(0);
  m_shaderProgram0->setUniformValue("texture0", 0);
  const float pi = std::acos(-1);
  const float a = std::cos(pi);
  const float b = std::sin(pi);
  const float windowAspectRatio =
      static_cast<float>(width()) / static_cast<float>(height());
  const float windowVerseRatio = 1.0f / windowAspectRatio;
  QMatrix4x4 rotation = {a, -b, 0, 0, b, a, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  QMatrix4x4 mirror = {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  QMatrix4x4 scale = imageScaleMatrix(imageWidth, imageHeight);
  QMatrix4x4 ortho = windowScaleMatrix(width(), height());
  // ortho.ortho(-1.0f * windowAspectRatio, 1.0f * windowAspectRatio,
  //  -1.0f * windowVerseRatio, 1.0f * windowVerseRatio, -1.0f, 1.0f);
  // QMatrix4x4 transform = ortho * scale * mirror;
  QMatrix4x4 transform =
      transformMatrix(width(), height(), imageWidth, imageHeight);
  m_shaderProgram0->setUniformValue("transform", transform);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  m_texture0->release();
  m_vao0->release();
  m_shaderProgram0->release();
}

QMatrix4x4 MyPlayer::transformMatrix(const float ww, const float wh,
                                     const float iw, const float ih) {

  float imageRatio = iw / ih;
  // float imageVerseRatio = 1.0f / imageRatio;
  float windowRatio = ww / wh;
  float windowVerseRatio = 1.0f / windowRatio;
  const float pi = std::acos(-1);
  const float a = std::cos(pi);
  const float b = std::sin(pi);
  // const float windowAspectRatio =
  // static_cast<float>(width()) / static_cast<float>(height());
  // const float windowVerseRatio = 1.0f / windowAspectRatio;
  QMatrix4x4 rotation = {a, -b, 0, 0, b, a, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  QMatrix4x4 mirror = {1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  QMatrix4x4 scale = imageScaleMatrix(imageWidth, imageHeight);
  QMatrix4x4 ortho = windowScaleMatrix(width(), height());
  QMatrix4x4 transformMatrix;
  QMatrix4x4 betweenMatrix = ortho * scale * mirror;
  QMatrix4x4 adjustMatrix;
  if (imageRatio >= 1.0f) {
    if (windowRatio >= 1.0f) {
      transformMatrix = betweenMatrix;

    } else {
      adjustMatrix = {windowRatio, 0, 0, 0, 0, windowRatio, 0, 0,
                      0,           0, 1, 0, 0, 0,           0, 1};

      transformMatrix = adjustMatrix * betweenMatrix;
    }
  } else {
    if (windowRatio >= 1.0f) {
      adjustMatrix = {windowVerseRatio,
                      0,
                      0,
                      0,
                      0,
                      windowVerseRatio,
                      0,
                      0,
                      0,
                      0,
                      1,
                      0,
                      0,
                      0,
                      0,
                      1};

      transformMatrix = adjustMatrix * betweenMatrix;
    } else {
      transformMatrix = betweenMatrix;
    }
  }
  return transformMatrix;
}

QMatrix4x4 MyPlayer::imageScaleMatrix(const float imgWidth,
                                      const float imgHeight) {
  QMatrix4x4 scaleMatrix;
  const float imgAspectRatio = imgWidth / imgHeight;
  const float imgVerseRatio = 1.0f / imgAspectRatio;
  if (imgAspectRatio >= 1.0f) {
    scaleMatrix = {1, 0, 0, 0, 0, imgVerseRatio, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  } else {
    scaleMatrix = {imgAspectRatio, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  };
  // std::cout << imgWidth << " " << imgHeight << std::endl;
  return scaleMatrix;
}

QMatrix4x4 MyPlayer::windowScaleMatrix(const float winWidth,
                                       const float winHeight) {
  QMatrix4x4 orthoMatrix;
  const float winAspectRatio = winWidth / winHeight;
  const float winVerseRatio = 1.0f / winAspectRatio;
  if (winWidth <= winHeight) {
    orthoMatrix = {winVerseRatio, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0, 0.0,
                   0.0,           0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  } else {
    orthoMatrix = {1.0, 0.0, 0.0, 0.0, 0.0,  winAspectRatio,
                   0.0, 0.0, 0.0, 0.0, -1.0, 0.0,
                   0.0, 0.0, 0.0, 1.0};
  }
  return orthoMatrix;
}
/*为什么写成一个矩阵就不行了？
QMatrix4x4 MyPlayer::transformMatrix(const float ww, const float wh,
                                     const float iw, const float ih) {
  float imageRatio = iw / ih;
  float imageVerseRatio = 1.0f / imageRatio;
  float windowRatio = ww / wh;
  float windowVerseRatio = 1.0f / windowRatio;
  QMatrix4x4 transformMatrix;
  QMatrix4x4 adjustMatrix;
  if (imageRatio >= 1.0f) {
    if (windowRatio >= 1.0f) {
      transformMatrix = {ww / 2, 0,
                         0,      ww / 2,
                         0,      -ww / 2 * imageVerseRatio,
                         0,      -ww / 2 * imageVerseRatio,
                         0,      0,
                         -1,     0,
                         0,      0,
                         0,      1};
    } else {
      adjustMatrix = {windowRatio, 0, 0, 0, 0, windowRatio, 0, 0,
                      0,           0, 1, 0, 0, 0,           0, 1};
      transformMatrix = {wh / 2, 0,
                         0,      wh / 2,
                         0,      -wh / 2 * imageVerseRatio,
                         0,      -wh / 2 * imageVerseRatio,
                         0,      0,
                         -1,     0,
                         0,      0,
                         0,      1};
      transformMatrix = adjustMatrix * transformMatrix;
    }
  } else {
    if (windowRatio >= 1.0f) {
      adjustMatrix = {windowVerseRatio,
                      0,
                      0,
                      0,
                      0,
                      windowVerseRatio,
                      0,
                      0,
                      0,
                      0,
                      1,
                      0,
                      0,
                      0,
                      0,
                      1};
      transformMatrix = {ww / 2 * imageRatio,
                         0,
                         0,
                         ww / 2 * imageRatio,
                         0,
                         -ww / 2,
                         0,
                         -ww / 2,
                         0,
                         0,
                         -1,
                         0,
                         0,
                         0,
                         0,
                         1};
      transformMatrix = adjustMatrix * transformMatrix;
    } else {
      transformMatrix = {wh / 2 * imageRatio,
                         0,
                         0,
                         wh / 2 * imageRatio,
                         0,
                         -wh / 2,
                         0,
                         -wh / 2,
                         0,
                         0,
                         -1,
                         0,
                         0,
                         0,
                         0,
                         1};
    }
  }

  return transformMatrix;
}*/
