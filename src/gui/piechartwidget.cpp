#include "gui/piechartwidget.h"
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <math.h>
#include <QRectF>
#define PI 3.14159265

PieChartWidget::PieChartWidget(QWidget *parent) : QWidget(parent)
{

}

void PieChartWidget::setData(std::vector<std::pair<std::string, double>> recordList, QVector<QColor> colors) {
    vRecordList=recordList;
    qvColor = colors;
    repaint();
    update();
}

void PieChartWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPen pen;
    QPen penWhite;
    QPen penBlack;
    QRectF size;
    int margin = 25;

    if(this->width()<=this->height()) {
        size = QRectF(margin, (this->height()-this->width())/2+margin, this->width()-margin*2, this->width()-margin*2);
    } else {
        size = QRectF(margin, margin, this->height()-margin*2, this->height()-margin*2);
    }

    QPointF centerPie=size.center();
    QPointF labelPoint;
    double xOffset;
    double yOffset;

    pen.setColor(Qt::white);
    pen.setWidth(0);
    penBlack.setColor(Qt::black);
    penBlack.setWidth(1);

    double sum = 0.0;
    double startAng = 0.0;
    double angle, endAng, percent;
    for (int i =0;i< (int) vRecordList.size();i++)
    {
        sum+=vRecordList[i].second;
    }
    for (int i =0;i< (int) vRecordList.size();i++)
    {
        painter.setPen(pen);
        percent = vRecordList[i].second / sum;
        angle = percent*360.0;
        endAng=startAng + angle;
        painter.setBrush(qvColor[i]);

        //RYAN -- making the pie chart look more stylized:
        painter.drawPie(size,startAng*16,angle*16);                     //draw pie wedges

//        painter.setBrush(QBrush(QColor(Qt::white),Qt::SolidPattern));
//        QRectF doughnutSize = size.adjusted( -(size.bottomRight().x()-size.bottomLeft().x())*.2,    //dx1
//                                             -(size.topRight().y()-size.topLeft().y())*.2,          //dy1
//                                             -(size.bottomRight().x()-size.bottomLeft().x())*.2,    //dx2
//                                             -(size.topRight().y()-size.topLeft().y())*.2           //dy2
//                                            ); //reduce "size" by 20% to draw center white circle (center of doughnut)
//        painter.drawPie(doughnutSize,startAng*16,angle*16);
//        painter.setBrush(qvColor[i]);                                 //return pen to original wedge color

        xOffset = (size.width()/2)*cos((startAng+(angle/2))*PI/180);
        yOffset = (size.width()/2)*sin((startAng+(angle/2))*PI/180);

        labelPoint.setX(centerPie.x()+xOffset);
        labelPoint.setY(centerPie.y()-yOffset);
        painter.setPen(penBlack);

        /*
        if (vRecordList[i].second<1000){
            painter.drawText(labelPoint.toPoint(), QString::number(vRecordList[i].second) );
        }else if (vRecordList[i].second>1000 && vRecordList[i].second<100000)
        {
            painter.drawText(labelPoint.toPoint(), QString::number(vRecordList[i].second/1000) + "K" );
        }else
        {
            painter.drawText(labelPoint.toPoint(), QString::number(vRecordList[i].second/1000000) + "M" );
        }
        */

        startAng=endAng;
    }

}
