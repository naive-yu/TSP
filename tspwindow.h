#ifndef TSPWINDOW_H
#define TSPWINDOW_H

#include <QMainWindow>
#include "ant_colony.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TSPWindow; }
QT_END_NAMESPACE

class TSPWindow : public QMainWindow
{
    Q_OBJECT

public:
    TSPWindow(QWidget *parent = nullptr);
    ~TSPWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::TSPWindow *ui;
    ant_colony antTsp;
    void paintEvent(QPaintEvent *event);
};
#endif // TSPWINDOW_H
