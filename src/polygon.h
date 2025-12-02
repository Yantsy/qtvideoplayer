#pragma once
#include <QColor>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

class polygon : public QWidget {
  Q_OBJECT

public:
  polygon(QWidget *parent = nullptr);
  void modelpath(int px, int py, int pw, int ph, int pxr, int pyr,
                 QColor color);
  ~polygon();

private:
  QPainter *apainter = nullptr;
  int w = 0, h = 0, xr = 0, yr = 0, x = 0, y = 0;
  QColor fillcolor;
  QPainterPath *path = nullptr;

protected:
  void paintEvent(QPaintEvent *event) override;
};