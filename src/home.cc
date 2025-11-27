#include "home.h"

home::home(QWidget *parent) : QWidget(parent) {
  this->setWindowTitle("Qt Video Player");
  this->setFixedSize(ww, wh);

  // 添加组件
  videoplayer = new QMediaPlayer(this);
  videowidget = new QVideoWidget(this);
  // videowidget->setWindowFlags(Qt::);
  audiooutput = new QAudioOutput(this);
  videowidget->setGeometry(20, 10, 800, 600);
  videoplayer->setVideoOutput(videowidget);
  videoplayer->setAudioOutput(audiooutput);
  slider = new QSlider(Qt::Horizontal, this);
  slider->setGeometry(videowidget->geometry().x(),
                      videowidget->geometry().y() +
                          videowidget->geometry().height(),
                      videowidget->geometry().width() - 80, 10);
  play = new QPushButton("Play", this);
  play->move(10, videowidget->geometry().y() +
                     videowidget->geometry().height() + 50);
  pause = new QPushButton("Pause", this);
  pause->move(110, videowidget->geometry().y() +
                       videowidget->geometry().height() + 50);
  open = new QPushButton("Open", this);
  open->move(210, videowidget->geometry().y() +
                      videowidget->geometry().height() + 50);
  fullscreen = new QPushButton("[--]", this);
  fullscreen->move(310, videowidget->geometry().y() +
                            videowidget->geometry().height() + 50);
  audio = new QPushButton("V", this);
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
  fulscr_shortcut =
      new QShortcut(QKeySequence("F"), fullscreen, SIGNAL(clicked()));
  fulscr_shortcut_x = new QShortcut(QKeySequence("ESC"), videowidget);
  connect(fullscreen, &QPushButton::clicked, this, [=]() {
    if (!videowidget->isFullScreen()) {
      videowidget->setFullScreen(true);
      // videowidget->show();
      this->hide();
    } else {
      videowidget->setFullScreen(false);
      videowidget->setGeometry(20, 10, 800, 600);
      this->show();
    }
  });
  connect(fulscr_shortcut_x, &QShortcut::activated, this, [=]() {
    if (!videowidget->isFullScreen()) {
      videowidget->setFullScreen(true);
      // videowidget->show();
      this->hide();
    } else {
      videowidget->setFullScreen(false);
      videowidget->setGeometry(20, 10, 800, 600);
      this->show();
    }
  });
}
home::~home() {}

void home::openfile() {
  path = new QString(QFileDialog::getOpenFileName(this, "open file from ..."));

  if (!path->isEmpty()) {
    videoplayer->setSource(QUrl::fromLocalFile(*path));
    videoplayer->play();
  }
}
