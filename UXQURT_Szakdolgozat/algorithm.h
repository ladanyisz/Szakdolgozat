#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include "graph.h"

class Algorithm
{
    Q_OBJECT
public:
    enum Algorithms {Szelessegi, Melysegi, Dijkstra, Prim};

    Algorithm(Graph* graph);
    void selectAlgorithm(Algorithms algo);
    void init();

signals:
    void needOnlyNonnegativeEdges();
    void noWeights();
    void needWeights();
    void needsToBeUndirected();
    void needsToBeDirected();



private:
    Graph* graph;
    QVector<int> distances;
    QVector<Node*> parents;

    Algorithms chosenAlgo;

};

#endif // ALGORITHM_H
