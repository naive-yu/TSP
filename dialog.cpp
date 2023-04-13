#include <QPainter>
#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog(){
    delete ui;
}

void Dialog::paintEvent(QPaintEvent *event) {
    if(!this->isHidden()){
        //绘制坐标轴
        int sz=(int)best->size();
        //cout<<best->size()<<" "<<avg->size()<<endl;
        double mini=1000000;
        for(int i=0;i<sz;i++){
            (*best)[i]=min((*best)[i],mini);
        }
        int width=this->width();
        int height=this->height();
        QPainter painter=QPainter(this);
        double y_scale=((*avg)[0]-(*best)[sz-1])/460;
        double y_start=(*best)[sz-1];
        double x_scale=(width-90)/250.0;//max_iter
        paint_axis(&painter,y_scale,y_start);

        //画最短环路距离
        painter.setPen("red");
        QPoint start,end;
        painter.setWindow(0,height,width,-height);//数学坐标系
        start=QPoint(50,(int)(52+((*best)[0]-(*best)[sz-1])/y_scale));
        for(int i=1;i<sz;i++){
            end=QPoint((int)(i*x_scale)+50,(int)(52+((*best)[i]-(*best)[sz-1])/y_scale));
            painter.drawLine(start,end);
            start=end;
        }

        //画平均环路距离
        painter.setPen("green");
        start=QPoint(50,(int)(52+((*avg)[0]-(*best)[sz-1])/y_scale));
        for(int i=1;i<sz;i++){
            end=QPoint((int)(i*x_scale)+50,(int)(52+((*avg)[i]-(*best)[sz-1])/y_scale));
            painter.drawLine(start,end);
            start=end;
        }
        painter.end();
    }
}

void Dialog::init(vector<double> *a, vector<double> *b) {
    best=a;
    avg=b;
}

void Dialog::paint_axis(QPainter *painter,double y_scale,double y_start) {
    QPoint m_ptStartPos=QPoint(50,50);
    int m_axisWidth = width()-90,m_axisHeight = height()-90;
    QPen pen;
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setWindow(0,height(),width(),-height());
    QPoint axisStartPoint;
    QPoint axisXEndPoint; // x 轴终点
    QPoint axisYEndPoint; // y 轴终点

    axisStartPoint.setX(m_ptStartPos.x());
    axisStartPoint.setY(m_ptStartPos.y());

    axisXEndPoint.setX(m_ptStartPos.x() + m_axisWidth+10);
    axisXEndPoint.setY(m_ptStartPos.y());

    axisYEndPoint.setX(m_ptStartPos.x());
    axisYEndPoint.setY(m_ptStartPos.y() + m_axisHeight+10);

    painter->drawLine(axisStartPoint, axisXEndPoint);
    painter->drawLine(axisStartPoint, axisYEndPoint);


    int deltaX = m_axisWidth / 25;  // X 轴坐标刻度宽度
    int deltaY = m_axisHeight / 16; // Y 轴坐标刻度宽度

    for (int i = m_ptStartPos.x()+ deltaX; i < m_axisWidth + m_ptStartPos.x(); i = i + deltaX )
    {
        // 横坐标位置每次递增

        // 坐标刻度起始点
        QPoint calibrationStartPoint;
        calibrationStartPoint.setX(i);
        calibrationStartPoint.setY(m_ptStartPos.y());

        // 坐标刻度结束点
        QPoint calibrationEndPoint;
        calibrationEndPoint.setX(i);
        calibrationEndPoint.setY(m_ptStartPos.y() - 4);

        painter->drawLine(calibrationStartPoint, calibrationEndPoint);
    }

    for (int i = m_ptStartPos.y() + deltaY; i <= m_axisHeight + m_ptStartPos.y(); i = i + deltaY)
    {
        // 坐标刻度起始点
        QPoint calibrationStartPoint;
        calibrationStartPoint.setX(m_ptStartPos.x()); // x 轴不变，y 轴变
        calibrationStartPoint.setY(i);

        // 坐标刻度结束点
        QPoint calibrationEndPoint;
        calibrationEndPoint.setX(m_ptStartPos.x() - 4);
        calibrationEndPoint.setY(i);

        painter->drawLine(calibrationStartPoint, calibrationEndPoint);
    }


    int axisXValue = 0;
    int axisYValue = 0;

    painter->setWindow(0, 0, this->width(), this->height()); // 必须恢复原来的坐标系，不然文字会镜像

    for (int i = m_ptStartPos.x(); i < m_axisWidth + m_ptStartPos.x(); )
    {
        i = i + deltaX;
        QString strAxisXValue = QString::number(axisXValue);
        QPoint temp;
        temp.setX(i - m_ptStartPos.x() + 13); // 左边移动的偏移量
        temp.setY(height() + 18 - m_ptStartPos.y());
        painter->drawText(temp, strAxisXValue);
        axisXValue = axisXValue + 10;
    }

    for (int i = 0; i <= m_axisHeight; i = i + deltaY)
    {
        //QString strAxisYValue = QString::number(axisYValue);
        QPoint temp;
        temp.setX(m_ptStartPos.x() - 43);         // 左边移动的偏移量
        temp.setY(height() - m_ptStartPos.y() - i + 3);
        painter->drawText(temp, QString::number((int)(y_start+2*axisYValue*y_scale)));
        axisYValue = axisYValue + 16;
    }
}



