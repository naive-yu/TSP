#pragma once
#include "AlgoExecuter.h"
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
  void show_route(const std::vector<int> &route, int iter, int max_iter);
  void showDialog(const QString &result, const std::vector<double> &best,
                  const std::vector<double> &avg);

private:
  Ui::TSPWindow *ui;
  QTimer *timer;
  int city_{};
  std::shared_ptr<Config> cfg_;
  std::unique_ptr<AlgoExecuter> executer_;
  std::vector<std::vector<int>> position;
  std::vector<std::vector<double>> distance;
  std::shared_ptr<const std::vector<int>> route;
  void reset() noexcept;
  void setInterval(int interval) const;
  void resetTimer() noexcept;
  void paintEvent(QPaintEvent *event) override;
  void computeDistance();
  template<typename T>
  void print(std::vector<T> &nums) const;
};
