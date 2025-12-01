#pragma once
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QVideoWidget>
class videoget : public QWidget {
  Q_OBJECT

public:
  explicit videoget(QWidget *parent = nullptr);
  ~videoget();

private:
  QPainter *roundmaskpainter = nullptr;
  QPainterPath *roundmask = nullptr;
private slots:
protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
};