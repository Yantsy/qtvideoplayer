
#include "../videoprocessing/glwidget.h"
#include "home.h"
#include "playlist.h"

#include <QMainWindow>

// #include <Eigen/Core>
// #include <opencv2/opencv.hpp>
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QMainWindow mainWindow;
  mainWindow.setWindowTitle("Yantsy Video Player");
  mainWindow.setMinimumSize(920, 500);
  mainWindow.setContentsMargins(0, 0, 0, 0);

  // MyGLWidget player;
  // player.show();

  home w;
  w.setMinimumSize(mainWindow.width() * 0.66666, mainWindow.height());
  w.setContentsMargins(0, 0, 0, 0);
  w.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  MyPlayList playlist;
  playlist.setQMediaPlayer(w.videoplayer);
  // ffmpegvideowidget.openFile(playlist.source());
  playlist.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  playlist.setMinimumSize(mainWindow.width() - w.width(), mainWindow.height());
  QObject::connect(w.skipnext, &QPushButton::clicked, &playlist,
                   &MyPlayList::nextVideo);
  QObject::connect(w.skiplast, &QPushButton::clicked, &playlist,
                   &MyPlayList::lastVideo);

  QSplitter *splitter = new QSplitter(&mainWindow);
  mainWindow.setCentralWidget(splitter);
  splitter->setOrientation(Qt::Horizontal);
  splitter->addWidget(&w);
  splitter->addWidget(&playlist);
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setHandleWidth(1);
  splitter->setStyleSheet(
      "QSplitter::handle { background-color: rgba(51, 232, 219, 50); }"
      "QSplitter::handle:hover { background-color: rgba(150, 150, 150, 100); "
      "}");
  splitter->setChildrenCollapsible(false);

  mainWindow.show();
  splitter->setStretchFactor(0, 2);
  splitter->setStretchFactor(1, 1);

  return a.exec();
}