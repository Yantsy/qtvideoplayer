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
                      videowidget->geometry().width() - 80, 10);
  play = new QPushButton("Play", this);
  play->move(10, wh - 30);
  pause = new QPushButton("Pause", this);
  pause->move(110, wh - 30);
  open = new QPushButton("Open", this);
  open->move(210, wh - 30);
  audio = new QPushButton("-_-", this);
  audio->setFixedSize(30, 20);
  audio->move(videowidget->geometry().width() - 60, slider->geometry().y());
  volume = new QSlider(Qt::Horizontal, this);
  volume->setGeometry(audio->geometry().x() + 30, audio->geometry().y(), 50,
                      10);
  volume->setRange(0, 100);
  volume->setValue(50);
  // 连接信号与槽

  connect(volume, &QSlider::valueChanged, this, [=](int value) {
    qreal dB = -60.0 + (value / 100.0) * 60.0; // 将线性值转换为分贝值
    qreal linearVolume = std::pow(10.0, dB / 20.0);
    audiooutput->setVolume(linearVolume);
  });
  connect(volume, &QSlider::sliderMoved, audiooutput, &QAudioOutput::setVolume);
  connect(videoplayer, &QMediaPlayer::durationChanged, slider,
          &QSlider::setMaximum);
  connect(videoplayer, &QMediaPlayer::positionChanged, slider,
          &QSlider::setValue);
  connect(slider, &QSlider::sliderMoved, videoplayer,
          &QMediaPlayer::setPosition);
  connect(play, &QPushButton::clicked, videoplayer, &QMediaPlayer::play);
  connect(pause, &QPushButton::clicked, videoplayer, &QMediaPlayer::pause);
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
