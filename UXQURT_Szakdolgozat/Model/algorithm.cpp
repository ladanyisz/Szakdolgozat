#include "algorithm.h"

Algorithm::Algorithm(Graph *graph)
{
    this->graph = graph;
}

void Algorithm::selectAlgorithm(Algorithm::Algorithms algo)
{
    chosenAlgo = algo;
    init();
}

void Algorithm::init()
{
    distances.resize(graph->getSize());
    parents.resize(graph->getSize());

    switch (chosenAlgo) {
        case Szelessegi:
        break;

    case Melysegi:

        break;

    case Dijkstra:
        if (!graph->checkAllEdgesNonnegative()) emit needOnlyNonnegativeEdges();
        else {
            for(int i=0; i<graph->getSize(); i++) {
                distances[i] = INT32_MAX;
                parents[i] = nullptr;
            }
        }
        break;

    case Prim:
        break;
    }
}
