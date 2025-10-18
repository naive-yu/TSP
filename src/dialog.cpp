#include "Dialog.h"
#include "Algorithm.h"
#include "ui_Dialog.h"
#include <QPainter>
#include <memory>

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);
}

Dialog::~Dialog() { delete ui; }

void Dialog::paintEvent(QPaintEvent *event) {
  std::vector<double> iter_best(*best_);
  if (!this->isHidden()) {
    // 绘制坐标轴，确定缩放比例
    int size = static_cast<int>(iter_best.size());
    double mini = ALGO_INF;
    double maxi = 0.0;
    for (int i = 0; i < size; i++) {
      iter_best[i] = std::min(iter_best[i], mini);
      maxi = std::max(maxi, (*avg_)[i]);
    }
    maxi = std::max(maxi, iter_best[0]);
    int width = this->width();
    int height = this->height();
    QPainter painter = QPainter(this);
    double y_scale = (height - 100) / (maxi - iter_best[size - 1]);
    double y_start = iter_best[size - 1];
    double x_scale = (width - 100) / (size + 0.0); // max_iter
    paint_axis(&painter, y_scale, y_start, x_scale);

    // 画最短环路距离曲线
    painter.setWindow(0, height, width, -height); // 数学坐标系
    painter.setPen(QPen(Qt::red, 2));
    QPoint start, end;
    start = QPoint(50, round(51 + (iter_best[0] - y_start) * y_scale));
    for (int i = 1; i < size; i++) {
      end = QPoint((int)(i * x_scale) + 50,
                   round(51 + (iter_best[i] - y_start) * y_scale));
      painter.drawLine(start, end);
      start = end;
    }

    // 画平均环路距离曲线
    painter.setPen(QPen(Qt::green, 2));
    start = QPoint(50, round(51 + ((*avg_)[0] - y_start) * y_scale));
    for (int i = 1; i < size; i++) {
      end = QPoint((int)(i * x_scale) + 50,
                   round(51 + ((*avg_)[i] - y_start) * y_scale));
      painter.drawLine(start, end);
      start = end;
    }
    painter.end();
  }
}

void Dialog::init(const std::vector<double> &best,
                  const std::vector<double> &avg) {
  best_ = std::make_shared<const std::vector<double>>(best);
  avg_ = std::make_shared<const std::vector<double>>(avg);
}

void Dialog::paint_axis(QPainter *painter, double y_scale, double y_start,
                        double x_scale) {
  QPoint StartPos = QPoint(50, 50);
  int axisWidth = width() - 100,
      axisHeight = height() - 100; // 600X500的绘图区域
  painter->setPen(QPen(Qt::black, 1));
  painter->setWindow(0, height(), width(), -height()); // 数字坐标系
  QPoint StartPoint;
  QPoint XEndPoint; // x 轴终点
  QPoint YEndPoint; // y 轴终点

  StartPoint.setX(StartPos.x());
  StartPoint.setY(StartPos.y());

  XEndPoint.setX(StartPos.x() + axisWidth + 30);
  XEndPoint.setY(StartPos.y());

  YEndPoint.setX(StartPos.x());
  YEndPoint.setY(StartPos.y() + axisHeight + 30);
  // x，y轴
  painter->drawLine(StartPoint, XEndPoint);
  painter->drawLine(StartPoint, YEndPoint);
  // 绘制剪头
  painter->drawLine(XEndPoint + QPoint(-8, 3), XEndPoint);
  painter->drawLine(XEndPoint + QPoint(-8, -3), XEndPoint);
  painter->drawLine(YEndPoint + QPoint(-3, -8), YEndPoint);
  painter->drawLine(YEndPoint + QPoint(3, -8), YEndPoint);
  // 画刻度
  int deltaX = axisWidth / 10;  // X 轴坐标刻度宽度,600/10
  int deltaY = axisHeight / 10; // Y 轴坐标刻度宽度,500/10
  painter->setPen(QPen(Qt::black, 1));
  for (int i = deltaX; i <= axisWidth; i = i + deltaX) {
    // 横坐标位置每次递增
    // 坐标刻度起始点
    QPoint cal_start;
    cal_start.setX(i + StartPos.x());
    cal_start.setY(StartPos.y());
    // 坐标刻度结束点
    QPoint cali_end;
    cali_end.setX(i + StartPos.x());
    cali_end.setY(StartPos.y() - 4);
    painter->drawLine(cal_start, cali_end);
  }
  for (int i = deltaY; i <= axisHeight; i = i + deltaY) {
    // 坐标刻度起始点
    QPoint cal_start;
    cal_start.setX(StartPos.x()); // x 轴不变，y 轴变
    cal_start.setY(i + StartPos.y());
    // 坐标刻度结束点
    QPoint cali_end;
    cali_end.setX(StartPos.x() - 4);
    cali_end.setY(i + StartPos.y());
    painter->drawLine(cal_start, cali_end);
  }
  // 画刻度值
  painter->setWindow(0, 0, this->width(),
                     this->height()); // 必须恢复原来的坐标系，不然文字会镜像
  for (int i = 0; i <= axisWidth; i = i + deltaX) {
    QString str = QString::number(round(i / x_scale));
    QPoint temp;
    temp.setX(i + 42); // 右边移动的偏移量
    temp.setY(height() + 18 - StartPos.y());
    painter->drawText(temp, str);
  }
  painter->drawText(QPoint(25 + axisWidth / 2, height() - 15), "迭代次数");
  QString str2 = QString::number(round(y_start + axisHeight / y_scale), 'g', 4);
  int mici = 0;
  if (str2.size() > 4) {
    mici = ((QString)str2.back()).toInt();
    painter->drawText(QPoint(58, 26), QString("x10^%1").arg(mici));
  }
  for (int i = 0; i <= axisHeight; i = i + deltaY) {
    QString str = QString::number(round(y_start + i / y_scale), 'g', 4);
    // int cur=((QString)str[-1]).toInt();
    int index = (int)str.indexOf('e');
    int cur = str.mid(index + 1, 3).toInt();
    str = str.mid(0, index);
    if (index == -1) {
      QString result = "0.";
      str.remove(".");
      for (int j = 1; j < mici - str.size(); j++)
        result += "0";
      str = result + str;
    } else if (cur < mici) {
      QString result = "0.";
      str.remove(".");
      for (int j = 1; j < mici - cur; j++)
        result += "0";
      str = result + str;
    }
    str = str.mid(0, 5);
    QPoint temp;
    temp.setX(StartPos.x() - 38); // 左边移动的偏移量
    temp.setY(height() - StartPos.y() - i + 4);
    painter->drawText(temp, str);
  }
}
