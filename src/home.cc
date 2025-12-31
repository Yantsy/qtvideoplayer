#include "home.h"

home::home(QWidget *parent) : QWidget(parent) {

  // basic settings
  this->setWindowTitle("Qt Video Player");
  ww = this->width();
  wh = this->height();
  // this->setMinimumSize(ww, wh);
  /*this->setWindowFlags(Qt::WindowMaximizeButtonHint |
                       Qt::WindowCloseButtonHint |
                       Qt::WindowMinimizeButtonHint);*/
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *videolayout = new QVBoxLayout();
  videolayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addLayout(videolayout);

  QVBoxLayout *selectlayout = new QVBoxLayout();
  selectlayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addLayout(selectlayout);

  // add mediapalyer and videowidget
  videowid = new videoget(this);
  videoplayer = new QMediaPlayer(this);
  videowidget = new QVideoWidget(this);
  audiooutput = new QAudioOutput(this);
  // videowidget->setGeometry(30, 10, ww, wh);
  videowidget->move(19, 10);
  videowidget->setMinimumSize(ww - 64, wh - 120);
  /*videowid->setGeometry(videowidget->geometry().x() - 13,
                        videowidget->geometry().y() - 1,
                        videowidget->geometry().width() + 26,
                        videowidget->geometry().height() + 2);*/
  videowid->move(videowidget->geometry().x() - 12,
                 videowidget->geometry().y() - 1);
  videowid->setMinimumSize(videowidget->minimumWidth() + 24,
                           videowidget->minimumHeight() + 2);
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

  /*setting =
      buttonset(setting, "setting",
                console->geometry().x() + console->geometry().width() - 236,
                buttony, 34, 34);*/
  videolayout->addWidget(setting);
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

  // clicked events

  connect(yvolume, &yslider::valueChanged, this,
          [=](int value) { volumeswicher(value); });
  connect(yvolume, &yslider::sliderMoved, audiooutput,
          &QAudioOutput::setVolume);

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

  // shortcut events
  playpause_shortcut =
      new QShortcut(QKeySequence("Space"), play, SIGNAL(clicked()));
  fastforward_shortcut =
      new QShortcut(QKeySequence("Right"), yantsyslider, SLOT(forward()));
  backward_shortcut =
      new QShortcut(QKeySequence("Left"), yantsyslider, SLOT(backward()));
  volumeup_shortcut =
      new QShortcut(QKeySequence("Up"), yvolume, SLOT(forward()));
  volumedown_shortcut =
      new QShortcut(QKeySequence("Down"), yvolume, SLOT(backward()));
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
      videowidget->setGeometry(19, 10, ww - 64, wh - 120);
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
      videowidget->setGeometry(19, 10, ww - 64, wh - 120);
      this->show();
    }
  });
}

home::~home() {}

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