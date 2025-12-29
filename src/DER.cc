#include "DER.h"
#include <QFileDialog>
#include <QMessageBox>
VideoGLWidget::VideoGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
  setMinimumSize(640, 480);
}

VideoGLWidget::~VideoGLWidget() {
  makeCurrent();
  if (textureID) {
    glDeleteTextures(1, &textureID);
  }
  if (VAO) {
    glDeleteVertexArrays(1, &VAO);
  }
  if (VBO) {
    glDeleteBuffers(1, &VBO);
  }
  if (EBO) {
    glDeleteBuffers(1, &EBO);
  }

  delete shaderProgram;
  doneCurrent();
}

void VideoGLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  setupShaders();
  setupGeometry();

  // create texture

  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  // parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void VideoGLWidget::setupShaders() {
  shaderProgram = new QOpenGLShaderProgram(this);

  // vertex shader
  const char *vertexShaderSource = R"(
  #version 330 core
  layout(location=0) in vec3 aPos; //create a vertex attribute vector of 3 floats called aPos
  layout(location=1) int vec2 aTexCoord;//create a vertex attribute vector of 2 ints called aTexCoord
  
  out vec2 TexCoord;
  
  uniform mat4 projection;
  
  void main() 
  {gl_Position =projection *vec4(aPos,1.0);TexCoord =aTexCoord;})";

  // fragment shader
  const char *fragmentShaderSource = R"(
  #version 330 core
  in vec2 TexCoord;
  out vec4 FragColor;
  
  uniform sampler2D texture1;
  
  void main() 
  {FragColor =texture(texture1,TexCoord);})";

  shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         vertexShaderSource);
  shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                         fragmentShaderSource);
  shaderProgram->link();
}

void VideoGLWidget::setupGeometry() {
  float vertices[] = {
      // positions   // texture coords
      1.0f,  1.0f,  0.0f, 1.0f, // top right
      1.0f,  -1.0f, 0.0f, 0.0f, // bottom right
      -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
      -1.0f, 1.0f,  0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void VideoGLWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);

  // 计算保持宽高比的投影矩阵
  projection.setToIdentity();

  if (videoWidth > 0 && videoHeight > 0) {
    float widgetAspect = (float)w / h;
    float videoAspect = (float)videoWidth / videoHeight;

    if (widgetAspect > videoAspect) {
      // 窗口更宽，基于高度缩放
      float scale = videoAspect / widgetAspect;
      projection.scale(scale, 1.0f, 1.0f);
    } else {
      // 窗口更高，基于宽度缩放
      float scale = widgetAspect / videoAspect;
      projection.scale(1.0f, scale, 1.0f);
    }
  }
}

void VideoGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  if (!hasFrame) {
    return;
  }

  shaderProgram->bind();
  shaderProgram->setUniformValue("projection", projection);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  shaderProgram->release();
}

void VideoGLWidget::updateTexture(uint8_t *data, int width, int height) {
  makeCurrent();

  videoWidth = width;
  videoHeight = height;
  hasFrame = true;

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);

  // 更新投影矩阵
  resizeGL(this->width(), this->height());

  update();

  doneCurrent();
}

void VideoGLWidget::clearTexture() {
  hasFrame = false;
  update();
}

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {

  glWidget = new VideoGLWidget(this);
  glWidget->setMinimumSize(640, 480);
  glWidget->move(10, 10);
  audioTrackCombo = new QComboBox(this);
  audioTrackCombo->setEnabled(false);
  audioTrackCombo->move(10, 460);
  subtitleCombo = new QComboBox(this);
  subtitleCombo->addItem("无字幕");
  subtitleCombo->setEnabled(false);
  subtitleCombo->move(40, 460);

  subtitleLabel = new QLabel("字幕", this);
  subtitleLabel->setEnabled(false);
  subtitleLabel->move(70, 460);

  playpauseButton = new QPushButton("播放", this);
  playpauseButton->move(100, 460);

  connect(playpauseButton, &QPushButton::clicked, this, &VideoWidget::openFile);

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &VideoWidget::updateFrame);

  initFFmpeg();
}

VideoWidget::~VideoWidget() { cleanupFFmpeg(); }

void VideoWidget::initFFmpeg() {}

void VideoWidget::cleanupFFmpeg() {
  if (timer->isActive()) {
    timer->stop();
  }

  if (swsCtx) {
    sws_freeContext(swsCtx);
    swsCtx = nullptr;
  }

  if (buffer) {
    av_free(buffer);
    buffer = nullptr;
  }

  if (rgbFrame) {
    av_frame_free(&rgbFrame);
  }

  if (frame) {
    av_frame_free(&frame);
  }

  if (packet) {
    av_packet_free(&packet);
  }

  if (videoCodecCtx) {
    avcodec_free_context(&videoCodecCtx);
  }

  if (audioCodecCtx) {
    avcodec_free_context(&audioCodecCtx);
  }

  if (subtitleCodecCtx) {
    avcodec_free_context(&subtitleCodecCtx);
  }

  if (formatCtx) {
    avformat_close_input(&formatCtx);
  }
}

void VideoWidget::openFile() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "选择视频文件", "", "视频文件 (*.mp4 *.avi *.mkv *.mov)");

  if (fileName.isEmpty()) {
    return;
  }

  cleanupFFmpeg();
  glWidget->clearTexture();

  // 打开视频文件
  if (avformat_open_input(&formatCtx, fileName.toUtf8().constData(), nullptr,
                          nullptr) < 0) {
    QMessageBox::critical(this, "错误", "无法打开文件");
    return;
  }

  if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
    QMessageBox::critical(this, "错误", "无法获取流信息");
    return;
  }

  loadStreams();
}

void VideoWidget::loadStreams() { // 初始化帧
  packet = av_packet_alloc();
  frame = av_frame_alloc();
  rgbFrame = av_frame_alloc();

  // 分配RGB缓冲区
  int numBytes = av_image_get_buffer_size(
      AV_PIX_FMT_RGB24, videoCodecCtx->width, videoCodecCtx->height, 1);
  buffer = (uint8_t *)av_malloc(numBytes);

  av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer,
                       AV_PIX_FMT_RGB24, videoCodecCtx->width,
                       videoCodecCtx->height, 1);
  swsCtx = sws_getContext(videoCodecCtx->width, videoCodecCtx->height,
                          videoCodecCtx->pix_fmt, videoCodecCtx->width,
                          videoCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BILINEAR,
                          nullptr, nullptr, nullptr);
}

void VideoWidget::playPause() {
  if (!isPlaying) {
    isPlaying = true;
    // playButton->setText("暂停");

    // 根据视频帧率设置定时器
    AVRational frameRate = formatCtx->streams[videoStreamIndex]->avg_frame_rate;
    int fps = av_q2d(frameRate);
    if (fps <= 0 || fps > 120)
      fps = 30;

    timer->start(1000 / fps);
  } else {
    isPlaying = false;
    // playButton->setText("播放");
    timer->stop();
  }
}

void VideoWidget::updateFrame() {
  if (decodeNextFrame()) {
    renderFrame(frame);
    renderSubtitle();
  }
}

bool VideoWidget::decodeNextFrame() {
  while (av_read_frame(formatCtx, packet) >= 0) {
    if (packet->stream_index == videoStreamIndex) {
      avcodec_send_packet(videoCodecCtx, packet);

      if (avcodec_receive_frame(videoCodecCtx, frame) == 0) {
        av_packet_unref(packet);
        return true;
      }
    } else if (packet->stream_index == currentSubtitleStreamIndex) {
      // 处理字幕
      if (subtitleCodecCtx) {
        AVSubtitle subtitle;
        int got_subtitle = 0;
        avcodec_decode_subtitle2(subtitleCodecCtx, &subtitle, &got_subtitle,
                                 packet);

        if (got_subtitle && subtitle.num_rects > 0) {
          if (subtitle.rects[0]->type == SUBTITLE_TEXT) {
            subtitleLabel->setText(QString::fromUtf8(subtitle.rects[0]->text));
            subtitleLabel->show();
          } else if (subtitle.rects[0]->type == SUBTITLE_ASS) {
            // 简化ASS字幕显示（移除格式标记）
            QString text = QString::fromUtf8(subtitle.rects[0]->ass);
            text = text.section(',', 9).trimmed(); // 提取文本部分
            subtitleLabel->setText(text);
            subtitleLabel->show();
          }
        }
        avsubtitle_free(&subtitle);
      }
    }

    av_packet_unref(packet);
  }

  // 文件结束，循环播放
  av_seek_frame(formatCtx, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
  avcodec_flush_buffers(videoCodecCtx);
  return false;
}

void VideoWidget::renderFrame(AVFrame *frame) {
  // 转换为RGB格式
  sws_scale(swsCtx, frame->data, frame->linesize, 0, videoCodecCtx->height,
            rgbFrame->data, rgbFrame->linesize);

  // 更新OpenGL纹理
  glWidget->updateTexture(rgbFrame->data[0], videoCodecCtx->width,
                          videoCodecCtx->height);
}

void VideoWidget::renderSubtitle() {
  // 字幕渲染已在decodeNextFrame中处理
}

void VideoWidget::onAudioTrackChanged(int index) {
  if (index < 0)
    return;

  int streamIndex = audioTrackCombo->itemData(index).toInt();
  currentAudioStreamIndex = streamIndex;

  // 实际应用中需要重新初始化音频解码器和输出设备
  // 这里仅记录选择的音轨索引
}

void VideoWidget::onSubtitleTrackChanged(int index) {
  if (subtitleCodecCtx) {
    avcodec_free_context(&subtitleCodecCtx);
    subtitleCodecCtx = nullptr;
  }

  subtitleLabel->hide();
  subtitleLabel->clear();

  if (index == 0) {
    currentSubtitleStreamIndex = -1;
    return;
  }

  int streamIndex = subtitleCombo->itemData(index).toInt();
  currentSubtitleStreamIndex = streamIndex;

  AVCodecParameters *codecParams = formatCtx->streams[streamIndex]->codecpar;
  const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);

  if (codec) {
    subtitleCodecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(subtitleCodecCtx, codecParams);
    avcodec_open2(subtitleCodecCtx, codec, nullptr);
  }
}