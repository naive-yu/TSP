#ifndef TSPWINDOW_H
#define TSPWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class TSPWindow;
}
QT_END_NAMESPACE

class TSPWindow : public QMainWindow
{
    Q_OBJECT

public:
    TSPWindow(QWidget *parent = nullptr);
    ~TSPWindow();

private:
    Ui::TSPWindow *ui;
};
#endif // TSPWINDOW_H
