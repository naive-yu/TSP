#include <QPainter>
#include "tspwindow.h"
#include "./ui_tspwindow.h"

using namespace std;
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


void TSPWindow::deal_menu(QAction *action) {
    if(action->objectName()=="action11"){
        city=48;
        position=att48_position;
        route.clear();
    }
    else if(action->objectName()=="action12"){
        city=70;
        position=st70_position;
        route.clear();
    }
    if(city!=0){
        //绘制点
        update();
    }
    if(city==0)ui->statusbar->showMessage("请先选择数据集！");
    else if(action->objectName()=="action211"){
        if(city==48){
            max_iter=250;
            antTsp=Ant_colony(city, 150, max_iter, 1000, 0.5, 4, 0.1);
        }
        else if(city==70){
            max_iter=500;
            antTsp=Ant_colony(city, 210, max_iter, 100, 0.5, 5, 0.1);
        }
        antTsp.init();
        antTsp.run();
        route=(*antTsp.get_route()).back();
        update();
        ui->listWidget->addItem(antTsp.output());
        auto *dialog=new Dialog();
        dialog->init(antTsp.get_best_aim(),antTsp.get_avg_aim());
        dialog->show();
    }else if(action->objectName()=="action212"){
        index=0;
        max_iter=200;
        cur_algorithm=1;
        if(city==48){
            max_iter=250;
            antTsp=Ant_colony(city, 150, max_iter, 1000, 0.5, 4, 0.1);
        }
        else if(city==70){
            max_iter=500;
            antTsp=Ant_colony(city, 210, max_iter, 10, 0.5, 4, 0.1);
        }
        antTsp.init();
        antTsp.run();
        timer=new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(show_route()));
        timer->start(100);
    }else if(action->objectName()=="action221"){
        max_iter=400;
        gen=Genetic(city, 100, max_iter, 4, 0.1);
        gen.init();
        gen.run();
        route=(*gen.get_route()).back();
        update();
        ui->listWidget->addItem(gen.output());
        auto *dialog=new Dialog();
        dialog->init(gen.get_best_aim(),gen.get_avg_aim());
        dialog->show();
    }else if(action->objectName()=="action222"){
        index=0;
        max_iter=400;
        cur_algorithm=2;
        gen=Genetic(city, 100, max_iter, 4, 0.1);
        gen.init();
        gen.run();
        timer=new QTimer(this);
        connect(timer,SIGNAL(timeout()),this,SLOT(show_route()));
        timer->start(100);
    }
}

void TSPWindow::show_route() {
    //cout<<"show_route"<<endl;
    if(cur_algorithm==1)route=(*antTsp.get_route())[index];
    else if(cur_algorithm==2)route=(*gen.get_route())[index];
    if(index==max_iter-1){
        if(cur_algorithm==1){
            ui->listWidget->addItem(antTsp.output());
            auto *dialog=new Dialog();
            dialog->init(antTsp.get_best_aim(),antTsp.get_avg_aim());
            dialog->show();
        }
        else if(cur_algorithm==2){
            ui->listWidget->addItem(gen.output());
            auto *dialog=new Dialog();
            dialog->init(gen.get_best_aim(),gen.get_avg_aim());
            dialog->show();
        }
        timer->stop();
    }
    ++index;
    update();
}


void TSPWindow::paintEvent(QPaintEvent *event) {
    //绘制
    int menu_height=ui->menubar->height()+2;
    QPainter painter(this);
    int width=ui->widget->width()-4;
    int height=ui->widget->height()-4;
    int max_x=0,max_y=0;
    for(int i=0;i<city;i++){
        max_x=max(max_x,position[i][0]);
        max_y=max(max_y,position[i][1]);
    }
    double scale_x=width/(double)max_x;
    double scale_y=height/(double)max_y;
    if(city!=0){
        //绘制点
        painter.setPen(QPen(Qt::black,5));
        painter.setFont(QFont("Arial",30));
        for(int i=0;i<city;i++){
            painter.drawEllipse(2+(int)(position[i][0]*scale_x),menu_height+(int)(position[i][1]*scale_y),2,2);
        }
    }
    if(!route.empty()){
        //绘制路线
        //cout<<"绘制直线"<<endl;
        painter.setPen(QPen(Qt::darkBlue,1));
        QPoint start,end;
        for(int i=0;i<city-1;i++){
            start=QPoint(2+(int)(position[route[i]][0]*scale_x),menu_height+(int)(position[route[i]][1]*scale_y));
            end=QPoint(2+(int)(position[route[i+1]][0]*scale_x),menu_height+(int)(position[route[i+1]][1]*scale_y));
            painter.drawLine(start,end);
        }
        start=QPoint(2+(int)(position[route[city-1]][0]*scale_x),menu_height+(int)(position[route[city-1]][1]*scale_y));
        end=QPoint(2+(int)(position[route[0]][0]*scale_x),menu_height+(int)(position[route[0]][1]*scale_y));
        painter.drawLine(start,end);
    }
}





