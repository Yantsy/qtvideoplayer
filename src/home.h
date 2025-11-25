#pragma once

#include <QFileDialog>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QVideoWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

class home : public QWidget {
  Q_OBJECT
public:
  home(QWidget *parent = nullptr);
  ~home();

private:
  QMediaPlayer *videoplayer = nullptr;
  QVideoWidget *videowidget = nullptr;
  QPushButton *play = nullptr, *pause = nullptr, *open = nullptr;
  QSlider *slider = nullptr;
  QString *path = nullptr;
  QFileDialog *dialog = nullptr;

  // ui参数表
  int ww = 800, wh = 600;
  int bw = 80, bh = 30;

private slots:
  void openfile();
};
