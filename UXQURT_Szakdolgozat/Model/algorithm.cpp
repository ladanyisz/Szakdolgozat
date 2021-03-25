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
            if (nodeTypes.at(prev_ind) == BaseNode) emit nodeStateChange(BaseNode, prev_ind);
            else emit nodeStateChange(ProcessedNode, prev_ind);
            emit edgeStateChange(BaseEdge, u, prev_ind);
        }
        if (!ended) {
            if (adj_ind_in_u == graph->getAdjNum(u)) {        // új u-t választunk
                qDebug() << "current u: " << graph->getName(graph->getId(u));
                adj_ind_in_u = 0;
                emit nodeStateChange(ProcessedNode, u);
                nodeTypes[u] = ProcessedNode;
                u = remMin(queue);
                qDebug() << "new u: " << graph->getName(graph->getId(u));
                emit nodeStateChange(ExamineAdj, u);
                ended = !(distances[u] < INT32_MAX && !queue.isEmpty());

            } else {        // belső ciklusmag
                qDebug() << "check adj: " << graph->getAdjName(u,adj_ind_in_u);
                int adj_index = graph->getAdjIndexInNodes(u,adj_ind_in_u);
                emit nodeStateChange(ExaminedNode, adj_index);
                emit edgeStateChange(ExaminedEdge, u, adj_index);
                int newDist = distances[u] + graph->getAdjWeight(u,adj_ind_in_u);
                if (distances[adj_index] > newDist) {
                    parents[adj_index] = graph->getId(u);
                    distances[adj_index] = newDist;
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
        emit nodeStateChange(ProcessedNode, u);
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
            distances.resize(graph->getSize());
            parents.resize(graph->getSize());
            nodeTypes.resize(graph->getSize());
            all_distances.clear();
            all_parents.clear();
            all_queue_states.clear();
            addNamesToStringLists();
            for(int i=0; i<graph->getSize(); i++) {
                distances[i] = INT32_MAX;
                parents[i] = -1;
                nodeTypes[i] = BaseNode;
                emit nodeStateChange(NodeType::BaseNode, graph->getId(i));
                for(int j=0; j < graph->getAdjNum(i); j++) {
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
    QString dist = "";
    QString parent = "";
    if (all_distances.length() >= 2) {
        QString all_distance = all_distances.at(0);
        all_distances.clear();
        all_distances.append(all_distance);
    }
    if (all_parents.length() >= 2) {
        QString all_parent = all_parents.at(0);
        all_parents.clear();
        all_parents.append(all_parent);
    }

    distances[start_node_ind] = 0;
    for(int i=0; i<graph->getSize(); i++) {
        if (i != start_node_ind) {
            queue.append(i);
            dist += QString(QChar(236));        // infinity (?)
        } else {
            dist += QString::number(0);
        }
        parent += QString(QChar(157));          // empty set (?)
    }
    u = start_node_ind;
    emit nodeStateChange(NodeType::ExamineAdj, graph->getId(u));
    qDebug() << "parents: " << parents;
    qDebug() << "distances: " << distances;
    qDebug() << "parents list: " << all_parents;
    qDebug() << "distance list: " << all_distances;
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

void Algorithm::addNamesToStringLists()
{
    QString dist = "dist;";
    QString parent = "parent;";
    QStringList names = graph->getNames();
    for(int i=0; i<names.length(); i++) {
        dist += names[i];
        parent += names[i];
        if (i != names.length()-1) {
            dist += ",";
            parent += ",";
        }
    }
    all_distances.append(dist);
    all_parents.append(parent);

}
