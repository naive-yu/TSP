#ifndef TSPWINDOW_H
#define TSPWINDOW_H

#include "Ant_colony.h"
#include "Genetic.h"
#include "Particle.h"
#include <QMainWindow>
#include <QTimer>

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
  Ant_colony ant;
  Genetic gen;
  Particle particle;
  int cur_algorithm{}, city{}, index{}, max_iter{};
  std::vector<std::vector<int>> position;
  std::vector<std::vector<double>> distance;
  std::vector<int> route;
  QTimer *timer{};
  void paintEvent(QPaintEvent *event) override;
  void computeDistance();
};

#endif // TSPWINDOW_H
