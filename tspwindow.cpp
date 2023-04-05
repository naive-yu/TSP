#include "tspwindow.h"
#include "ant_colony.h"
#include "./ui_tspwindow.h"

TSPWindow::TSPWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TSPWindow)
{
    ui->setupUi(this);
}

TSPWindow::~TSPWindow()
{
    delete ui;
}

