#include "playlist.h"

MyPlayList::MyPlayList(QWidget *parent)
    : QWidget(parent), currentPage(0), itemsPerPage(13) {

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
void MyPlayList::setQMediaPlayer(QMediaPlayer *pplayer) { player = pplayer; }

void MyPlayList::initUI() {
  // Initialize UI components and layout here

  QWidget *centralWidget = new QWidget(this);

  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);

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
  // folderTree->setFixedWidth(346);
  connect(folderTree, &QTreeWidget::itemClicked, this,
          &MyPlayList::onFolderSelected);

  // episode part
  QWidget *episodeWidget = new QWidget(this);
  // episodeWidget->setFixedWidth(357);
  QVBoxLayout *episodeLayout = new QVBoxLayout(episodeWidget);
  episodeLayout->setContentsMargins(0, 0, 0, 0);
  QLabel *listlabel = new QLabel("视频列表", this);
  episodeLayout->addWidget(listlabel);

  videoList = new QListWidget(this);
  // videoList->setFixedWidth(346);
  // videoList->setMinimumHeight(250);

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
