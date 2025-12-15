#pragma once

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTreeWidget>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

class MyPlayList : public QWidget {
  Q_OBJECT
public:
  MyPlayList(QWidget *parent = nullptr);
  ~MyPlayList();
  void setQVideoWidget(QVideoWidget *pvideowidget);
  void setAudioOutput(QAudioOutput *paudio);
  void setQMediaPlayer(QMediaPlayer *pplayer);

private:
  // funtions
  void initUI();
  void loadSavedFolders();
  void saveFolders();
  void scanFolderStructure(const QString &Path, QTreeWidgetItem *parent);
  void loadVideosFromFolder(const QString &folderPath);
  void updateVideoList();
  void updatePageLabel();
  bool isVideoFile(const QString &fileName);

  // widgets

  QTreeWidget *folderTree = nullptr;
  QListWidget *videoList = nullptr;
  QPushButton *addFolderButton = nullptr;
  QPushButton *prevPageButton = nullptr;
  QPushButton *nextPageButton = nullptr;
  QLabel *pageLabel = nullptr;
  QMediaPlayer *player = nullptr;
  QVideoWidget *videoWidget = nullptr;
  QAudioOutput *audiooutput = nullptr;

  // data

  QSet<QString> savedFolders;
  QMap<QTreeWidgetItem *, QString> itemPathMap;
  QVector<QString> currentVideos;
  int currentPage;
  int itemsPerPage;
  QSet<QString> videoExtensions;

  QString configFile;

public slots:
  void lastVideo();
  void nextVideo();

private slots:

  void addFolder();
  void onFolderSelected(QTreeWidgetItem *item, int column);
  void playVideo(QListWidgetItem *item);
  void nextPage();
  void prevPage();

protected:
};