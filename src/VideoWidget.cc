#include "VideoWidget.h"

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent), isFullscreen(false), isPlaying(false),
      controlBarVisible(true) {
  mediaPlayer = new QMediaPlayer(this);
  videoWidget = new QVideoWidget(this);

  audioOutput = new QAudioOutput(this);
  mediaPlayer->setVideoOutput(videoWidget);
  mediaPlayer->setAudioOutput(audioOutput);
  setupUI();
  setupConnections();
  createStyleSheet();

  hideControlBarTimer = new QTimer(this);
  hideControlBarTimer->setSingleShot(true);
  hideControlBarTimer->setInterval(3000);

  connect(hideControlBarTimer, &QTimer::timeout, this,
          &VideoWidget::hideControlBar);

  setMouseTracking(true);
  videoWidget->setMouseTracking(true);
}

VideoWidget::~VideoWidget() {}

void VideoWidget::setupUI() {
  this->setContentsMargins(0, 0, 0, 0);
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  videoWidget->setGeometry(this->rect());
  videoWidget->setStyleSheet("background-color: black;");
  videoWidget->lower();

  controlBar = new QWidget(this);
  controlBar->setObjectName("controlBar");
  controlBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout *controlLayout = new QVBoxLayout(controlBar);
  controlLayout->setContentsMargins(10, 5, 10, 5);
  controlLayout->setSpacing(5);

  videoprogress = new yslider(this);
  videoprogress->setObjectName("videoprogress");
  videoprogress->setRange(0, 0);
  controlLayout->addWidget(videoprogress);

  QHBoxLayout *buttonLayout = new QHBoxLayout(controlBar);
  buttonLayout->setContentsMargins(2, 2, 2, 2);
  buttonLayout->setSpacing(5);

  playpauseButton = new QPushButton(this);
  playpauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  playpauseButton->setIconSize(QSize(40, 40));

  stopButton = new QPushButton(this);
  stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  stopButton->setIconSize(QSize(40, 40));

  skipnextButton = new QPushButton(this);
  skipnextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
  skipnextButton->setIconSize(QSize(40, 40));

  skippreviousButton = new QPushButton(this);
  skippreviousButton->setIcon(
      style()->standardIcon(QStyle::SP_MediaSkipBackward));
  skippreviousButton->setIconSize(QSize(40, 40));

  currentTimeLabel = new QLabel("00:00/00:00", this);
  currentTimeLabel->setMinimumWidth(120);

  volumeWidget = new QWidget(this);
  QHBoxLayout *volumeLayout = new QHBoxLayout(this);

  muteButton = new QPushButton(controlBar);
  muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
  muteButton->setIconSize(QSize(40, 40));

  volumeslider = new yslider(controlBar);
  volumeslider->setRange(0, 100);
  volumeslider->setValue(80);
  volumeSwitch(volumeslider->value());

  volumeWidget->setContentsMargins(0, 0, 0, 0);
  volumeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  volumeLayout->addWidget(muteButton);
  volumeLayout->addWidget(volumeslider);

  subtitleTrackComboBox = new QComboBox(this);
  subtitleTrackComboBox->addItem("Default");
  subtitleTrackComboBox->addItem("中文");
  subtitleTrackComboBox->addItem("英文");
  subtitleTrackComboBox->addItem("日文");
  subtitleTrackComboBox->setMinimumWidth(80);

  audioTrackComboBox = new QComboBox(this);
  audioTrackComboBox->addItem("Default");
  audioTrackComboBox->setMinimumWidth(80);

  settingButton = new QPushButton(controlBar);
  settingButton->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
  settingButton->setIconSize(QSize(40, 40));

  minimizeButton = new QPushButton(controlBar);
  minimizeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
  minimizeButton->setIconSize(QSize(40, 40));

  fullscreenButton = new QPushButton(controlBar);
  fullscreenButton->setIcon(
      style()->standardIcon(QStyle::SP_TitleBarMaxButton));
  fullscreenButton->setIconSize(QSize(40, 40));

  buttonLayout->addWidget(playpauseButton);
  buttonLayout->addWidget(stopButton);
  buttonLayout->addWidget(skipnextButton);
  buttonLayout->addWidget(skippreviousButton);
  buttonLayout->addWidget(currentTimeLabel);
  buttonLayout->setStretch(0, 1);
  buttonLayout->addWidget(volumeWidget);
  buttonLayout->addWidget(subtitleTrackComboBox);
  buttonLayout->addWidget(audioTrackComboBox);
  buttonLayout->addWidget(settingButton);
  buttonLayout->addWidget(minimizeButton);
  buttonLayout->addWidget(fullscreenButton);

  controlLayout->addLayout(buttonLayout);

  controlBar->adjustSize();
  updateControlBarPosition();

  controlBar->raise();

  controlBarAnimation = new QPropertyAnimation(controlBar, "geometry");
  controlBarAnimation->setDuration(500);
  controlBarAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void VideoWidget::volumeSwitch(int value) noexcept {

  qreal dB = -60.0 + (value / 100.0) * 60.0;
  qreal linearVolume = std::pow(10.0, dB / 20.0);
  audioOutput->setVolume(linearVolume);
}

void VideoWidget::setupConnections() {}

void VideoWidget::createStyleSheet() {}

void VideoWidget::resizeEvent(QResizeEvent *event) {}

void VideoWidget::updateControlBarPosition() {}

void VideoWidget::setSource(const QUrl url) {}

void VideoWidget::play() {}

void VideoWidget::pause() {}

void VideoWidget::stop() {}

void VideoWidget::OnPlayPauseClicked() {}

void VideoWidget::OnStopClicked() {}

void VideoWidget::OnSkipNextClicked() {}

void VideoWidget::OnSkipPreviousClicked() {}

void VideoWidget::OnPositionChanged(qint64 position) {}

void VideoWidget::OnDurationChanged(qint64 duration) {}

void VideoWidget::OnMuteClicked() {}

void VideoWidget::OnVolumeChanged(int value) {}

void VideoWidget::OnFullscreenClicked() {}

void VideoWidget::OnMinimizeClicked() {}

void VideoWidget::OnAudioTrackClicked(qreal index) {}

void VideoWidget::OnSubtitleTrackClicked(qreal index) {}

void VideoWidget::OnSettingClicked() {}

void VideoWidget::hideControlBar() {}

void VideoWidget::showControlBar() {}

void VideoWidget::mouseMoveEvent(QMouseEvent *event) {}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event) {}

void VideoWidget::keyPressEvent(QKeyEvent *event) {}

void VideoWidget::updatePlayPauseButton() {}

QString VideoWidget::formatTime(qint64 timeMilliSeconds) { return ""; };
