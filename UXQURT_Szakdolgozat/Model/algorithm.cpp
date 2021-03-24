#include "algorithm.h"
#include <QDebug>

Algorithm::Algorithm(Graph *graph)
{
    this->graph = graph;
    start_node_ind = -1;
    chosenAlgo = Algorithms::None;

    timer.setInterval(1000);
}

void Algorithm::selectAlgorithm(Algorithm::Algorithms algo)
{
    chosenAlgo = algo;
}

void Algorithm::selectStartNode(int s_ind)
{
    start_node_ind = s_ind;
}

bool Algorithm::stepAlgorithm()
{
    if (chosenAlgo == None) return false;
    bool ended = false;
    switch (chosenAlgo) {
        case Szelessegi:
        break;

    case Melysegi:

        break;

    case Dijkstra:
        ended = !(distances[u] < INT32_MAX && !queue.isEmpty());
        if (!ended) {
            for(int j=0; j<graph->getAdjNum(u); j++) {

            }
            u = remMin(queue);
            ended = !(distances[u] < INT32_MAX && !queue.isEmpty());
        }
        break;

    case Prim:
        break;
    default:
        break;
    }
    if (ended) {
        timer.stop();
        qDebug() << "algorithm ended";
        emit algorithmEnded();
    }
    return true;
}

void Algorithm::startAlgorithm()
{
    timer.start();
}

void Algorithm::pauseAlgrotithm()
{
    timer.stop();
}

void Algorithm::reset()
{
    start_node_ind = -1;
    chosenAlgo = Algorithms::None;
    for(int i=0; i<graph->getSize(); i++) {
        int id = graph->getId(i);
        emit nodeStateChange(Node::NodeType::None, id);
        for (int j=0; j<graph->getAdjNum(i); j++) {
            emit edgeStateChange(AdjNode::EdgeType::None, id, graph->getId(j));
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
            all_distances.clear();
            all_parents.clear();
            all_queue_states.clear();
            addNamesToStringLists();
            for(int i=0; i<graph->getSize(); i++) {
                distances[i] = INT32_MAX;
                parents[i] = -1;
                emit nodeStateChange(Node::NodeType::None, graph->getId(i));
                for(int j=0; j < graph->getAdjNum(i); j++) {
                    emit edgeStateChange(AdjNode::EdgeType::None, graph->getId(i), graph->getId(j));
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
    emit nodeStateChange(Node::NodeType::ExamineAdj, graph->getId(u));
    qDebug() << "parents: " << parents;
    qDebug() << "distances: " << distances;
    qDebug() << "parents list: " << all_parents;
    qDebug() << "distance list: " << all_distances;
}

int Algorithm::remMin(QVector<int> q)
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
