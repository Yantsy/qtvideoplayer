#include "home.h"

home::home(QWidget *parent) : QWidget(parent) {
  this->setWindowTitle("Qt Video Player");
  this->setFixedSize(ww, wh);

  // 添加组件
  videoplayer = new QMediaPlayer(this);
  videowidget = new QVideoWidget(this);
  audiooutput = new QAudioOutput(this);
  videowidget->setGeometry(20, 10, ww - 40, wh - 60);
  videoplayer->setVideoOutput(videowidget);
  videoplayer->setAudioOutput(audiooutput);
  slider = new QSlider(Qt::Horizontal, this);
  slider->setGeometry(videowidget->geometry().x(),
                      videowidget->geometry().y() +
                          videowidget->geometry().height(),
                      videowidget->geometry().width(), 10);
  connect(videoplayer, &QMediaPlayer::durationChanged, slider,
          &QSlider::setMaximum);
  connect(videoplayer, &QMediaPlayer::positionChanged, slider,
          &QSlider::setValue);
  connect(slider, &QSlider::sliderMoved, videoplayer,
          &QMediaPlayer::setPosition);
  play = new QPushButton("Play", this);
  play->move(10, wh - 30);
  connect(play, &QPushButton::clicked, videoplayer, &QMediaPlayer::play);
  pause = new QPushButton("Pause", this);
  pause->move(110, wh - 30);
  connect(pause, &QPushButton::clicked, videoplayer, &QMediaPlayer::pause);
  open = new QPushButton("Open", this);
  open->move(210, wh - 30);
  connect(open, &QPushButton::clicked, this, &home::openfile);
}

home::~home() {}

void home::openfile() {
  path = new QString(QFileDialog::getOpenFileName(this, "open file from ..."));

  if (!path->isEmpty()) {
    videoplayer->setSource(QUrl::fromLocalFile(*path));
    videoplayer->play();
  }
}

void home::slidermove() {}