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
  int cur_algorithm{};
  Ant_colony antTsp;
  Genetic gen;
  Particle particle;
  vector<vector<int>> position;
  vector<int> route;
  void paintEvent(QPaintEvent *event) override;
  int city{}, index{}, max_iter{};
  QTimer *timer{};
};
#endif // TSPWINDOW_H
