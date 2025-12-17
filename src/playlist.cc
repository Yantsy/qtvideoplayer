#include "playlist.h"
#include "label.h"

MyPlayList::MyPlayList(QWidget *parent)
    : QWidget(parent), currentPage(0), itemsPerPage(20) {

  configFile = QApplication::applicationDirPath() + "/playlist_config.json";
  qDebug() << "配置文件路径：" << configFile;

  videoExtensions << "mp4"
                  << "avi"
                  << "mkv"
                  << "mov"
                  << "flv"
                  << "wmv"
                  << "webm"
                  << "mpg"
                  << "mpeg"
                  << "m4v";

  initUI();
  loadSavedFolders();
}

MyPlayList::~MyPlayList() { saveFolders(); }

void MyPlayList::setQVideoWidget(QVideoWidget *pvideowidget) {
  videoWidget = pvideowidget;
}
void MyPlayList::setAudioOutput(QAudioOutput *paudio) { audiooutput = paudio; }
void MyPlayList::setQMediaPlayer(QMediaPlayer *pplayer) {
  player = pplayer;
  connect(player, &QMediaPlayer::mediaStatusChanged, this,
          [=](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
              nextVideo();
            }
          });
}

void MyPlayList::initUI() {
  // Initialize UI components and layout here

  // QWidget *centralWidget = new QWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // TOP BUTTONS
  QHBoxLayout *topLayout = new QHBoxLayout();
  addFolderButton = new QPushButton(this);
  addFolderButton->setIcon(QIcon(":/resources/addfile.png"));
  addFolderButton->setIconSize(QSize(24, 24));
  addFolderButton->setFlat(true);
  connect(addFolderButton, &QPushButton::clicked, this, &MyPlayList::addFolder);
  downloadButton = new QPushButton(this);
  downloadButton->setIcon(QIcon(":/resources/download.png"));
  downloadButton->setIconSize(QSize(24, 24));
  downloadButton->setFlat(true);
  inboxButton = new QPushButton(this);
  inboxButton->setIcon(QIcon(":/resources/inbox.png"));
  inboxButton->setIconSize(QSize(24, 24));
  inboxButton->setFlat(true);
  menuButton = new QPushButton(this);
  menuButton->setIcon(QIcon(":/resources/menu.png"));
  menuButton->setIconSize(QSize(24, 24));
  menuButton->setFlat(true);
  moreButton = new QPushButton(this);
  moreButton->setIcon(QIcon(":/resources/more_horiz.png"));
  moreButton->setIconSize(QSize(24, 24));
  moreButton->setFlat(true);
  topLayout->addWidget(addFolderButton);
  topLayout->addWidget(downloadButton);
  topLayout->addWidget(inboxButton);
  topLayout->addWidget(menuButton);
  topLayout->addWidget(moreButton);

  topLayout->addStretch();
  mainLayout->addLayout(topLayout);

  // splitter
  QSplitter *splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);

  // season part
  MyLabel *folderlabel = new MyLabel("文件夹列表", this);
  folderlabel->setColor(QColor(235, 88, 88));
  mainLayout->addWidget(folderlabel);
  folderTree = new QTreeWidget(this);
  folderTree->setHeaderHidden(true);
  scrollbarsetup(folderTree);
  connect(folderTree, &QTreeWidget::itemClicked, this,
          &MyPlayList::onFolderSelected);

  // episode part
  QWidget *episodeWidget = new QWidget(this);

  QVBoxLayout *episodeLayout = new QVBoxLayout(episodeWidget);
  episodeLayout->setContentsMargins(0, 0, 0, 0);
  MyLabel *listlabel = new MyLabel("视频列表", this);
  listlabel->setColor(QColor(235, 88, 88));
  episodeLayout->addWidget(listlabel);

  videoList = new QListWidget(this);
  scrollbarsetup(videoList);

  connect(videoList, &QListWidget::itemDoubleClicked, this,
          &MyPlayList::playVideo);
  episodeLayout->addWidget(videoList, 1);

  // pages
  QHBoxLayout *pageLayout = new QHBoxLayout();
  pageLayout->setContentsMargins(0, 0, 0, 0);
  prevPageButton = new QPushButton(this);
  prevPageButton->setIcon(QIcon(":/resources/fast_rewind.png"));
  prevPageButton->setIconSize(QSize(24, 24));
  prevPageButton->setFlat(true);
  nextPageButton = new QPushButton(this);
  nextPageButton->setIcon(QIcon(":/resources/fast_forward.png"));
  nextPageButton->setIconSize(QSize(24, 24));
  nextPageButton->setFlat(true);
  pageLabel = new QLabel("页码：1/1", this);

  connect(prevPageButton, &QPushButton::clicked, this, &MyPlayList::prevPage);
  connect(nextPageButton, &QPushButton::clicked, this, &MyPlayList::nextPage);

  pageLayout->addWidget(prevPageButton);
  pageLayout->addWidget(pageLabel);
  pageLayout->addWidget(nextPageButton);
  episodeLayout->addLayout(pageLayout);

  splitter->addWidget(folderTree);
  splitter->addWidget(episodeWidget);
  folderTree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  episodeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 2);

  mainLayout->addWidget(splitter);
}

void MyPlayList::addFolder() {
  QString folderPath = QFileDialog::getExistingDirectory(
      this, "选择文件夹", "~", QFileDialog::ShowDirsOnly);

  if (folderPath.isEmpty())
    return;

  if (savedFolders.contains(folderPath)) {
    QMessageBox::information(this, "提示", "该文件夹已添加！");
    return;
  }
  savedFolders.insert(folderPath);
  qDebug() << "当前保存的文件夹数量：" << savedFolders.size();

  QTreeWidgetItem *rootItem = new QTreeWidgetItem(folderTree);
  rootItem->setText(0, QDir(folderPath).dirName());
  itemPathMap[rootItem] = folderPath;

  scanFolderStructure(folderPath, rootItem);

  saveFolders();
}

void MyPlayList::scanFolderStructure(const QString &Path,
                                     QTreeWidgetItem *parent) {
  QDir dir(Path);
  QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  for (const QString &subDir : subDirs) {
    QString subPath = dir.filePath(subDir);
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, subDir);
    itemPathMap[item] = subPath;

    scanFolderStructure(subPath, item);
  }
}

void MyPlayList::onFolderSelected(QTreeWidgetItem *item, int column) {

  QString folderPath = itemPathMap.value(item);

  if (folderPath.isEmpty())
    return;

  loadVideosFromFolder(folderPath);
}

void MyPlayList::loadVideosFromFolder(const QString &folderPath) {
  currentVideos.clear();
  currentPage = 0;

  QDir dir(folderPath);
  QStringList files = dir.entryList(QDir::Files);

  for (const QString &file : files) {
    if (isVideoFile(file)) {
      currentVideos.append(dir.filePath(file));
    }
  }

  updateVideoList();
}

bool MyPlayList::isVideoFile(const QString &fileName) {
  QString suffix = QFileInfo(fileName).suffix().toLower();
  return videoExtensions.contains(suffix);
}

void MyPlayList::updateVideoList() {
  videoList->clear();

  int totalPages = (currentVideos.size() + itemsPerPage - 1) / itemsPerPage;
  if (totalPages == 0)
    totalPages = 1;
  int startIdx = currentPage * itemsPerPage;
  int endIdx = qMin(startIdx + itemsPerPage, currentVideos.size());
  for (int i = startIdx; i < endIdx; ++i) {
    QListWidgetItem *item =
        new QListWidgetItem(QFileInfo(currentVideos[i]).fileName());
    item->setData(Qt::UserRole, currentVideos[i]);
    videoList->addItem(item);
  }

  updatePageLabel();
}

void MyPlayList::updatePageLabel() {
  int totalPages = (currentVideos.size() + itemsPerPage - 1) / itemsPerPage;
  if (totalPages == 0)
    totalPages = 1;

  pageLabel->setText(
      QString("页码：%1/%2").arg(currentPage + 1).arg(totalPages));

  prevPageButton->setEnabled(currentPage > 0);
  nextPageButton->setEnabled(currentPage < totalPages - 1);
}

void MyPlayList::playVideo(QListWidgetItem *item) {
  QString videoPath = item->data(Qt::UserRole).toString();
  player->setSource(QUrl::fromLocalFile(videoPath));
  player->play();
}
void MyPlayList::lastVideo() {
  QString currentVideoPath = player->source().toLocalFile();
  int currentIndex = currentVideos.indexOf(currentVideoPath);
  int lastone = currentVideos.size() - 1;
  if (currentIndex == -1)
    return;
  if (currentIndex == 0) {
    player->setSource(QUrl::fromLocalFile(currentVideos[lastone]));
    player->play();
  } else {
    player->setSource(QUrl::fromLocalFile(currentVideos[currentIndex - 1]));
    player->play();
  }
}
void MyPlayList::nextVideo() {
  QString currentVideoPath = player->source().toLocalFile();
  int currentIndex = currentVideos.indexOf(currentVideoPath);
  if (currentIndex == -1)
    return;
  if (currentIndex < currentVideos.size() - 1) {
    player->setSource(QUrl::fromLocalFile(currentVideos[currentIndex + 1]));
    player->play();
  } else {
    player->setSource(QUrl::fromLocalFile(currentVideos[0]));
    player->play();
  }
}
void MyPlayList::nextPage() {
  int totalPages = (currentVideos.size() + itemsPerPage - 1) / itemsPerPage;
  if (currentPage < totalPages - 1) {
    currentPage++;
    updateVideoList();
  }
}

void MyPlayList::prevPage() {
  if (currentPage > 0) {
    currentPage--;
    updateVideoList();
  }
}

void MyPlayList::loadSavedFolders() {
  QFile file(configFile);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "配置文件不存在:" << configFile;
    return;
  }

  QByteArray data = file.readAll();
  file.close();
  qDebug() << "读取到的配置文件内容：" << data;

  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isArray()) {
    qDebug() << "配置文件格式错误";
    return;
  }

  QJsonArray array = doc.array();
  qDebug() << "配置文件中的文件数量：" << array.size();

  for (const QJsonValue &value : array) {
    QString folderPath = value.toString();
    qDebug() << "加载文件夹：" << folderPath;
    if (QDir(folderPath).exists()) {
      savedFolders.insert(folderPath);

      QTreeWidgetItem *rootItem = new QTreeWidgetItem(folderTree);
      rootItem->setText(0, QDir(folderPath).dirName());
      itemPathMap[rootItem] = folderPath;
      scanFolderStructure(folderPath, rootItem);
    } else {
      qDebug() << "文件夹不存在：" << folderPath;
    }
  }
}

void MyPlayList::saveFolders() {
  QJsonArray array;
  for (const QString &folderPath : savedFolders) {
    array.append(folderPath);
  }

  QJsonDocument doc(array);

  QFile file(configFile);
  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
    file.close();
    qDebug() << "配置已经保存到：" << configFile;
    qDebug() << "保存的文件夹数量：" << savedFolders.size();
  } else {
    qDebug() << "无法保存配置文件到：" << configFile;
  }
}

void MyPlayList::scrollbarsetup(QAbstractItemView *treewidget) {
  QScrollBar *vertical = treewidget->verticalScrollBar();
  vertical->setStyleSheet(
      "QScrollBar:vertical{ width: 12px; background:transparent; margin:0px;}"
      "QScrollBar::groove:vertical { background: transparent; }"
      "QScrollBar::handle:vertical{background:rgba(235, 88, "
      "88,80);border-radius:5px;min-"
      "height:30px;}"
      "QScrollBar::handle:vertical:hover{background:rgba(235, 88, 88,150);}"
      "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0px;}"
      "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background: "
      "none; }");
  QScrollBar *horizontal = treewidget->horizontalScrollBar();
  horizontal->setStyleSheet(
      "QScrollBar:horizontal{ height: 8px; background:transparent; "
      "margin:0px;}"
      "QScrollBar::groove:horizontal { background: transparent; }"
      "QScrollBar::handle:horizontal{background:rgba(235, 88, "
      "88,80);border-radius:5px;min-"
      "width:30px;}"
      "QScrollBar::handle:horizontal:hover{background:rgba(235, 88, 88,150);}"
      "QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal{height:"
      "0px;}"
      "QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal{"
      "background: "
      "none; }");
}
