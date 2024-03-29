#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QList>
#include "Model/graph.h"
#include "Model/algorithm.h"
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
    QVector<QPointF> nodePositions();                                               // mentéshez a pozíciók ablak méreteivel vett aránya
    void addNode(int id, QChar name, QPointF pos);



public slots:
    void setMode(GraphMode mode);
    void updateNodes();                                             // a graph csúcsinak száma szerint létrehoz vagy töröl csúcsokat
    void resetNodeLayout();
    void findEdgeNodes(int from, int to, int w, bool is_new);
    void deleteReversed(int from, int to);
    void addNewEdge(QString fromName, QString toName, int w);       // két megadott csúcs között létrehoz egy új élt
    void deleteEdge(QString fromName, QString toName);
    void setEdgeWeight(QString fromName, QString toName, int w);          // meglévő él súlyát állítja be (a csúcsok alapján keres)
    void setDirected(bool d);
    void setWeighted(bool w);

    void changeNodeState(Algorithm::NodeType type, int id);
    void changeEdgeState(Algorithm::EdgeType type, int from_id, int to_id);


signals:
    void edgeSelected();
    void graphDirectedChanged(bool d);
    void graphWeightedChanged(bool w);
    void nodeAdded();
    void allNodesDeleted();
    void nodesChanged();


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
    void addEdge(NodeGraphics* from, NodeGraphics* to, int weight);
    void deleteEdgeGraphics(EdgeGraphics* edge);
};

#endif // GRAPHSCENE_H
