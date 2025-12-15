#pragma once

#include "playlist.h"
#include "polygon.h"
#include "scrollarea.h"
#include "videowidget.h"
#include "yslider.h"
#include <QAudioOutput>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
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
  QPushButton *playblock = nullptr, *play = nullptr, *pause = nullptr,
              *open = nullptr, *outview = nullptr, *skipnext = nullptr,
              *skiplast = nullptr, *sound = nullptr, *subtitle = nullptr,
              *audio = nullptr, *fullscreen = nullptr, *setting = nullptr;
  QSlider *slider = nullptr, *volume = nullptr;
  QString *path = nullptr;
  QAudioOutput *audiooutput = nullptr;
  QShortcut *fulscr_shortcut = nullptr, *fulscr_shortcut_x = nullptr;
  QPushButton *buttonset(QPushButton *button, std::string iconpath, int x,
                         int y, int w, int h);
  videoget *videowid = nullptr;
  // ui参数表
  int ww = 1307 - 120, wh = 644;
  int bw = 80, bh = 30;

  int id = 0;
  QString *family = nullptr;
  QFont *iconfont = nullptr;
  QPainter *homepainter = nullptr;

  polygon *console = nullptr;

  yslider *yantsyslider = nullptr, *yvolume = nullptr;
  Myscrollarea *season = nullptr, *episode = nullptr;
  MyPlayList *playlist = nullptr;
  void volumeswicher(int value) noexcept;

private slots:
  void openfile();

protected:
  void paintEvent(QPaintEvent *event) override;
};
