#include "VideoWidget.h"

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent), isFullscreen(false), isPlaying(false),
      controlBarVisible(true) {
  setupUI();
  setupConnections();
}

VideoWidget::~VideoWidget() {}

void VideoWidget::setupUI() {
  this->setContentsMargins(0, 0, 0, 0);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mediaPlayer = new QMediaPlayer(this);
  videoWidget = new QVideoWidget(this);
  videoWidget->setGeometry(this->rect());
  videoWidget->setStyleSheet("background-color: black;");
  videoWidget->lower();
  audioOutput = new QAudioOutput(this);
  mediaPlayer->setVideoOutput(videoWidget);
  mediaPlayer->setAudioOutput(audioOutput);

  controlBar = new QWidget(this);
  controlBar->setObjectName("controlBar");
  controlBar->setContentsMargins(0, 0, 0, 0);
  controlBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(videoWidget);
  mainLayout->addWidget(controlBar);

  QVBoxLayout *controlLayout = new QVBoxLayout(controlBar);
  controlLayout->setContentsMargins(10, 5, 10, 5);
  controlLayout->setSpacing(5);
}