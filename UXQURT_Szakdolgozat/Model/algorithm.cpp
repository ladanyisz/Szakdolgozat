#include "algorithm.h"
#include <QDebug>

Algorithm::Algorithm(Graph *graph)
{
    this->graph = graph;
    start_node_ind = -1;
    chosenAlgo = Algorithms::None;
    init_ready = false;
    adj_ind_in_u = 0;
    u = -1;

    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, &Algorithm::stepAlgorithm);
}

void Algorithm::selectAlgorithm(Algorithm::Algorithms algo)
{
    chosenAlgo = algo;
    init_ready = false;
}

void Algorithm::selectStartNode(int s_ind)
{
    start_node_ind = s_ind;
    init_ready = false;
}

bool Algorithm::stepAlgorithm()
{
    if (chosenAlgo == None) return false;
    if (!init_ready) {
        init();
        return true;
    }
    bool ended = false;
    switch (chosenAlgo) {
        case Szelessegi:
        break;

    case Melysegi:

        break;

    case Dijkstra:
        ended = !(distances[u] < INT32_MAX && !queue.isEmpty());
        if (adj_ind_in_u > 0) {                     // az előző lépésben vizsgáltat visszaállítjuk ( ha volt )
            int prev_ind = graph->getAdjIndexInNodes(u,adj_ind_in_u-1);
            int prev_id =  graph->getId(prev_ind);
            emit nodeStateChange(nodeTypes.at(prev_ind), prev_id);
//            emit edgeStateChange(edgeTypes[u][adj_ind_in_u-1], graph->getId(u), prev_id);
            emit edgeStateChange(BaseEdge, graph->getId(u), prev_id);
        }
        if (!ended) {
            if (adj_ind_in_u == graph->getAdjNum(u)) {        // új u-t választunk
                qDebug() << "current u: " << graph->getName(graph->getId(u));
                adj_ind_in_u = 0;
                emit nodeStateChange(ProcessedNode, graph->getId(u));
                nodeTypes[u] = ProcessedNode;
                u = remMin(queue);                  // ehhez a legjobb utat ismerjük
                qDebug() << "new u: " << graph->getName(graph->getId(u));
                emit nodeStateChange(ExamineAdj, graph->getId(u));
                ended = !(distances[u] < INT32_MAX && !queue.isEmpty());

            } else {        // belső ciklusmag
                qDebug() << "check adj: " << graph->getAdjName(u,adj_ind_in_u);
                int adj_index = graph->getAdjIndexInNodes(u,adj_ind_in_u);
                emit nodeStateChange(ExaminedNode, graph->getId(adj_index));
                emit edgeStateChange(ExaminedEdge, graph->getId(u), graph->getId(adj_index));
                int newDist = distances[u] + graph->getAdjWeight(u,adj_ind_in_u);
                if (distances[adj_index] > newDist) {
                    parents[adj_index] = u;
                    distances[adj_index] = newDist;
//                    edgeTypes[u][adj_ind_in_u] = EdgeType::NeededEdge;
//                    emit edgeStateChange(EdgeType::NeededEdge, graph->getId(u), graph->getId(adj_index));
                } else {
//                    edgeTypes[parents[adj_index]][adj_ind_in_u] = EdgeType::NotNeededEdge;
//                    emit edgeStateChange(EdgeType::NotNeededEdge, graph->getId(u), graph->getId(adj_index));
                }
                adj_ind_in_u++;
            }

//            for(int j=0; j<graph->getAdjNum(u); j++) {
//            }

        }
        break;

    case Prim:
        break;
    default:
        break;
    }
    qDebug() << "queue: " << queue;
    qDebug() << "parents: " << parents;
    qDebug() << "distances: " << distances;
    if (ended) {
        timer.stop();
        qDebug() << "algorithm ended";
        emit algorithmEnded();
        emit nodeStateChange(ProcessedNode, graph->getId(u));
        return false;
    }
    return true;
}

bool Algorithm::stepBackAlgorithm()
{
    return true;
}

void Algorithm::startAlgorithm()
{
    if (!init_ready) init();
    timer.start();
}

void Algorithm::pauseAlgrotithm()
{
    timer.stop();
}

void Algorithm::reset()
{
    init_ready = false;
    adj_ind_in_u = 0;
    u = -1;
    for(int i=0; i<graph->getSize(); i++) {
        int id = graph->getId(i);
        emit nodeStateChange(NodeType::BaseNode, id);
        for (int j=0; j<graph->getAdjNum(i); j++) {
            emit edgeStateChange(EdgeType::BaseEdge, id, graph->getId(j));
        }
    }
    timer.stop();
}

void Algorithm::init()
{
    switch (chosenAlgo) {
        case Szelessegi:
        break;

    case Melysegi:

        break;

    case Dijkstra:
        if (!graph->checkAllEdgesNonnegative()) emit needOnlyNonnegativeEdges();
        else {
            queue.clear();
            int n = graph->getSize();
            distances.resize(n);
            parents.resize(n);
            nodeTypes.resize(n);
            edgeTypes.resize(n);
            for(int i=0; i<n; i++) {
                distances[i] = INT32_MAX;
                parents[i] = -1;
                nodeTypes[i] = BaseNode;
                emit nodeStateChange(NodeType::BaseNode, graph->getId(i));
//                edgeTypes[i].resize(graph->getAdjNum(i));
                for(int j=0; j < graph->getAdjNum(i); j++) {
//                    edgeTypes[i][j] = EdgeType::BaseEdge;
                    emit edgeStateChange(EdgeType::BaseEdge, graph->getId(i), graph->getId(j));
                }
            }
        }
        break;

    case Prim:
        break;
    default:
        break;
    }
    if (start_node_ind != -1) initNode();
    init_ready = true;
}

void Algorithm::initNode()
{
    distances[start_node_ind] = 0;
    for(int i=0; i<graph->getSize(); i++) {
        if (i != start_node_ind)
            queue.append(i);
    }
    u = start_node_ind;
    emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));
    qDebug() << "parents: " << parents;
    qDebug() << "distances: " << distances;
}

int Algorithm::remMin(QVector<int> &q)
{
    int min_ind = q.at(0);
    int min_d = INT32_MAX;
    foreach(int index, q) {
        if (distances.at(index) < min_d) {
            min_d = distances.at(index);
            min_ind = index;
        }
    }
    q.removeAll(min_ind);
    return min_ind;
}

