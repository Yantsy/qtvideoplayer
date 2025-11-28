#pragma once

#include <QAudioOutput>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QPushButton>
#include <QShortcut>
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
  QPushButton *play = nullptr, *pause = nullptr, *open = nullptr,
              *audio = nullptr, *fullscreen = nullptr;
  QSlider *slider = nullptr, *volume = nullptr;
  QString *path = nullptr;
  QAudioOutput *audiooutput = nullptr;
  QShortcut *fulscr_shortcut = nullptr, *fulscr_shortcut_x = nullptr;
  QPushButton *buttonset(QPushButton *button, std::string iconpath, int x,
                         int y, int w, int h);

  // ui参数表
  int ww = 1600, wh = 900;
  int bw = 80, bh = 30;

private slots:
  void openfile();
};
