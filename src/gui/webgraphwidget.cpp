#define _USE_MATH_DEFINES
#include <cmath>
#include <QPainter>
#include "webgraphwidget.h"
#include <QList>
#include <QPair>
#include <QDebug>

WebGraphWidget::WebGraphWidget(QWidget *parent) : QWidget(parent)
{

}

void WebGraphWidget::setData(QList<QPushButton *> nodeList,
                             QList<QPair<int,int>> nRel,
                             QList<QPair<QString, int> > nodeRef,
                             QList<CollabWebGraph::CollabData> allCollabData,
                             QList<int> collaborationTotals,
                             QList<QList<QPair<int,int>>> singleNodeRel )
{
    mNodes = nodeList;
    nRelationships = nRel;
    eachNodeRelationships = singleNodeRel;


}

void WebGraphWidget::resetClicked()
{
    nodeNumber=-1;
    passNodeClicked(-1);
    update();
}

void WebGraphWidget::passNodeClicked(int nodeNum)
{
    nodeNumber=nodeNum; update();

    //RESET SIZE OF ALL BUTTONS
    int index = 0;

    if (mNodes.size()<20)
    {
        foreach (QPushButton* otherNodes, mNodes)
        {
            int x= nodeCoordData[index].first;
            int y= nodeCoordData[index].second;
            otherNodes->setChecked(false);
            otherNodes->setGeometry(x-(NODE_SIZE/2),y-(NODE_SIZE/2),NODE_SIZE, NODE_SIZE);
            index++;
        }
    }


    else if (mNodes.size()>=20 && mNodes.size()<40)
    {
        foreach (QPushButton* otherNodes, mNodes)
        {
            int x= nodeCoordData[index].first;
            int y= nodeCoordData[index].second;
            otherNodes->setChecked(false);
            otherNodes->setGeometry(x-(NODE_SIZE/4),y-(NODE_SIZE/4),NODE_SIZE/2, NODE_SIZE/2);
            index++;
        }
    }

    else if (mNodes.size()>=40 && mNodes.size()<80)
    {
        foreach (QPushButton* otherNodes, mNodes)
        {
            int x= nodeCoordData[index].first;
            int y= nodeCoordData[index].second;
            otherNodes->setChecked(false);
            otherNodes->setGeometry(x-(NODE_SIZE/8),y-(NODE_SIZE/8),NODE_SIZE/4, NODE_SIZE/4);
            index++;
        }
    }

    else if (mNodes.size()>=80 && mNodes.size()<=160)
    {
        foreach (QPushButton* otherNodes, mNodes)
        {
            int x= nodeCoordData[index].first;
            int y= nodeCoordData[index].second;
            otherNodes->setChecked(false);
            otherNodes->setGeometry(x-(NODE_SIZE/16),y-(NODE_SIZE/16),NODE_SIZE/8, NODE_SIZE/8);
            index++;
        }
    }

    if(nodeNumber!=-1){
    //Make sure the stylization applied is for a clicked button
    QPushButton* clickedButton = mNodes[nodeNumber-1];
     //int x= nodeCoordData[nodeNumber-1].first;
     //int y= nodeCoordData[nodeNumber-1].second;
     clickedButton->setChecked(true);
    //clickedButton->setGeometry(x-(NODE_SIZE/2),y-(NODE_SIZE/2),NODE_SIZE+50, NODE_SIZE+50);
    }

    update();

} //pass node number that was clicked to draw thick / dotted edges and update widget to display new edges

void WebGraphWidget::resizeEvent(QResizeEvent *e)
{
    nodeCoordData.clear();
    double theta = 360.0 / mNodes.length();
    int count = 0;
    //int w = width();
    int containerWidth = width();
    int containerHeight = height();
    double minDimension = (containerHeight > containerWidth) ? containerWidth : containerHeight;
    double graphRadius = (minDimension/2) - 60;
                                          // ^   This Integer value represents the space between the web graph and the edges of the widget container
    int x; int y;

    // DRAW NODES IN WIDGET GRAPH CONTAINER

    if(!mNodes.isEmpty())
    {
        foreach (QPushButton *btn, mNodes)
        {
            double nodeTheta = theta*count;                                                         //determine angle at which node must be drawn at (starting node is always drawn at 12 o'clock position):
                                                                                                    //Derive our (x,y) co-ord (equations below are for theta = 0 defined @ 12o'clock position:
            x = graphRadius*sin(nodeTheta*M_PI/180) + containerWidth/2;                             //x = r·sin(theta)
            y = (-graphRadius*cos(nodeTheta*M_PI/180)) + containerHeight/2;                         //y = r·cos(theta)

            nodeCoordData.append(QPair<int,int> (x,y));                                             //Save (x,y) coord data for each node. Index in QList = Node Number (to be when connecting lines)




            if (mNodes.size()<20)
            {
                btn->setGeometry(x-(NODE_SIZE/2),y-(NODE_SIZE/2),NODE_SIZE, NODE_SIZE);
            }

            else if (mNodes.size()>=20 && mNodes.size()<40)
            {
                btn->setGeometry(x-(NODE_SIZE/4),y-(NODE_SIZE/4),NODE_SIZE/2, NODE_SIZE/2);
            }

            else if (mNodes.size()>=40 && mNodes.size()<80)
            {
                btn->setGeometry(x-(NODE_SIZE/8),y-(NODE_SIZE/8),NODE_SIZE/4, NODE_SIZE/4);
            }

            else if (mNodes.size()>=80 && mNodes.size()<=160)
            {
                btn->setGeometry(x-(NODE_SIZE/16),y-(NODE_SIZE/16),NODE_SIZE/8, NODE_SIZE/8);
            }


            if(nodeNumber==-1) {
                btn->setCheckable(true);
                btn->setChecked(false);
                btn->show();
            }                                                                         //Draw Nodes


            count++;
        }
    }

    // CREATE EDGES

    QPair<int,int> startingPoint;
    QPair<int,int> endingPoint;
    nEdges.clear();
    for(int i=0;i<nRelationships.length();++i)
    {
        startingPoint = nodeCoordData[nRelationships[i].first-1];
        endingPoint = nodeCoordData[nRelationships[i].second-1];
        nEdges.append(QLine(startingPoint.first,startingPoint.second,endingPoint.first,endingPoint.second));    //Create edge and add to nEdges
    }

    QList<QPair<int,int>> nodeInNum;

    QList<QLine> nodeEdges;

    allNodeEdges.clear();

    nodeEdges.clear();



    // GENERATING X,Y DATA FOR EACH NODE'S CONNECTING LINES -- needed for node button functionality (yes this is duplicating the above data in a way)
    // ie. this is for when user clicks on a node --> that node's lines are bolded / all other nodes fade away (stay dotted and become thinner).
    for (int i=0;i<eachNodeRelationships.length();++i)
    {
       nodeInNum = eachNodeRelationships[i];

       nodeEdges.clear();

       //qDebug() << eachNodeRelationships[i].length();

       for(int j=0;j<nodeInNum.length();++j)
       {
           startingPoint = nodeCoordData[nodeInNum[j].first-1];
           endingPoint = nodeCoordData[nodeInNum[j].second-1];
           nodeEdges.append(QLine(startingPoint.first,startingPoint.second,endingPoint.first,endingPoint.second));

           //qDebug() << startingPoint.first << "," << startingPoint.second << " | " << endingPoint.first << "," << endingPoint.second;

       }

        //qDebug() <<"";

       allNodeEdges.append(nodeEdges);
    }




    update();
//    qDebug() << ""; qDebug() << "Printing nEdges x,y data:"; qDebug() << "";
//    foreach(QLine edge, nEdges)  {qDebug() << "(" << edge.x1() << edge.x2() << "," << edge.y1() << edge.y2() << ")";}

    QWidget::resizeEvent(e);


}



void WebGraphWidget::paintEvent(QPaintEvent *event)
{
    //qDebug() << nodeNumber;
    if(nodeNumber == -1)
    {
        QPainter painter(this); QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(4);
        pen.setColor(QColor(0,56,104));
        painter.setPen(pen);
        foreach(QLine edge, nEdges) { painter.drawLine(edge); }  //DRAW EDGES
    }

    else if(nodeNumber != -1)
    {
        //DRAW UNFOCUSED FADED LINES THAT BRANCH OUT FROM ALL UNCLICKED NODES
        QPainter painter(this); QPen pen;
       // DOTTED STYLE:
        pen.setStyle(Qt::DotLine);
        pen.setWidth(1);
        // END OF DOTTED STYLE
        pen.setColor(QColor(0,56,104));
        painter.setPen(pen);
        foreach(QLine edge, nEdges) { painter.drawLine(edge); }


        //DRAW FOCUSED BOLDED LINES THAT BRANCH OUT FROM THE USER-CLICKED NODE
        // BOLD STYLE:
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(5);
        // END OF BOLD STYLE
        pen.setColor(QColor(0,56,104));
        painter.setPen(pen);

        QList<QLine> boldLines = allNodeEdges[nodeNumber-1];
        foreach(QLine boldline, boldLines)
        {
            painter.drawLine(boldline);
        }
    }

}
