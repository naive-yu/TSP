#ifndef TSPWINDOW_H
#define TSPWINDOW_H

#include <QMainWindow>
#include "Ant_colony.h"
#include "Genetic.h"
#include "resource.h"
#include<iostream>
#include<QTimer>
#include "dialog.h"
#include "Particle.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TSPWindow; }
QT_END_NAMESPACE

class TSPWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TSPWindow(QWidget *parent = nullptr);
    ~TSPWindow() override;

private slots:
    void deal_menu(QAction*);
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
    int city{},index{},max_iter{};
    QTimer *timer{};
};
#endif // TSPWINDOW_H
