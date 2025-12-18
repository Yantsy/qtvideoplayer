#pragma once
#include <QLabel>
#include <QPainter>
#include <QPainterPath>

class MyLabel : public QLabel {
  Q_OBJECT
public:
  MyLabel(QString ptext, QWidget *parent = nullptr) {
    text = new QString(ptext);
  };
  void setColor(const QColor &pcolor) { color = new QColor(pcolor); };
  void setFont(const QFont &pfont) { font = new QFont(pfont); };
  void setImage(const QPixmap &ppixmap) {
    pixmap = new QPixmap(ppixmap);
    if (colororimage() == 1) {
      this->setPixmap(*pixmap);
    }
    return;
  };
  ~MyLabel() {};

private:
  QString *text = nullptr;
  QColor *color = nullptr;
  QFont *font = nullptr;
  QPixmap *pixmap = nullptr;
  int colororimage() {
    if (color != nullptr)
      return 0;
    if (pixmap != nullptr)
      return 1;
    return -1;
  };

protected:
  void paintEvent(QPaintEvent *event) override {
    QPainter p(this);
    if (colororimage() == 0) {
      p.setPen(Qt::NoPen);
      p.setBrush(*color);
      p.setRenderHint(QPainter::Antialiasing);
      p.drawRoundedRect(this->rect(), 0, 0);

      p.setPen(Qt::white);
      p.setRenderHint(QPainter::Antialiasing);
      p.setBrush(Qt::NoBrush);
      p.drawText(this->rect(), Qt::AlignCenter, *text);
    } else {
      QLabel::paintEvent(event);
    }
  }
};