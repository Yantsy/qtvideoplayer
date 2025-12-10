#include "scrollarea.h"

Myscrollarea::Myscrollarea(QWidget *parent) : QScrollArea(parent) {

  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  this->setWidgetResizable(true);
  this->setFrameShape(QFrame::NoFrame);

  view = new polygon();
  view->getpara(this->x(), this->y(), this->width(), 780, 0, 0,
                QColor(142, 239, 197));
  view->setMinimumSize(450, 780);

  QLabel *label = new QLabel(QString("Amexican Vaquero"));
  /*bgp = new QPixmap(":/resources/amexicanvaquero.jpg");
  QPalette p;
  p.setBrush(QPalette::Base, QBrush(*bgp));
  view->setPalette(p);*/

  QVBoxLayout *layout = new QVBoxLayout(view);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(label);

  this->setWidget(view);
}

Myscrollarea::~Myscrollarea() {}
