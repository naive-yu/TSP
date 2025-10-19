#include "TSPWindow.h"
#include "AlgoExecuter.h"
#include "Config.h"
#include "Dialog.h"
#include "resource.h"
#include "ui_TSPWindow.h"
#include <QPainter>
#include <QLoggingCategory>
#include <QFile>
#include <QDir>
#include <cassert>
#include <cmath>
#include <memory>
#include <qobjectdefs.h>
#include <string>
#include <vector>

Q_LOGGING_CATEGORY(TSPWindowLog, "TSPWindow")

TSPWindow::TSPWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TSPWindow), timer(nullptr) {
  ui->setupUi(this);

  // 尝试多个候选配置路径，优先级从高到低
  const QStringList candidates = {
      // 部署时常见位置：可执行文件目录下的 config 目录
      QCoreApplication::applicationDirPath() + "/config/tsp_config.ini",
      // 可执行文件目录下直接的配置文件
      QCoreApplication::applicationDirPath() + "/tsp_config.ini",
      // 工程运行时常用的相对路径（开发环境）
      QCoreApplication::applicationDirPath() + "/../config/tsp_config.ini",
      // 源代码位置（开发时 CMake 的源目录）
      QStringLiteral("../config/tsp_config.ini")
  };

  QString chosen;
  for (const QString &p : candidates) {
    QFile f(p);
    if (f.exists()) {
      chosen = QDir::cleanPath(p);
      qCInfo(TSPWindowLog) << "Using config file:" << chosen;
      break;
    }
  }

  if (chosen.isEmpty()) {
    // 若没有找到，降级到默认相对路径（保留原行为），并记录警告
    chosen = QCoreApplication::applicationDirPath() + "/config/tsp_config.ini";
    qCWarning(TSPWindowLog) << "Config file not found in candidates, will try default:" << chosen;
  }

  cfg_ = std::make_shared<Config>(chosen);
  executer_ = std::make_unique<AlgoExecuter>(cfg_, this);
  connect(executer_.get(), &AlgoExecuter::finished, this,
          &TSPWindow::showDialog);
  connect(executer_.get(), &AlgoExecuter::routeUpdated, this,
          &TSPWindow::show_route);
}

TSPWindow::~TSPWindow() { delete ui; }

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

void TSPWindow::reset() noexcept { executer_->stop(); }

void TSPWindow::setInterval(int interval) const {
  assert(interval > 0);
  executer_->setInterval(interval);
}

void TSPWindow::resetTimer() noexcept {
  if (timer != nullptr) {
    timer->stop();
    timer->deleteLater();
    timer = nullptr;
  }
}

void TSPWindow::deal_menu(QAction *action) {
  qCInfo(TSPWindowLog) << "Menu action triggered:" << action->objectName();

  if (action->objectName() == "action11") {
    reset();
    city_ = 29;
    position = bayg29_position;
    computeDistance();
    route = nullptr;
    ui->menu_1->setActiveAction(action);
  } else if (action->objectName() == "action12") {
    reset();
    city_ = 48;
    position = att48_position;
    route = nullptr;
    computeDistance();
    ui->menu_1->setActiveAction(action);
  } else if (action->objectName() == "action13") {
    reset();
    city_ = 70;
    position = st70_position;
    route = nullptr;
    computeDistance();
    ui->menu_1->setActiveAction(action);
  }

  if (city_ != 0) {
    qCInfo(TSPWindowLog) << "City set to" << city_;
    update();
  }

  if (city_ == 0) {
    qCWarning(TSPWindowLog) << "No city selected. Please choose a dataset.";
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

  if (action->objectName() == "action51") {
    ui->menu_5->setActiveAction(action);
    setInterval(200);
  } else if (action->objectName() == "action52") {
    setInterval(50);
    ui->menu_5->setActiveAction(action);
  } else if (action->objectName() == "action53") {
    ui->menu_5->setActiveAction(action);
    setInterval(20);
  }

  if (action->objectName() == "action61") {
    ui->menu_6->setActiveAction(action);
    executer_->resume();
  } else if (action->objectName() == "action62") {
    executer_->pause();
    ui->menu_6->setActiveAction(action);
  }
}

void TSPWindow::show_route(const std::vector<int> &route, int iter,
                           int max_iter) {
  qCInfo(TSPWindowLog) << "show_route: iter " << iter << " city " << route.size();
  QString str = QString("show_route: %1/%2").arg(iter).arg(max_iter);
  ui->statusbar->showMessage(str);
  this->route = std::make_shared<const std::vector<int>>(route);
  update();
}

void TSPWindow::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true); // 开启抗锯齿

  int menu_height = ui->menubar->height() + 3;
  int width = ui->widget->width() - 6;
  int height = ui->widget->height() - 6;

  // 计算缩放比例
  int max_x = 0, max_y = 0;
  for (const auto &pos : position) {
    max_x = std::max(max_x, pos[0]);
    max_y = std::max(max_y, pos[1]);
  }
  double scale_x = width / static_cast<double>(max_x);
  double scale_y = height / static_cast<double>(max_y);

  if (city_ != 0) {
    // 绘制城市位置点
    painter.setPen(QPen(Qt::black, 5)); // 设置点的大小和颜色
    for (const auto &pos : position) {
      painter.drawEllipse(
          QPointF(3 + pos[0] * scale_x, menu_height + pos[1] * scale_y), 4, 4);
    }
  }

  if (route != nullptr && static_cast<int>(route->size()) == city_) {
    // 绘制当前路线
    painter.setPen(QPen(Qt::blue, 2)); // 设置线条颜色和粗细
    QPointF start, end;
    for (size_t i = 0; i < city_ - 1; ++i) {
      start = QPointF(3 + position[(*route)[i]][0] * scale_x,
                      menu_height + position[(*route)[i]][1] * scale_y);
      end = QPointF(3 + position[(*route)[i + 1]][0] * scale_x,
                    menu_height + position[(*route)[i + 1]][1] * scale_y);
      painter.drawLine(start, end);
    }
    // 绘制最后一条线，连接起点和终点
    start = QPointF(3 + position[(*route)[city_ - 1]][0] * scale_x,
                    menu_height + position[(*route)[city_ - 1]][1] * scale_y);
    end = QPointF(3 + position[(*route)[0]][0] * scale_x,
                  menu_height + position[(*route)[0]][1] * scale_y);
    painter.drawLine(start, end);
  }
}

template <typename T> void TSPWindow::print(std::vector<T> &nums) const {
  QString str;
  for (T &t : nums) {
    str += std::to_string(t);
  }
  qCInfo(TSPWindowLog) << str;
}
