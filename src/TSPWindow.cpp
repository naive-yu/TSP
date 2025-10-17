#include "TSPWindow.h"
#include "Ant_colony.h"
#include "Config.h"
#include "Dialog.h"
#include "Genetic.h"
#include "Particle.h"
#include "resource.h"
#include "ui_TSPWindow.h"
#include <QPainter>
#include <cmath>
#include <memory>
#include <qtimer.h>
#include <vector>

TSPWindow::TSPWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TSPWindow) {
  ui->setupUi(this);
  cfg_ = std::make_shared<Config>("config/tsp_config.ini");
  timer = nullptr;
}

TSPWindow::~TSPWindow() {
  resetTimer();
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

void TSPWindow::showDialog() {
  if (timer != nullptr) {
    timer->stop();
  }
  ui->listWidget->addItem(algo->output());
  auto dialog = new Dialog();
  dialog->init(algo->get_best_aim(), algo->get_avg_aim());
  dialog->show();
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
    city_ = 29;
    position = bayg29_position;
    computeDistance();
    route = nullptr;
  } else if (action->objectName() == "action12") {
    city_ = 48;
    position = att48_position;
    route = nullptr;
    computeDistance();
  } else if (action->objectName() == "action13") {
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
    auto params = cfg_->getAntParams(city_);
    algo = std::make_unique<Ant_colony>(city_, params);
    algo->init(position, distance);
    algo->run();

    route = std::make_shared<std::vector<int>>(algo->get_route().back());
    update();
    showDialog();
  } else if (action->objectName() == "action212") {
    idx_ = 0;
    auto params = cfg_->getAntParams(city_);
    max_iter_ = params.max_iter;
    algo = std::make_unique<Ant_colony>(city_, params);
    algo->init(position, distance);
    algo->run();

    resetTimer();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TSPWindow::show_route);
    timer->start(100);
  } else if (action->objectName() == "action221") {
    auto params = cfg_->getGeneticParams(city_);
    algo = std::make_unique<Genetic>(city_, params);
    algo->init(position, distance);
    algo->run();

    route = std::make_shared<std::vector<int>>(algo->get_route().back());
    update();
    showDialog();
  } else if (action->objectName() == "action222") {
    idx_ = 0;
    auto params = cfg_->getGeneticParams(city_);
    max_iter_ = params.max_iter;
    algo = std::make_unique<Genetic>(city_, params);
    algo->init(position, distance);
    algo->run();

    resetTimer();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TSPWindow::show_route);
    timer->start(100);
  } else if (action->objectName() == "action231") {
    auto parms = cfg_->getParticleParams(city_);
    algo = std::make_unique<Particle>(city_, parms);
    algo->init(position, distance);
    algo->run();

    route = std::make_shared<std::vector<int>>(algo->get_route().back());
    update();
    showDialog();
  } else if (action->objectName() == "action232") {
    idx_ = 0;
    auto params = cfg_->getParticleParams(city_);
    max_iter_ = params.max_iter;
    algo = std::make_unique<Particle>(city_, params);
    algo->init(position, distance);
    algo->run();

    resetTimer();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TSPWindow::show_route);
    timer->start(100);
  }
}

void TSPWindow::show_route() {
  if (algo == nullptr) {
    if (timer != nullptr) {
      timer->stop();
      return;
    }
  }

  const auto &routes = algo->get_route();
  if (routes.empty()) {
    if (timer != nullptr) {
      timer->stop();
    }
    return;
  }

  if (idx_ < 0) {
    idx_ = 0;
  }

  if (idx_ >= max_iter_) {
    showDialog();
    return;
  }

  route = std::make_shared<std::vector<int>>(algo->get_route()[idx_]);
  update();

  ++idx_;
  if (idx_ >= max_iter_) {
    showDialog();
  }
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
  if (route != nullptr) {
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
