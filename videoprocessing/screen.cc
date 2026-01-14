#include "screen.h"
screen::screen(QWidget *parent) : QOpenGLWidget(parent) {
  this->setContentsMargins(0, 0, 0, 0);
  this->setMinimumSize(600, 600);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QTimer *frameTimer = new QTimer(this);
  connect(frameTimer, &QTimer::timeout, this, QOverload<>::of(&screen::update));
  frameTimer->start(16); // approximately 60 FPS
}
screen::~screen() {
  makeCurrent();
  delete vertexShader;
  delete fragmentShader;
  delete shaderProgram;
  delete VBO;
  delete VAO;
  delete EBO;
  delete fragmentShader1;
  delete shaderProgram1;
  delete VBO1;
  delete VAO1;
  delete VBO2;
  delete VAO2;
  delete VBO3;
  delete VAO3;
  delete texture;
  doneCurrent();
}

void screen::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  timer.start();

  // vertex input
  /*
  float vertices[] = {0.2f,  0.7f,  0.0f, 0.1f,  -0.3f, 0.0f,
                      -0.8f, -0.3f, 0.0f, -0.9F, 0.7f,  0.0f};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    EBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  EBO->create();
  EBO->bind();
  EBO->allocate(indices, sizeof(indices));
  */
  QImage img(":/resources/container.jpg");
  float vertices[] = {0.0f, 0.5f, 0.0f, -0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.0f};

  float vertices1[] = {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f};

  float vertices2[] = {0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, 0.5f, -0.5f, 0.0f,
                       0.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  1.0f};

  float vertices3[] = {0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                       0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                       -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                       -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  const char *vertexShaderSource = R"(
#version 330 core 
layout(location = 0) in vec3 aPos;

 out vec4 vertexColor;
void main()
{
    gl_Position =vec4(aPos, 1.0);
    vertexColor = vec4(120.0, 78.0, 135.0, 255.0)/255.0;
}
)";
  const char *vertexShaderSource1 = R"(
  #version 330 core
  layout(location =0) in vec3 aPos;
  layout(location =1)in vec3 aColor;
  out vec3 ourColor;
  void main(){
  gl_Position =vec4(aPos,1.0);
  ourColor =aColor;})";

  const char *vertexShaderSource2 = R"(
  #version 330 core
  layout(location =0) in vec3 aPos;
  void main(){
  gl_Position =vec4(aPos,1.0);
 })";

  const char *vertexShaderSource3 = R"(
  #version 330 core
  layout(location=0) in vec3 aPos;
  layout(location=1) in vec3 aColor;
  layout(location=2) in vec2 aTexCoord;
  out vec3 ourColor;
  out vec2 TexCoord;
  void main(){
  gl_Position =vec4(aPos,1.0);
  ourColor =aColor;
  TexCoord =aTexCoord;})";

  const char *fragmentShaderSource5 = R"(
  #version 330 core
  out vec4 FragColor;
  in vec2 TexCoord;
  in vec3 ourColor;
  uniform sampler2D texture; 
  void main(){
  FragColor =texture2D(texture,TexCoord)*vec4(ourColor,1.0);})";

  const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec4 vertexColor;
void main()
{
    FragColor = vertexColor;
}
)";

  const char *fragmentShaderSource1 = R"(
  #version 330 core
  out vec4 FragColor;

  void main(){
      FragColor = vec4(0.0, 1.0, 0.0, 1.0);
  } 
  )";

  const char *fragmentShaderSource2 = R"(
  #version 330 core
  out vec4 FragColor;
  uniform vec4 ourColor;

  void main(){
      FragColor = ourColor;
  } 
  )";

  const char *fragmentShaderSource3 = R"(
  #version 330 core
  out vec4 FragColor;
  in vec3 ourColor;
  void main(){FragColor =vec4(ourColor,1.0);})";
  // how to make the color change
  const char *fragmentShaderSource4 = R"(
  #version 330 core
  out vec4 FragColor;
  uniform vec4 aColor;
  uniform vec4 bColor;
  uniform vec4 cColor;
  void main(){FragColor =vec4(aColor.x,bColor.y,cColor.z,1.0);})";

  VAO = new QOpenGLVertexArrayObject();
  VAO->create();
  VAO->bind();

  VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  VBO->create();
  VBO->bind();
  VBO->allocate(vertices, sizeof(vertices));

  // vertex shader

  // compile vertex shader
  vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
  vertexShader->compileSourceCode(vertexShaderSource);

  // fragment shader

  // compile fragment shader
  fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
  fragmentShader->compileSourceCode(fragmentShaderSource);

  // shader program
  shaderProgram = new QOpenGLShaderProgram();
  shaderProgram->addShader(vertexShader);
  shaderProgram->addShader(fragmentShader);
  shaderProgram->link();
  shaderProgram->bind();

  // link vertex attributes

  shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
  shaderProgram->enableAttributeArray(0);

  VAO->release();
  shaderProgram->release();

  VAO1 = new QOpenGLVertexArrayObject();
  VAO1->create();
  VAO1->bind();

  VBO1 = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  VBO1->create();
  VBO1->bind();
  VBO1->allocate(vertices1, sizeof(vertices1));

  fragmentShader1 = new QOpenGLShader(QOpenGLShader::Fragment);
  fragmentShader1->compileSourceCode(fragmentShaderSource2);

  shaderProgram1 = new QOpenGLShaderProgram();
  shaderProgram1->addShader(vertexShader);
  shaderProgram1->addShader(fragmentShader1);
  shaderProgram1->link();
  shaderProgram1->bind();
  shaderProgram1->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
  shaderProgram1->enableAttributeArray(0);

  VAO1->release();
  shaderProgram1->release();

  VAO2 = new QOpenGLVertexArrayObject();
  VAO2->create();
  VAO2->bind();

  VBO2 = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  VBO2->create();
  VBO2->bind();
  VBO2->allocate(vertices2, sizeof(vertices2));

  vertexShader->compileSourceCode(vertexShaderSource1);
  fragmentShader->compileSourceCode(fragmentShaderSource3);

  shaderProgram2 = new QOpenGLShaderProgram();
  shaderProgram2->addShader(vertexShader);
  shaderProgram2->addShader(fragmentShader);
  shaderProgram2->link();
  shaderProgram2->bind();
  shaderProgram2->setAttributeBuffer(0, GL_FLOAT, 0, 3, 6 * sizeof(float));
  shaderProgram2->enableAttributeArray(0);
  shaderProgram2->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3,
                                     6 * sizeof(float));
  shaderProgram2->enableAttributeArray(1);

  VAO2->release();
  shaderProgram2->release();

  VAO3 = new QOpenGLVertexArrayObject();
  VAO3->create();
  VAO3->bind();

  VBO3 = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  VBO3->create();
  VBO3->bind();
  VBO3->allocate(vertices3, sizeof(vertices3));

  EBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  EBO->create();
  EBO->bind();
  EBO->allocate(indices, sizeof(indices));

  vertexShader->compileSourceCode(vertexShaderSource3);
  fragmentShader->compileSourceCode(fragmentShaderSource5);
  texture = new QOpenGLTexture(img);

  shaderProgram->addShader(vertexShader);
  shaderProgram->addShader(fragmentShader);
  shaderProgram->link();
  shaderProgram->bind();

  shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 8 * sizeof(float));
  shaderProgram->enableAttributeArray(0);
  shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3,
                                    8 * sizeof(float));
  shaderProgram->enableAttributeArray(1);
  shaderProgram->setAttributeBuffer(2, GL_FLOAT, 6 * sizeof(float), 2,
                                    8 * sizeof(float));
  shaderProgram->enableAttributeArray(2);

  VAO3->release();
  shaderProgram->release();
}

void screen::resizeGL(int w, int h) {
  projection.setToIdentity();
  glViewport(0, 0, w, h);
}

void screen::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  shaderProgram->bind();
  VAO->bind();
  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  VAO->release();
  shaderProgram->release();

  shaderProgram1->bind();
  VAO1->bind();
  float timeValue = timer.elapsed() / 1000.0f;
  float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
  float redValue = (cos(timeValue) / 2.0f) + 0.5f;
  float blueValue = (sin(timeValue + 3.14f) / 2.0f) + 0.5f;
  shaderProgram1->setUniformValue("ourColor", 0.0f, greenValue, 0.0f, 1.0f);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  VAO1->release();
  shaderProgram1->release();

  shaderProgram2->bind();
  VAO2->bind();
  glDrawArrays(GL_TRIANGLES, 0, 3);

  VAO2->release();
  shaderProgram2->release();

  shaderProgram->bind();
  VAO3->bind();
  texture->bind(0);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  texture->release();
  VAO3->release();
  shaderProgram->release();
}