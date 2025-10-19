#include "TSPWindow.h"
#include "AlgoExecuter.h"
#include "Config.h"
#include "Dialog.h"
#include "resource.h"
#include "ui_TSPWindow.h"
#include <QPainter>
#include <cmath>
#include <memory>
#include <vector>

TSPWindow::TSPWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TSPWindow), timer(nullptr) {
  ui->setupUi(this);
  cfg_ = std::make_shared<Config>(QStringLiteral("../config/tsp_config.ini"));
  executer_ = std::make_unique<AlgoExecuter>(cfg_, this);
  connect(executer_.get(), &AlgoExecuter::finished, this,
          &TSPWindow::showDialog);
  connect(executer_.get(), &AlgoExecuter::routeUpdated, this,
          &TSPWindow::show_route);
}

TSPWindow::~TSPWindow() {
  delete ui;
}

void TSPWindow::computeDistance() {
  int n = static_cast<int>(position.size());
  distance.assign(n, std::vector<double>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      distance[i][j] = std::sqrt(std::pow(position[i][0] - position[j][0], 2) +
                                 std::pow(position[i][1] - position[j][1], 2));
    }
  }
}

void TSPWindow::showDialog(const QString &result,
                           const std::vector<double> &best,
                           const std::vector<double> &avg) {
  ui->listWidget->addItem(result);
  auto dialog = new Dialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose, true); // 自动关闭时删除
  dialog->init(best, avg);
  dialog->show();
}

void TSPWindow::reset() noexcept {
  executer_->stop();
}

void TSPWindow::resetTimer() noexcept {
  if (timer != nullptr) {
    timer->stop();
    timer->deleteLater();
    timer = nullptr;
  }
}

void TSPWindow::deal_menu(QAction *action) {
  if (action->objectName() == "action11") {
    reset();
    city_ = 29;
    position = bayg29_position;
    computeDistance();
    route = nullptr;
  } else if (action->objectName() == "action12") {
    reset();
    city_ = 48;
    position = att48_position;
    route = nullptr;
    computeDistance();
  } else if (action->objectName() == "action13") {
    reset();
    city_ = 70;
    position = st70_position;
    route = nullptr;
    computeDistance();
  }

  if (city_ != 0) {
    update();
  }

  if (city_ == 0) {
    ui->statusbar->showMessage("请先选择数据集! ");
  } else if (action->objectName() == "action211") {
    executer_->execute(Ant_Type, city_, position, distance, false);
  } else if (action->objectName() == "action212") {
    executer_->execute(Ant_Type, city_, position, distance, true);
  } else if (action->objectName() == "action221") {
    executer_->execute(Genetic_Type, city_, position, distance, false);
  } else if (action->objectName() == "action222") {
    executer_->execute(Genetic_Type, city_, position, distance, true);
  } else if (action->objectName() == "action231") {
    executer_->execute(Particle_Type, city_, position, distance, false);
  } else if (action->objectName() == "action232") {
    executer_->execute(Particle_Type, city_, position, distance, true);
  }
}

void TSPWindow::show_route(const std::vector<int> &route, int iter) {
  qDebug() << "show_route call: iter " << iter << " city " << route.size();
  this->route = std::make_shared<const std::vector<int>>(route);
  update();
}

void TSPWindow::paintEvent(QPaintEvent *event) {
  // 绘制 QWidget
  int menu_height = ui->menubar->height() + 2;
  QPainter painter(this);
  int width = ui->widget->width() - 4;
  int height = ui->widget->height() - 4;
  int max_x = 0, max_y = 0;
  for (int i = 0; i < city_; i++) {
    max_x = std::max(max_x, position[i][0]);
    max_y = std::max(max_y, position[i][1]);
  }
  double scale_x = width / static_cast<double>(max_x);
  double scale_y = height / static_cast<double>(max_y);
  if (city_ != 0) {
    // 绘制城市位置点
    painter.setPen(QPen(Qt::black, 5));
    painter.setFont(QFont("Arial", 30));
    for (int i = 0; i < city_; i++) {
      painter.drawEllipse(2 + (int)(position[i][0] * scale_x),
                          menu_height + (int)(position[i][1] * scale_y), 2, 2);
    }
  }
  if (route != nullptr && static_cast<int>(route->size()) == city_) {
    // 绘制当前路线
    painter.setPen(QPen(Qt::darkBlue, 1));
    QPoint start, end;
    for (int i = 0; i < city_ - 1; i++) {
      start = QPoint(2 + (int)(position[(*route)[i]][0] * scale_x),
                     menu_height + (int)(position[(*route)[i]][1] * scale_y));
      end = QPoint(2 + (int)(position[(*route)[i + 1]][0] * scale_x),
                   menu_height + (int)(position[(*route)[i + 1]][1] * scale_y));
      painter.drawLine(start, end);
    }
    start =
        QPoint(2 + (int)(position[(*route)[city_ - 1]][0] * scale_x),
               menu_height + (int)(position[(*route)[city_ - 1]][1] * scale_y));
    end = QPoint(2 + (int)(position[(*route)[0]][0] * scale_x),
                 menu_height + (int)(position[(*route)[0]][1] * scale_y));
    painter.drawLine(start, end);
  }
}

template <typename T> 
void TSPWindow::print(std::vector<T> &nums) const {
  QString str;
  for (T &t: nums) {
    str += std::to_string(t);
  }
  qDebug() << str;
}
