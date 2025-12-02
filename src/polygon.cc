#include "polygon.h"

polygon::polygon(QWidget *parent) : QWidget(parent) {}

polygon::~polygon() {}

void polygon::modelpath(int px, int py, int pw, int ph, int pxr, int pyr,
                        QColor color) {
  this->setGeometry(px, py, pw, ph);
  xr = pxr;
  yr = pyr;

  fillcolor = color;
}

void polygon::paintEvent(QPaintEvent *event) {
  QWidget::paintEvent(event);
  QPainter apainter(this);
  apainter.setRenderHint(QPainter::Antialiasing);
  apainter.setBrush(fillcolor.rgb());
  apainter.setPen(Qt::NoPen);
  apainter.drawRoundedRect(this->rect(), xr, yr);
  apainter.end();
}