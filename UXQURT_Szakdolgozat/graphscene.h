#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QList>
#include "graph.h"
#include "nodegraphics.h"
#include "edgegraphics.h"

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum GraphMode {Move, AddNode, AddEdge, SetWeight, Delete};

    explicit GraphScene(Graph* graph, QObject *parent = nullptr);
    QList<QPointF> originalPositions();                                             // megadja a scene-en szereplő elemek pozícióit
    void updatePositions(QList<QPointF> positions, float w_scale, float h_scale);   // scene elemeinek pozícióját frissíti
    void updateEdges();                                                             // a csúcsok (grafikus elemek) éleinek pozícióit újraszámolja
    void setWeight(int w);


public slots:
    void setMode(GraphMode mode);
    void updateNodes();                                             // a graph csúcsinak száma szerint létrehoz vagy töröl csúcsokat
    void addNewEdge(QString fromName, QString toName, int w);       // két megadott csúcs között létrehoz egy új élt
    void deleteEdge(QString fromName, QString toName);
    void setEdge(QString fromName, QString toName, int w);          // meglévő él súlyát állítja be (a csúcsok alapján keres)


signals:
    void edgeSelected();


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    GraphMode mode;
    Graph* graph;

    NodeGraphics* currFrom;
    EdgeGraphics* currEdge;


    void resetCurrFrom();
    void resetCurrEdge();
    void updateNodePositions(QList<NodeGraphics*> nodes);           // csúcsokat körbe rendezi
    void deleteNode(NodeGraphics* node);

};

#endif // GRAPHSCENE_H
