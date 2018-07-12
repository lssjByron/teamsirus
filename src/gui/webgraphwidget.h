#ifndef WEBGRAPHWIDGET_H
#define WEBGRAPHWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "collabwebgraph.h"

#define NODE_SIZE 65

class WebGraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WebGraphWidget(QWidget *parent = 0);


    void setData(
            QList<QPushButton *> nodeList,
            QList<QPair<int,int>> nRel,
            QList<QPair<QString,int> > nodeRef,
            QList<CollabWebGraph::CollabData> allCollabData,
            QList<int> collaborationTotals,
            QList<QList<QPair<int,int>>> singleNodeRel );
    void passNodeClicked(int nodeNum);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void resetClicked();


public slots:

private:
    QList<QPushButton *> mNodes;
    QList<QPair<int,int>> nodeCoordData;
    QList<QList<QLine>> allNodeEdges;
    QList<QLine> nEdges;
    QList<QPair<int,int>> nRelationships;
    QList<int> collaborationTotals;
    QList<QPair<QString,int> > nodeReferences;
    QList<CollabWebGraph::CollabData> allCollaborationData;
    int nodeNumber=-1;
    QList<QList<QPair<int,int>>> eachNodeRelationships;
};

#endif // WEBGRAPHWIDGET_H
