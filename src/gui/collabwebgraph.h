#ifndef COLLABWEBGRAPH_H
#define COLLABWEBGRAPH_H

#include <QDialog>
#include <iterator>
#include <QList>
#include <QPair>
#include <QSet>
#include <QTableWidgetItem>




#define NODE_SIZE 65

namespace Ui {
class CollabWebGraph;
}

class CollabWebGraph : public QDialog
{
    Q_OBJECT

public:

    struct CollabData
    {
        QString authName;                                   //Contains author's name that this collab data belongs too
        QList<QPair<QString, QString> > datails;            //Contains list of authors that this author collaborated with and the papers they collab'd on:
    };                                                          //Each first QString = author names that have collab'd with authName
                                                                //Each second QString = name of paper that first QString author and authName collaborated on


    explicit CollabWebGraph(QWidget *parent = 0);

    void init(QList<CollabData> dataList, QString pathName);                  //function used to generate CollabData list

    ~CollabWebGraph();



private slots:
    void on_pushButton_5_clicked();
    void nodePressed();


    void on_WG_Button_Reset_clicked();

    void on_pushButton_2_clicked();

    void on_table_WebGraph_author_itemClicked(QTableWidgetItem *item);

private:
    Ui::CollabWebGraph *ui;
    QList<CollabWebGraph::CollabData> allCollaborationData; //store "tables" of all authors for this webgraph
    QList<QPushButton *> mNodes;                            //store collection of created nodes (that are really buttons)
    QList<QLine> nEdges;                                    //store collection of connecting lines between nodes that represent collabarations between 2 authors
    QList<QList<QPair<int,int>>> eachNodeRelationships;
    QList<QPair<int,int>> nRelationships;                   //(x,y)1 and (x,y)2 coord data for each line
    QList<QList<QString>> authNodeConnections;              //stores the connections to be made between auther names (NOT the xmy co-ord data or node numbers). The position of each QList in the QList represents each auth's node number (i+1)
    QList<QList<int>> authNodeConnectionsNumbers;           //the position of each QList in the QList represents each auth's node number (i+1)
    QList<QPair<QString,int> > nodeReferences;              //stores which author name refers to which node number
    QString publicationPathName;
};

#endif // COLLABWEBGRAPH_H
