#pragma once

#include "playlist.h"
#include "polygon.h"
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
  QMediaPlayer *videoplayer = nullptr;
  // void Linkplaylist(MyPlayList *pplaylist) { myplaylist = pplaylist; };
  ~home();
  QPushButton *skipnext = nullptr, *skiplast = nullptr;

private:
  QVideoWidget *videowidget = nullptr;
  QPushButton *playblock = nullptr, *play = nullptr, *pause = nullptr,
              *open = nullptr, *outview = nullptr, *sound = nullptr,
              *subtitle = nullptr, *audio = nullptr, *fullscreen = nullptr,
              *setting = nullptr;
  QSlider *slider = nullptr, *volume = nullptr;
  QString *path = nullptr;
  QAudioOutput *audiooutput = nullptr;
  QShortcut *fulscr_shortcut = nullptr, *fulscr_shortcut_x = nullptr;
  QPushButton *buttonset(QPushButton *button, std::string iconpath, int x,
                         int y, int w, int h);
  videoget *videowid = nullptr;
  // ui参数表
  int ww, wh;
  int bw = 80, bh = 30;

  int id = 0;
  QString *family = nullptr;
  QFont *iconfont = nullptr;
  QPainter *homepainter = nullptr;

  polygon *console = nullptr;

  yslider *yantsyslider = nullptr, *yvolume = nullptr;
  MyPlayList *myplaylist = nullptr;
  void volumeswicher(int value) noexcept;

private slots:

protected:
  void paintEvent(QPaintEvent *event) override;
};
