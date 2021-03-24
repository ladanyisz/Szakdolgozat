#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QTimer>
#include <qvector.h>
#include "graph.h"
#include "tuple"

class Algorithm: public QObject
{
    Q_OBJECT

public:
    enum Algorithms {None, Szelessegi, Melysegi, Dijkstra, Prim};

    Algorithm(Graph* graph);
    void selectAlgorithm(Algorithms algo);
    void selectStartNode(int s_ind);
    void init();
    void reset();
    bool stepAlgorithm();
    void startAlgorithm();
    void pauseAlgrotithm();

signals:
    void needOnlyNonnegativeEdges();
    void noWeights();
    void needWeights();
    void needsToBeUndirected();
    void needsToBeDirected();
    void algorithmEnded();

    void nodeStateChange(Node::NodeType, int id);
    void edgeStateChange(AdjNode::EdgeType, int from_id, int to_id);


private:
    Graph* graph;
    QVector<int> distances;
    QVector<int> parents;           // parent node-ok id-i
    QVector<int> queue;             // graph nodes index
    int u;                          // vizsgált node indexe a graph nodes-ban
    QStringList all_distances;
    QStringList all_parents;
    QStringList all_queue_states;

    Algorithms chosenAlgo;
    int start_node_ind;

    void initNode();
    int remMin(QVector<int>);       // megadja (és eltávolítja a vektorból) a graph nodes megfelelő indexét, ahol a legkisebb a dest
    void addNamesToStringLists();

    QTimer timer;
};

#endif // ALGORITHM_H
