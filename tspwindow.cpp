#include <QPainter>
#include "tspwindow.h"
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


void TSPWindow::on_pushButton_clicked()
{
    ui->pushButton->hide();
    antTsp=ant_colony(48,60,400,1000,1,4,0.2);
    antTsp.init();
    antTsp.run();
    antTsp.output();
}

void TSPWindow::paintEvent(QPaintEvent *event) {
    //绘制
    QPainter painter(ui->widget);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial",30));
    int width=ui->widget->width();
    int height=ui->widget->height();

}

