#include "playlist.h"

MyPlayList::MyPlayList(QWidget *parent)
    : QWidget(parent), currentPage(0), itemsPerPage(10) {

  configFile = "playlist_config.json";

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
void MyPlayList::setQMediaPlayer(QMediaPlayer *pplayer) { player = pplayer; }

void MyPlayList::initUI() {
  // Initialize UI components and layout here

  QWidget *centralWidget = new QWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  // TOP BUTTONS
  QHBoxLayout *topLayout = new QHBoxLayout();
  addFolderButton = new QPushButton(this);
  addFolderButton->setIcon(QIcon(":/resources/addfile.png"));
  addFolderButton->setIconSize(QSize(34, 34));
  addFolderButton->setFlat(true);
  connect(addFolderButton, &QPushButton::clicked, this, &MyPlayList::addFolder);
  topLayout->addWidget(addFolderButton);
  topLayout->addStretch();
  mainLayout->addLayout(topLayout);

  // splitter
  QSplitter *splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);

  // season part

  folderTree = new QTreeWidget(this);
  folderTree->setHeaderLabel("文件夹列表");
  folderTree->setFixedWidth(380);
  connect(folderTree, &QTreeWidget::itemClicked, this,
          &MyPlayList::onFolderSelected);

  // episode part
  QWidget *episodeWidget = new QWidget(this);
  episodeWidget->setFixedWidth(380);
  QVBoxLayout *episodeLayout = new QVBoxLayout(episodeWidget);
  QLabel *listlabel = new QLabel("视频列表", this);
  episodeLayout->addWidget(listlabel);

  videoList = new QListWidget(this);
  videoList->setFixedWidth(380);
  videoList->setMinimumHeight(250);

  connect(videoList, &QListWidget::itemDoubleClicked, this,
          &MyPlayList::playVideo);
  episodeLayout->addWidget(videoList);

  // pages
  QHBoxLayout *pageLayout = new QHBoxLayout();
  prevPageButton = new QPushButton("<", this);
  nextPageButton = new QPushButton(">", this);
  pageLabel = new QLabel("页码：1/1", this);

  connect(prevPageButton, &QPushButton::clicked, this, &MyPlayList::prevPage);
  connect(nextPageButton, &QPushButton::clicked, this, &MyPlayList::nextPage);

  pageLayout->addWidget(prevPageButton);
  pageLayout->addWidget(pageLabel);
  pageLayout->addWidget(nextPageButton);
  episodeLayout->addLayout(pageLayout);

  splitter->addWidget(folderTree);
  splitter->addWidget(episodeWidget);

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

void MyPlayList::nextPage() {}

void MyPlayList::prevPage() {}

void MyPlayList::loadSavedFolders() {}

void MyPlayList::saveFolders() {}
