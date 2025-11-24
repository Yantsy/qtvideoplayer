#include "home.h"

home::home(QWidget *parent) : QWidget(parent) {
  this->setWindowTitle("Qt Video Player");
  this->setFixedSize(ww, wh);

  // 添加组件
  videoplayer = new QMediaPlayer(this);
  videowidget = new QVideoWidget(this);
  videowidget->setGeometry(10, 10, ww - 60, wh - 60);
  videoplayer->setVideoOutput(videowidget);
  slider = new QSlider(Qt::Horizontal, this);
  slider->setGeometry(videowidget->geometry().x(),
                      videowidget->geometry().y() +
                          videowidget->geometry().height(),
                      videowidget->geometry().width(), 10);
  play = new QPushButton("Play", this);
  play->move(100, wh - 30);
  pause = new QPushButton("Pause", this);
  pause->move(200, wh - 30);
  open = new QPushButton("Open", this);
  open->move(300, wh - 30);

  path = new QString(QFileDialog::getOpenFileName(this, "open file from ..."));

  if (!path->isEmpty()) {
    videoplayer->setSource(QUrl::fromLocalFile(*path));
    videoplayer->play();
  }
}

home::~home() {}