#include "yslider.h"
yslider::yslider(QWidget *parent) : QSlider(parent) {
  this->setOrientation(Qt::Horizontal);
}
void yslider::getpara(int pxr, int pyr) {

  xr = pxr;
  yr = pyr;

  /*groovepath = new QPainterPath();
  groovepath->addRoundedRect(this->rect(), xr, yr);

  tracepath = new QPainterPath();
  tracepath->addRoundedRect(this->rect().x(), this->rect().y() - 2,
                            this->rect().width() * ratio,
                            this->rect().height() - 4, xr, yr);

  handelpath = new QPainterPath();
  handelpath->addRoundedRect(this->rect().x() +
                                 tracepath->boundingRect().width(),
                             this->rect().y() + 2, 6, 12, xr, yr);*/
}

void yslider::setgroovecolor(QColor color) { groovecolor = color; }

void yslider::settracecolor(QColor color) { tracecolor = color; }

void yslider::sethandelcolor(QColor color) { handelcolor = color; }

yslider::~yslider() {}

void yslider::paintEvent(QPaintEvent *event) {
  QPainter groove(this);
  ratio = (double)value() / (double)maximum();

  groove.setRenderHint(QPainter::Antialiasing);
  groove.setPen(Qt::NoPen);
  groove.setBrush(groovecolor.rgb());
  groove.drawRoundedRect(this->rect(), xr, yr);

  groove.setRenderHint(QPainter::Antialiasing);
  groove.setPen(Qt::NoPen);
  groove.setBrush(tracecolor.rgb());
  groove.drawRoundedRect(this->rect().x(), this->rect().y() - 2,
                         width() * ratio, height() - 4, xr, yr);

  groove.setRenderHint(QPainter::Antialiasing);
  groove.setPen(Qt::NoPen);
  groove.setBrush(handelcolor.rgb());

  if (this->rect().x() + width() * ratio < this->rect().width() - 6) {
    groove.drawRoundedRect(this->rect().x() + width() * ratio,
                           this->rect().y() + 2, 6, 12, xr, yr);
  } else {
    groove.drawRoundedRect(this->rect().width() - 6, this->rect().y() + 2, 6,
                           12, xr, yr);
  }

  groove.end();
}
