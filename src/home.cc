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
  const auto buttony = console->geometry().y() + 45;
  // add buttons and sliders
  /*slider = new QSlider(Qt::Horizontal, this);
  slider->setGeometry(videowid->geometry().x() + 5,
                      console->geometry().y() + 18,
                      console->geometry().width() - 10, 5);*/
  yantsyslider = new yslider(this);
  yantsyslider->setGeometry(videowid->geometry().x() + 5,
                            console->geometry().y() + 18,
                            console->geometry().width() - 10, 10);
  yantsyslider->getpara(8, 8);
  yantsyslider->setgroovecolor(QColor(255, 255, 255));
  yantsyslider->settracecolor(QColor(51, 232, 219));
  yantsyslider->sethandelcolor(QColor(235, 88, 88));

  play = buttonset(play, "playpause", console->geometry().x() + 7, buttony, 34,
                   34);

  skiplast = buttonset(skiplast, "skip_previous", console->geometry().x() + 55,
                       buttony, 34, 34);
  skipnext = buttonset(skipnext, "skip_next", console->geometry().x() + 103,
                       buttony, 34, 34);
  audio = buttonset(audio, "volume", console->geometry().x() + 151, buttony, 34,
                    34);

  /*volume = new QSlider(Qt::Horizontal, this);
  volume->setGeometry(
      audio->geometry().x() + 35,
      audio->geometry().y() + audio->geometry().height() / 2 - 10, 50, 20);
  volume->setRange(0, 100);
  volume->setValue(50);*/

  yvolume = new yslider(this);
  yvolume->setGeometry(
      audio->geometry().x() + 35,
      audio->geometry().y() + audio->geometry().height() / 2 - 5, 50, 10);
  yvolume->getpara(8, 8);
  yvolume->setgroovecolor(QColor(255, 255, 255));
  yvolume->settracecolor(QColor(187, 168, 237));
  yvolume->sethandelcolor(QColor(157, 215, 132));
  yvolume->setRange(0, 100);
  yvolume->setValue(80);
  volumeswicher(yvolume->value());

  open = buttonset(open, "addfile",
                   console->geometry().x() + console->geometry().width() - 284,
                   buttony, 34, 34);
  setting =
      buttonset(setting, "setting",
                console->geometry().x() + console->geometry().width() - 236,
                buttony, 34, 34);
  subtitle =
      buttonset(subtitle, "subtitle",
                console->geometry().x() + console->geometry().width() - 188,
                buttony, 34, 34);
  sound = buttonset(sound, "sound",
                    console->geometry().x() + console->geometry().width() - 140,
                    buttony, 34, 34);
  outview =
      buttonset(outview, "setting",
                console->geometry().x() + console->geometry().width() - 92,
                buttony, 34, 34);
  fullscreen =
      buttonset(fullscreen, "fullscreen",
                console->geometry().x() + console->geometry().width() - 44,
                buttony, 34, 34);

  /*season = new Myscrollarea(this);
  season->move(videowid->geometry().x() + videowid->geometry().width() + 10, 0);
  season->setFixedSize(ww - season->geometry().x(), wh / 2 - 10);

  episode = new Myscrollarea(this);
  episode->move(videowid->geometry().x() + videowid->geometry().width() + 10,
                wh / 2);
  episode->setFixedSize(ww - episode->geometry().x(), wh / 2);*/

  // myscrollarea->setWidgetResizable(true);
  playlist = new MyPlayList(this);
  playlist->setQVideoWidget(videowidget);
  playlist->move(videowid->geometry().x() + videowid->geometry().width() + 10,
                 0);
  playlist->resize(ww - playlist->geometry().x(), wh - 10);

  // signals and slots
  // clicked events
  connect(yvolume, &yslider::valueChanged, this,
          [=](int value) { volumeswicher(value); });
  connect(yvolume, &yslider::sliderMoved, audiooutput,
          &QAudioOutput::setVolume);
  /*connect(videoplayer, &QMediaPlayer::durationChanged, slider,
          &QSlider::setMaximum);
  connect(videoplayer, &QMediaPlayer::positionChanged, slider,
          &QSlider::setValue);
  connect(slider, &QSlider::sliderMoved, videoplayer,
          &QMediaPlayer::setPosition);*/
  connect(videoplayer, &QMediaPlayer::durationChanged, yantsyslider,
          &yslider::setMaximum);

  connect(yantsyslider, &yslider::dragStarted, videoplayer,
          &QMediaPlayer::pause);

  connect(yantsyslider, &yslider::dragFinished, videoplayer, [=]() {
    videoplayer->setPosition(yantsyslider->value());
    videoplayer->play();
  });

  connect(videoplayer, &QMediaPlayer::positionChanged, this, [=](qint64 pos) {
    if (!yantsyslider->isdragging()) {
      yantsyslider->blockSignals(true);
      yantsyslider->setValue(pos);
      yantsyslider->blockSignals(false);
    }
  });
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

void home::volumeswicher(int value) noexcept {
  qreal dB = -60.0 + (value / 100.0) * 60.0;
  qreal linearVolume = std::pow(10.0, dB / 20.0);
  audiooutput->setVolume(linearVolume);
}