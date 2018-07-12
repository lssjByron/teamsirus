#include "collabwebgraph.h"
#include "ui_collabwebgraph.h"
#include "math.h"
#include <iterator>
#include <QList>
#include <QPair>
#include <QDebug>
#include <QFileInfo>
#include "mainwindow.h"
#include <QGraphicsDropShadowEffect>
#include <QPointF>
#include <QObject>
#include <qobjectdefs.h>
#include <QStyle>
#include <QDesktopWidget>
#include <QSize>
#include <QRect>
#include <qsize.h>



CollabWebGraph::CollabWebGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollabWebGraph)
{
    ui->setupUi(this);
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius( 5 ); shadowEffect->setOffset(0.0); shadowEffect->setBlurRadius(25.0);
    ui->frame->setGraphicsEffect(shadowEffect);

    //this->setWindowFlags(Qt::Window | Qt:: FramelessWindowHint);

    //this->showMaximized();

    QSize availableSize = qApp->desktop()->availableGeometry().size();
           int width = availableSize.width();
           int height = availableSize.height();
           //width = width*.8;
           //height = height*.8;
           QSize newSize(width,height);


           this->setGeometry(
                  QStyle::alignedRect(
                      Qt::LeftToRight,
                      Qt::AlignCenter,
                      newSize,
                      qApp->desktop()->availableGeometry()
                  )
              );

       // qSize newSize(parentWidget()->width(),parentWidget()->height();
        //this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter, newSize,qApp->desktop()->availableGeometry()));

}

void CollabWebGraph::init(QList<CollabWebGraph::CollabData> allCollabData, QString pathName)
{
    allCollaborationData = allCollabData; publicationPathName=pathName;

    // (1): CREATE OUR nodeReferences (a "reference sheet" of unique author names and their corresponding node number)

   int count=1; QList<QString> init;
    foreach (CollabWebGraph::CollabData collabDataItem, allCollabData)
    {
        nodeReferences.append(qMakePair(collabDataItem.authName,count)); count++;
    }

    // (1.1):
    //CREATE n NODES (n = allCollabData.length (ie. the number of authors that collaborated))
     count = 0;
     QList<int> collaborationTotals; //for tallying up total number of collaborations done by each author
    foreach (CollabWebGraph::CollabData item, allCollabData)
    {

        QPushButton *btn = new QPushButton(QString::number(count+1), ui->widget);       //Create a node for each CollabData item ie. each author (which is technically a button (that can be pressed by user to highlight node's connecting lines))

        if (allCollabData.size()<20)
        {
                //SET STYLE OF NODES
                    btn->setMinimumHeight(NODE_SIZE);
                    btn->setMaximumHeight(NODE_SIZE);
                    btn->setMinimumWidth(NODE_SIZE);
                    btn->setMaximumWidth(NODE_SIZE);
                    //btn->setFlat(true);
                    btn->setCursor(Qt::PointingHandCursor); //btn->setFont();
                    btn->setStyleSheet("font-size: 24pt");
                    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();                                              //Add shadow to button
                    shadowEffect->setBlurRadius( 5 ); shadowEffect->setOffset(0.0); shadowEffect->setBlurRadius(35.0);
                    btn->setGraphicsEffect(shadowEffect);
                    //Get author's name to place in ToolTip label that displays when user hovers cursor over node
                    QString memberName = nodeReferences[count].first;
                    btn->setToolTip(memberName);
                    btn->setToolTipDuration(0);

                //END OF NODE STYLING

                    mNodes.append(btn); count++;
         }


        else if (allCollabData.size()>=20 && allCollabData.size()<40)
        {
            //SET STYLE OF NODES
                btn->setMinimumHeight(NODE_SIZE/2);
                btn->setMaximumHeight(NODE_SIZE/2);
                btn->setMinimumWidth(NODE_SIZE/2);
                btn->setMaximumWidth(NODE_SIZE/2);
                //btn->setFlat(true);
                btn->setCursor(Qt::PointingHandCursor); //btn->setFont();
                btn->setStyleSheet("font-size: 12pt");
                QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();                                              //Add shadow to button
                shadowEffect->setBlurRadius( 5 ); shadowEffect->setOffset(0.0); shadowEffect->setBlurRadius(35.0);
                btn->setGraphicsEffect(shadowEffect);
                //Get author's name to place in ToolTip label that displays when user hovers cursor over node
                QString memberName = nodeReferences[count].first;
                btn->setToolTip(memberName);
                btn->setToolTipDuration(0);

            //END OF NODE STYLING

                mNodes.append(btn); count++;
        }

        else if (allCollabData.size()>=40 && allCollabData.size()<80)
        {
            //SET STYLE OF NODES
                btn->setMinimumHeight(NODE_SIZE/4);
                btn->setMaximumHeight(NODE_SIZE/4);
                btn->setMinimumWidth(NODE_SIZE/4);
                btn->setMaximumWidth(NODE_SIZE/4);
                //btn->setFlat(true);
                btn->setCursor(Qt::PointingHandCursor); //btn->setFont();
                btn->setStyleSheet("font-size: 6pt");
                QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();                                              //Add shadow to button
                shadowEffect->setBlurRadius( 5 ); shadowEffect->setOffset(0.0); shadowEffect->setBlurRadius(35.0);
                btn->setGraphicsEffect(shadowEffect);
                //Get author's name to place in ToolTip label that displays when user hovers cursor over node
                QString memberName = nodeReferences[count].first;
                btn->setToolTip(memberName);
                btn->setToolTipDuration(0);

            //END OF NODE STYLING

                mNodes.append(btn); count++;
        }

        else if (allCollabData.size()>=80 && allCollabData.size()<=160)
        {
            //SET STYLE OF NODES
                btn->setMinimumHeight(NODE_SIZE/8);
                btn->setMaximumHeight(NODE_SIZE/8);
                btn->setMinimumWidth(NODE_SIZE/8);
                btn->setMaximumWidth(NODE_SIZE/8);
                //btn->setFlat(true);
                btn->setCursor(Qt::PointingHandCursor); //btn->setFont();
                btn->setStyleSheet("font-size: 3pt");           //Stylesheet styles
                QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();                                              //Add shadow to button
                shadowEffect->setBlurRadius( 5 ); shadowEffect->setOffset(0.0); shadowEffect->setBlurRadius(35.0);
                btn->setGraphicsEffect(shadowEffect);
                //Get author's name to place in ToolTip label that displays when user hovers cursor over node
                QString memberName = nodeReferences[count].first;
                btn->setToolTip(memberName);
                btn->setToolTipDuration(0);

            //END OF NODE STYLING

                mNodes.append(btn); count++;
        }
        else if (allCollabData.size()>160)
        {
            ui->label_WG_title->setText("The number of collaborations found in this file has exceeded NVIZION's ability to fit all collaboration data on this screen. Unfortunately, NVIZION is unable to display this collaboration web graph for you :(");
        }



       collaborationTotals.append(item.datails.size());


    }

    //CONNECT GENERATED NODE BUTTONS TO THE NODEPRESSED SLOT
    //CALL CONNECT FUNCTION INSIDE A FOREACH LOOP TO CONNECT ALL GENERATED BUTTONS

    foreach(QPushButton* btn, mNodes)
    {
        connect(btn,SIGNAL(clicked(bool)),this,SLOT(nodePressed()));
    }


    //CREATE EDGES (connecting lines between nodes that represent a collaboration between 2 authors)

    // ALGORITHM B.3.a
    // (2): BUILD A UNIQUE LIST OF AUTHOR NAMES THAT HAVE COLLABORATED WITH authName FOR EACH authName (building the 'authNodeConnections' QList of Qlist QStrings.
    bool alreadyExists=false;
    QList<QString> uniqueCollaborators;

    foreach (CollabWebGraph::CollabData collabDataItem, allCollabData)
    {
        uniqueCollaborators.clear();                                                    //Make a temp QList of unique author names that have collab'd with authName to be appended to authNodeConnections. The order of authNodeConnections's QLists is equal to the each author's Node number.
        QList<QPair<QString,QString>> collabDataList = collabDataItem.datails;          //Get the list of all collaborations for 'n' author

        for (int i=0;i<collabDataList.count();++i)                                      //Now, go through all collaborations and add only unique collaborators to "addedAuthCollaborators".
        {
            QString collaborator = collabDataList[i].first;
            alreadyExists=false;                                                        //reset our "does-it-already-exist tracker"
            foreach(QString addedUniqueCollaborator, uniqueCollaborators)
            {
                if(QString::compare(collaborator,addedUniqueCollaborator)==0)           //"0" = they're equal, ie. not unqiue, so don't add it to the authNodeConnections QList because that author already exists in the list of unqiue authors.
                { alreadyExists=true; break; }
            }
            if(!alreadyExists)                                                          //if it doesn't exist, then it's is a unique author name so add it to the authNodeConnections QList
            { uniqueCollaborators.append(collaborator); }
        }
        authNodeConnections.append(uniqueCollaborators);
    }

    /*//CONFIRM RESULTS OF ALGORITHM B.3.a VIA QDEBUG:
    QList<QString> debugList;
    for (int j=0;j<authNodeConnections.length();++j)
    {
        qDebug() << "";
        qDebug() << "M" << j+1 << "node connections:";
        debugList = authNodeConnections[j];

        for(int i=0;i<debugList.length();++i)
        { qDebug() << debugList[i]; }
    }
    *///END OF RESULT CHECKINGALGORITHM B.3.a

    // ALGORITHM B.3.b
    // (3): CHANGE EACH AUTHOR NAME TO ITS CORRESPONDING NODE NUMBER USING nodeReferences (this is done for each unique author name)
    // for each authNodeConnections item, look up matched name in nodeReference and replace with index number
    QList<QString> collaborators; QList<int> authNodeConnectionNumbers;
    for(int i=0;i<authNodeConnections.length();++i)
    {
        authNodeConnectionNumbers.clear();
        collaborators = authNodeConnections[i];
        for(int j=0;j<collaborators.length();++j)
        {
            QString collaboratorName = collaborators[j];
            for(int k=0;k<nodeReferences.length();++k)
            {
                if(QString::compare(collaboratorName,nodeReferences[k].first)==0)        //"0" = a match has been found. Replace "collaborator" with k+1 (its corresponding node number)
                { authNodeConnectionNumbers.append((k+1)); break; }
            }
        }
        authNodeConnectionsNumbers.append(authNodeConnectionNumbers);
        authNodeConnections[i].clear();                                                 //destroy redundant data from memory
    }

    /*//CONFIRM RESULTS OF ALGORITHM B.3.b VIA QDEBUG:
    QList<int> debugList;
    for (int j=0;j<authNodeConnectionsNumbers.length();++j)
    {
        qDebug() << "";
        qDebug() << "(" << j+1 << ")" << "Node connections:";
        qDebug() << "";
        debugList = authNodeConnectionsNumbers[j];

        for(int i=0;i<debugList.length();++i)
        { qDebug() << debugList[i]; }
    }
    *///END OF RESULT CHECKING ALGORITHM B.3.b

    // ALGORITHM F.2.a (continued in webgraphwidget.cpp)
    //  (4): Building set of relationships between nodes
    QList<int> collabNodelist;
    for (int i=0;i<authNodeConnectionsNumbers.length();++i)
    {
         collabNodelist = authNodeConnectionsNumbers[i];
         for(int j=0;j<collabNodelist.length();++j)
         {
            alreadyExists=false;                                                                    //assume new relationship is not in nRelationships until proven otherwise
            for(int k=0;k<nRelationships.length();++k)
            {
                if((nRelationships[k].first==i+1 && nRelationships[k].second==collabNodelist[j]) ||
                   (nRelationships[k].first==collabNodelist[j] && nRelationships[k].second==i+1) ||
                     (collabNodelist[j] == i+1))
                {
                    alreadyExists=true; break;                                                      //this edge has been proven to already exit in nRelationships so do NOT add it
                }
            }
            if(!alreadyExists)                                                                      //evaluates to TRUE if this edge does not exist in nRelationships; so add it
            { nRelationships.append(QPair<int,int>(i+1,collabNodelist[j])); }
         }
    }

//eachNodeRelationships
    //Alg F.2.b
    collabNodelist.clear(); QList<QPair<int,int>> eachNode;
    for (int i=0;i<authNodeConnectionsNumbers.length();++i)
    {
        eachNode.clear();
         collabNodelist = authNodeConnectionsNumbers[i];
         for(int j=0;j<collabNodelist.length();++j)
         {
            eachNode.append(QPair<int,int>(i+1,collabNodelist[j])); //qDebug() << i+1 << "," << collabNodelist[j] ;
         }
        eachNodeRelationships.append(eachNode);
        //qDebug() << "";
    }


   /* debugging Alg F.2.b
    for (int i=0;i<eachNodeRelationships.length();++i)
    {
         qDebug() << ""; qDebug() << "Node:" << i;
        for (int j=0; j<eachNodeRelationships[i].length();++j)
        {
            qDebug() << (eachNodeRelationships[i])[j].first << ","  <<  (eachNodeRelationships[i])[j].second;
        }
    }

/*

     OLD CODE
    QList<QList<QPair<int,int>>> nodeRelationships;
    QList<QPair<int,int>> eachNodeRelationship;
    QPair<int,int> coordinateData;

    QList<int> collabNodelist;
    for (int i=0;i<authNodeConnectionsNumbers.length();++i)
    {
         collabNodelist = authNodeConnectionsNumbers[i];
         for(int j=0;j<collabNodelist.length();++j)
         {
            alreadyExists=false;                                                                    //assume new relationship is not in nRelationships until proven otherwise
            for(int k=0;k<nRelationships.length();++k)
            {
                if((nRelationships[k].first==i+1 && nRelationships[k].second==collabNodelist[j]) ||
                   (nRelationships[k].first==collabNodelist[j] && nRelationships[k].second==i+1) ||
                     (collabNodelist[j] == i+1))
                {
                    alreadyExists=true; break;                                                      //this edge has been proven to already exit in nRelationships so do NOT add it
                }
            }
            if(!alreadyExists)                                                                      //evaluates to TRUE if this edge does not exist in nRelationships; so add it
            { nRelationships.append(QPair<int,int>(i+1,collabNodelist[j])); }
         }
    }
    END OF OLD CODE
    */


   /* //CONFIRM RESULTS OF ALGORITHM F.2 VIA QDEBUG:
    qDebug() << "";qDebug() << "Size of nRelationships:" << nRelationships.size();qDebug() << "";
    for (int j=0;j<nRelationships.length();++j)
    {
        qDebug() << "(" << nRelationships[j].first << "," << nRelationships[j].second << ")";
    }
    *///END OF RESULT CHECKING ALGORITHM F.2


    // CALCULATING TOTAL COLLABORATIONS PARTICIPATED IN FOR EACH AUTHOR
//    QList<int> collaborationTotals;
//    for(int i=0;i<authNodeConnectionsNumbers.length();++i)
//    {

//        collaborationTotals.append(authNodeConnectionsNumbers[i].count());
//        //qDebug() << collaborationTotals[i];
//    }


    //PASS ALL NEEDED DATA TO WEBGRAPHWIDGET
    ui->widget->setData(mNodes,nRelationships,nodeReferences,allCollaborationData,collaborationTotals,eachNodeRelationships);



    for (int i=0; i<nodeReferences.length(); i++)
    {
        QPair<QString,int> node = nodeReferences[i];
        int collCount = collaborationTotals[i];

        QString autherName = node.first;

        int nodeIndex = node.second;

        ui->table_WebGraph_author->insertRow(ui->table_WebGraph_author->rowCount());

        QTableWidgetItem *item1 =  new QTableWidgetItem();
        item1->setData(Qt::EditRole,nodeIndex);
        item1->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *item2 = new QTableWidgetItem(autherName);
        item2->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *item3 = new QTableWidgetItem();
        item3->setData(Qt::EditRole,collCount);
        item3->setTextAlignment(Qt::AlignCenter);

        ui->table_WebGraph_author->setItem(ui->table_WebGraph_author->rowCount()-1,
                                           0, item1);
        ui->table_WebGraph_author->setItem(ui->table_WebGraph_author->rowCount()-1,
                                           1, item2);
        ui->table_WebGraph_author->setItem(ui->table_WebGraph_author->rowCount()-1,
                                           2, item3);


    }

    publicationPathName.replace("  >  ","/");
    QFileInfo publicationFilename(publicationPathName);
    QString currentlyViewingFileName = publicationFilename.fileName();
    currentlyViewingFileName.prepend("Currently Viewing:  ");
    ui->WG_FilePath->setText(currentlyViewingFileName);


}


CollabWebGraph::~CollabWebGraph()
{
    delete ui;
}


//CREATE SINGLE SLOT TO HANLDE ALL NODE-BUTTON PRESSES

void CollabWebGraph::nodePressed()
{
    ui->WG_Button_Reset->setEnabled(true);
    QPushButton* nodeBtn = qobject_cast<QPushButton*>(sender());
    int nodeNum=-1;
    if(nodeBtn != NULL)
    {
       QString nodeNumber = nodeBtn->text();
       nodeNum = nodeNumber.toInt();
       //qDebug() << nodeNum;
    }

    ui->widget->passNodeClicked(nodeNum);

    QString authName = nodeReferences[nodeNum-1].first;
    authName.append(" collaborated with these members on these publications:");

    ui->labal_selectedAuthor->setText(authName);
    ui->label_WG_title->setText((nodeReferences[nodeNum-1].first));

    ui->tableWidget->clear(); //make sure only clears data
    while(ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }

    CollabData selected = allCollaborationData[nodeNum -1];

    QList<QPair<QString, QString> > details = selected.datails;
    for (int i=0; i<details.length(); i++)
    {
        QPair<QString, QString> data = details[i];
        QString authName = data.first;
        QString pubTitle = data.second;

        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QTableWidgetItem *item1 = new QTableWidgetItem(authName);
        QTableWidgetItem *item2 = new QTableWidgetItem(pubTitle);

        ui->tableWidget->setItem((ui->tableWidget->rowCount()-1),0,item1);
        ui->tableWidget->setItem((ui->tableWidget->rowCount()-1),1,item2);
    }
}

void CollabWebGraph::on_pushButton_5_clicked()
{
    this->close();
}

void CollabWebGraph::on_WG_Button_Reset_clicked()
{
    ui->WG_Button_Reset->setEnabled(false);
    ui->widget->resetClicked();
    ui->labal_selectedAuthor->setText("Selected author collaborated with:");
    ui->label_WG_title->setText("");
}

// PDF Exporting

void CollabWebGraph::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),  QDir::homePath(), tr("PDF (*.pdf)"));

    QPdfWriter writer(fileName);

    //Setup each PDF page attributes (creator of PDF, pagae orientation)
    writer.setCreator("NVIZION");
    writer.setPageOrientation(QPageLayout::Portrait);
    //Setup QPainter object that paints the widgets on the pages
    QPainter painter;

    painter.begin(&writer);



    //paint the widgets:
    //First page has the visualization and title:

    //painter.scale(8,8);
    ui->widget->render(&painter);

    painter.scale(0.83, 0.83);
    ui->label_WG_title->render(&painter);

    //Second page has legend (which node represents which author and total collabs for each author
    writer.newPage();
    painter.scale(6, 6);
    ui->table_WebGraph_author->render(&painter);
}

void CollabWebGraph::on_table_WebGraph_author_itemClicked(QTableWidgetItem *item)
{
   int index = ui->table_WebGraph_author->currentColumn();

   int selectedNodeNumber=-1;
    if(index==0)
    {
        selectedNodeNumber=item->text().toInt();
        QPushButton* buttonToPush = mNodes[selectedNodeNumber-1];
        buttonToPush->click();
    }

    QString name;
    if(index==1)
    {
        name = item->text();
        for(int i=0;i<nodeReferences.length();++i)
        {
            if(QString::compare(nodeReferences[i].first,name)==0)
            {
                QPushButton* buttonToPush = mNodes[i];
                buttonToPush->click();
            }
        }
    }

}
