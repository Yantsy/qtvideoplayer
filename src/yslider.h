#pragma once
#include <QPainter>
#include <QPainterPath>
#include <QSlider>

class yslider : public QSlider {
  Q_OBJECT
public:
  yslider(QWidget *parent = nullptr);
  void getpara(int pxr, int pyr);
  ~yslider();
  void setgroovecolor(QColor color);
  void settracecolor(QColor color);
  void sethandelcolor(QColor color);

private:
  QColor groovecolor, tracecolor, handelcolor;
  QPainterPath *groovepath = nullptr, *tracepath = nullptr,
               *handelpath = nullptr;
  int w = 0, h = 0, xr = 0, yr = 0, x = 0, y = 0;
  double ratio = 0;

private slots:
  // void valueChanged();

protected:
  void paintEvent(QPaintEvent *event) override;
};