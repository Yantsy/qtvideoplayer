#include "home.h"

home::home(QWidget *parent) : QWidget(parent) {
  this->setWindowTitle("Qt Video Player");
  this->setMinimumSize(ww, wh);
  this->setWindowFlags(Qt::WindowMaximizeButtonHint |
                       Qt::WindowCloseButtonHint |
                       Qt::WindowMinimizeButtonHint);

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

  // the console polygon
  console = new polygon(this);
  console->getpara(
      videowid->geometry().x(),
      15 + videowid->geometry().height() + videowid->geometry().y(),
      videowid->geometry().width(), 104, 8, 8, QColor(243, 232, 232));
  console->lower();

  yantsyslider = new yslider(this);
  yantsyslider->setGeometry(videowid->geometry().x() + 5,
                            console->geometry().y() + 80,
                            console->geometry().width() - 10, 10);
  yantsyslider->getpara(8, 8);
  yantsyslider->setgroovecolor(QColor(255, 255, 255));
  yantsyslider->settracecolor(QColor(224, 102, 102));
  yantsyslider->sethandelcolor(QColor(235, 88, 88));

  connect(videoplayer, &QMediaPlayer::durationChanged, yantsyslider,
          &yslider::setMaximum);
  connect(videoplayer, &QMediaPlayer::positionChanged, yantsyslider,
          &yslider::setValue);
  // connect(slider, &QSlider::sliderMoved, videoplayer,
  //&QMediaPlayer::setPosition);
  // add buttons and sliders
  slider = new QSlider(Qt::Horizontal, this);
  slider->setGeometry(videowid->geometry().x() + 5,
                      console->geometry().y() + 18,
                      console->geometry().width() - 10, 5);

  play = buttonset(play, "playpause", console->geometry().x() + 7,
                   console->geometry().y() + 40, 34, 34);

  skiplast = buttonset(skiplast, "skip_previous", console->geometry().x() + 55,
                       console->geometry().y() + 40, 34, 34);
  skipnext = buttonset(skipnext, "skip_next", console->geometry().x() + 103,
                       console->geometry().y() + 40, 34, 34);
  audio = buttonset(audio, "volume", console->geometry().x() + 151,
                    console->geometry().y() + 40, 34, 34);

  volume = new QSlider(Qt::Horizontal, this);
  volume->setGeometry(
      audio->geometry().x() + 35,
      audio->geometry().y() + audio->geometry().height() / 2 - 10, 50, 20);
  volume->setRange(0, 100);
  volume->setValue(50);

  open = buttonset(open, "addfile",
                   console->geometry().x() + console->geometry().width() - 284,
                   console->geometry().y() + 40, 34, 34);
  setting =
      buttonset(setting, "setting",
                console->geometry().x() + console->geometry().width() - 236,
                console->geometry().y() + 40, 34, 34);
  subtitle =
      buttonset(subtitle, "subtitle",
                console->geometry().x() + console->geometry().width() - 188,
                console->geometry().y() + 40, 34, 34);
  sound = buttonset(sound, "sound",
                    console->geometry().x() + console->geometry().width() - 140,
                    console->geometry().y() + 40, 34, 34);
  outview =
      buttonset(outview, "setting",
                console->geometry().x() + console->geometry().width() - 92,
                console->geometry().y() + 40, 34, 34);
  fullscreen =
      buttonset(fullscreen, "fullscreen",
                console->geometry().x() + console->geometry().width() - 44,
                console->geometry().y() + 40, 34, 34);

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
      videowidget->setGeometry(30, 10, 897 + (1 / 3) - 20, 475);
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
      videowidget->setGeometry(30, 10, 897 + (1 / 3) - 20, 475);
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

  this->setWindowTitle(*path);
}

void home::paintEvent(QPaintEvent *event) {
  homepainter = new QPainter(this);
  homepainter->setRenderHint(QPainter::Antialiasing);
  // homepainter->setBrush(QColor(255, 255, 255));
  homepainter->fillRect(this->rect(), QColor(244, 204, 204));
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
