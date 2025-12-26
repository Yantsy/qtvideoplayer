#pragma once
#include "yslider.h"
#include <QAudioOutput>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMediaPlayer>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QVideoWidget>
#include <QWidget>
class VideoWidget : public QWidget {
  Q_OBJECT
public:
  explicit VideoWidget(QWidget *parent = nullptr);
  ~VideoWidget();

  void setSource(QUrl url);
  void play();
  void pause();
  void stop();

  QMediaPlayer *mediaPlayer = nullptr;
  QPushButton *skipnextButton = nullptr, *skippreviousButton = nullptr;

private:
  QVideoWidget *videoWidget = nullptr;
  QAudioOutput *audioOutput = nullptr;
  QWidget *controlBar = nullptr, *volumeWidget = nullptr;
  QPropertyAnimation *controlBarAnimation = nullptr;
  yslider *videoprogress = nullptr, *volumeslider = nullptr;
  QLabel *currentTimeLabel = nullptr;
  QPushButton *playpauseButton = nullptr, *stopButton = nullptr,
              *muteButton = nullptr, *audioTrackButton = nullptr,
              *subtitleTrackButton = nullptr, *settingButton = nullptr,
              *fullscreenButton = nullptr, *minimizeButton = nullptr;
  QComboBox *audioTrackComboBox = nullptr, *subtitleTrackComboBox = nullptr;

  QTimer *hideControlBarTimer = nullptr;

  bool isFullscreen;
  bool isPlaying;
  bool controlBarVisible;

  QString formatTime(qint64 timeMilliSeconds);
  void setupUI();
  void setupConnections();
  void updatePlayPauseButton();
  void updateControlBarPosition();
  void createStyleSheet();
  void volumeSwitch(int volume) noexcept;

private slots:
  void OnPositionChanged(qint64 position);
  void OnDurationChanged(qint64 duration);
  void OnPlayPauseClicked();
  void OnStopClicked();
  void OnSkipPreviousClicked();
  void OnSkipNextClicked();
  void OnMuteClicked();
  void OnVolumeChanged(int volume);
  void OnAudioTrackClicked(qreal index);
  void OnSubtitleTrackClicked(qreal index);
  void OnSettingClicked();
  void OnFullscreenClicked();
  void OnMinimizeClicked();
  void hideControlBar();
  void showControlBar();

protected:
  void resizeEvent(QResizeEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
};