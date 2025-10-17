#pragma once
#include "Algorithm.h"
#include "Config.h"
#include <QMainWindow>
#include <QTimer>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class TSPWindow;
}
QT_END_NAMESPACE

class TSPWindow : public QMainWindow {
  Q_OBJECT

public:
  TSPWindow(QWidget *parent = nullptr);
  ~TSPWindow();

private slots:
  void deal_menu(QAction *);
  void show_route();

private:
  Ui::TSPWindow *ui;
  QTimer *timer;
  std::unique_ptr<Algorithm> algo;
  int city_{}, idx_{}, max_iter_{};
  std::vector<std::vector<int>> position;
  std::vector<std::vector<double>> distance;
  std::shared_ptr<const std::vector<int>> route;
  std::shared_ptr<Config> cfg_;
  void showDialog();
  void resetTimer() noexcept;
  void paintEvent(QPaintEvent *event) override;
  void computeDistance();
};
