#include "player.h"

MyPlayer::MyPlayer(QWidget *parent) noexcept : QOpenGLWidget(parent) {
  this->setContentsMargins(0, 0, 0, 0);
  this->resize(600, 600);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

MyPlayer::~MyPlayer() {}

void MyPlayer::initializeGL() {
  /*constexpr auto m_imageSource = ":/resources/waterlilies.jpg";
  const float imageWidth = QImage(m_imageSource).width();
  const float imageHeight = QImage(m_imageSource).height();*/
  const float m_vertices[] = {1.0f,  screenVerseRatio,  0.0f, 1.0f, 1.0f,
                              1.0f,  -screenVerseRatio, 0.0f, 1.0f, 0.0f,
                              -1.0f, -screenVerseRatio, 0.0f, 0.0f, 0.0f,
                              -1.0f, screenVerseRatio,  0.0f, 0.0f, 1.0f};

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

  m_texture0 = new QOpenGLTexture(QImage(MyImage::location));

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
  const float imageWidth = QImage(MyImage::location).width();
  const float imageHeight = QImage(MyImage::location).height();
  const float pi = std::acos(-1);
  const float a = std::cos(pi);
  const float b = std::sin(pi);
  const float windowAspectRatio =
      static_cast<float>(width()) / static_cast<float>(height());
  const float windowVerseRatio = 1.0f / windowAspectRatio;
  QMatrix4x4 rotation = {a, -b, 0, 0, b, a, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
  QMatrix4x4 ortho;
  ortho.ortho(-1.0f * windowAspectRatio, 1.0f * windowAspectRatio,
              -1.0f * windowVerseRatio, 1.0f * windowVerseRatio, -1.0f, 1.0f);
  QMatrix4x4 transform = ortho * rotation;
  m_shaderProgram0->setUniformValue("transform", transform);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  m_texture0->release();
  m_vao0->release();
  m_shaderProgram0->release();
}