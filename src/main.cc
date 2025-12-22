#include "home.h"
#include <QMainWindow>
int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QMainWindow mainWindow;
  home w;
  MyPlayList playlist;
  // w.Linkplaylist(&playlist);
  playlist.setQMediaPlayer(w.videoplayer);
  playlist.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  QObject::connect(w.skipnext, &QPushButton::clicked, &playlist,
                   &MyPlayList::nextVideo);
  QObject::connect(w.skiplast, &QPushButton::clicked, &playlist,
                   &MyPlayList::lastVideo);

  QSplitter *splitter = new QSplitter(&mainWindow);
  splitter->setOrientation(Qt::Horizontal);
  splitter->addWidget(&w);
  splitter->addWidget(&playlist);
  mainWindow.setCentralWidget(splitter);
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setHandleWidth(10);
  mainWindow.setWindowTitle("Yantsy Video Player");
  mainWindow.setMinimumSize(1307 - 120, 644);
  mainWindow.setContentsMargins(0, 0, 0, 0);
  w.setMinimumSize(mainWindow.width() * 2 / 3, mainWindow.height());
  playlist.setMinimumSize(mainWindow.width() * 1 / 3, mainWindow.height());
  mainWindow.show();
  return a.exec();
}