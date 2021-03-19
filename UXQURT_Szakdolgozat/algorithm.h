#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include "graph.h"

class Algorithm
{
    Q_OBJECT
public:
    Algorithm(Graph* graph);

private:
    Graph* graph;
    QVector<int> distances;
    QVector<Node*> parents;


};

#endif // ALGORITHM_H
