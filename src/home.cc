#include "home.h"

home::home(QWidget *parent) : QWidget(parent) {
  this->setWindowTitle("Qt Video Player");
  this->setMinimumSize(ww, wh);
  this->setWindowFlags(Qt::WindowMaximizeButtonHint |
                       Qt::WindowCloseButtonHint |
                       Qt::WindowMinimizeButtonHint);
  // load iconfont
  // id = QFontDatabase::addApplicationFont(
  // ":/resources/MaterialIconsRound-Regular.otf");
  // family = new QString(QFontDatabase::applicationFontFamilies(id).at(0));
  // iconfont = new QFont(*family, 20);

  // add mediapalyer and videowidget
  videowid = new videoget(this);
  videoplayer = new QMediaPlayer(this);
  videowidget = new QVideoWidget(this);
  audiooutput = new QAudioOutput(this);
  videowidget->setGeometry(30, 10, 897 + (1 / 3) - 20, 475);
  videowid->setGeometry(17, 9, 897 + (1 / 3) - 20 + 26, 477);
  videowidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);

  videoplayer->setVideoOutput(videowidget);
  videoplayer->setAudioOutput(audiooutput);

  // add buttons and sliders
  slider = new QSlider(Qt::Horizontal, this);
  slider->setGeometry(videowidget->geometry().x(),
                      videowidget->geometry().y() +
                          videowidget->geometry().height(),
                      videowidget->geometry().width() - 80, 10);

  play = buttonset(play, "playpause", 10,
                   videowidget->geometry().y() +
                       videowidget->geometry().height() + 50,
                   30, 20);

  open = buttonset(open, "addfile", 210,
                   videowidget->geometry().y() +
                       videowidget->geometry().height() + 50,
                   30, 20);

  fullscreen = buttonset(fullscreen, "fullscreen", 310,
                         videowidget->geometry().y() +
                             videowidget->geometry().height() + 50,
                         30, 20);

  setting = new QPushButton("Settings", videowidget);
  setting->move(20, 20);
  setting->raise();
  setting->show();

  audio = buttonset(audio, "volume", videowidget->geometry().width() - 60,
                    slider->geometry().y(), 30, 20);

  volume = new QSlider(Qt::Horizontal, this);
  volume->setGeometry(audio->geometry().x() + 30, audio->geometry().y(), 50,
                      10);
  volume->setRange(0, 100);
  volume->setValue(50);

  // signals and slots
  // clicked events
  connect(volume, &QSlider::valueChanged, this, [=](int value) {
    qreal dB = -60.0 + (value / 100.0) * 60.0;
    qreal linearVolume = std::pow(10.0, dB / 20.0);
    audiooutput->setVolume(linearVolume);
  });
  connect(volume, &QSlider::sliderMoved, audiooutput, &QAudioOutput::setVolume);
  connect(videoplayer, &QMediaPlayer::durationChanged, slider,
          &QSlider::setMaximum);
  connect(videoplayer, &QMediaPlayer::positionChanged, slider,
          &QSlider::setValue);
  connect(slider, &QSlider::sliderMoved, videoplayer,
          &QMediaPlayer::setPosition);
  connect(play, &QPushButton::clicked, this, [=]() {
    if (videoplayer->isPlaying() == false) {
      videoplayer->play();
    } else {
      videoplayer->pause();
    }
  });

  connect(open, &QPushButton::clicked, this, &home::openfile);

  // shortcut events
  fulscr_shortcut =
      new QShortcut(QKeySequence("F"), fullscreen, SIGNAL(clicked()));
  fulscr_shortcut_x = new QShortcut(QKeySequence("ESC"), videowidget);
  connect(fullscreen, &QPushButton::clicked, this, [=]() {
    if (!videowidget->isFullScreen()) {
      videowidget->setFullScreen(true);
      // videowidget->show();
      this->hide();
    } else {
      videowidget->setFullScreen(false);
      videowidget->setGeometry(20, 10, 897 + (1 / 3), 475);
      this->show();
    }
  });
  connect(fulscr_shortcut_x, &QShortcut::activated, this, [=]() {
    if (!videowidget->isFullScreen()) {
      videowidget->setFullScreen(true);
      // videowidget->show();
      this->hide();
    } else {
      videowidget->setFullScreen(false);
      videowidget->setGeometry(20, 10, 897 + (1 / 3), 475);
      this->show();
    }
  });
}

home::~home() {}

void home::openfile() {
  path = new QString(QFileDialog::getOpenFileName(this, "open file from ..."));

  if (!path->isEmpty()) {
    videoplayer->setSource(QUrl::fromLocalFile(*path));
    videoplayer->play();
  }
}

void home::paintEvent(QPaintEvent *event) {
  homepainter = new QPainter(this);
  homepainter->setBrush(QColor(41, 134, 204));
  homepainter->drawRect(this->rect());
  homepainter->end();
}

QPushButton *home::buttonset(QPushButton *button, std::string iconpath, int x,
                             int y, int w, int h) {
  button = new QPushButton(this);
  button->setIcon(QIcon(QString(":/resources/") +
                        QString::fromStdString(iconpath) + QString(".png")));
  button->setIconSize(QSize(w, h));
  button->setGeometry(x, y, w + 2, h + 2);
  button->setFlat(true);
  return button;
}
