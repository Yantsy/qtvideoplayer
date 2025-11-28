#pragma once
#include <QProxyStyle>

class style : public QProxyStyle {
  Q_OBJECT

public:
  style(QProxyStyle *style = nullptr);
  ~style();

private:
  void buttonstyle();
  void sliderstyle();
private slots:
};