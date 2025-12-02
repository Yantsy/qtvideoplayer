#include "videowidget.h"

videoget::videoget(QWidget *parent) : QWidget(parent) {
  this->setAttribute(Qt::WA_TranslucentBackground);
};

videoget::~videoget() {}

void videoget::paintEvent(QPaintEvent *event) {
  QWidget::paintEvent(event);
  roundmaskpainter = new QPainter(this);
  roundmaskpainter->setRenderHint(QPainter::Antialiasing);

  roundmaskpainter->setBrush(QColor(0, 0, 0));
  roundmaskpainter->setPen(Qt::NoPen);
  roundmaskpainter->drawRoundedRect(this->rect(), 20, 20);

  roundmaskpainter->end();
}

void videoget::resizeEvent(QResizeEvent *event) {

  QWidget::resizeEvent(event);
  roundmask = new QPainterPath();

  roundmask->addRoundedRect(this->rect(), 15, 15);

  this->setMask(roundmask->toFillPolygon().toPolygon());
}

/*void videoget::polygonset(int w, int h, int xr, int yr, QColor color) {
  QRect rect(0, 0, w, h);
  polypainter = new QPainter(this);
  polypainter->setRenderHint(QPainter::Antialiasing);

  polypainter->setBrush(color);
  polypainter->setPen(Qt::NoPen);
  polypainter->drawRoundedRect(rect, xr, yr);

  polypainter->end();
}*/