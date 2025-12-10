#pragma once

#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QPalette>
#include <QPixmap>
#include <QScrollArea>

#include "polygon.h"

class Myscrollarea : public QScrollArea {
  Q_OBJECT
public:
  Myscrollarea(QWidget *parent = nullptr);
  ~Myscrollarea();

private:
  QPainterPath *p = nullptr;
  QPixmap *bgp = nullptr;
  polygon *view = nullptr;

protected:
  // void paintEvent(QPaintEvent *event) override;
};