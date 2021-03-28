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
            emit edgeStateChange(edgeTypes[u][prev_ind], graph->getId(u), prev_id);
        }
        if (!ended) {
            if (adj_ind_in_u == graph->getAdjNum(u)) {        // új u-t választunk
                adj_ind_in_u = 0;
                emit nodeStateChange(ProcessedNode, graph->getId(u));   // visszafelé lépéshez                    //
                nodeTypes[u] = ProcessedNode;
                u = remMin(queue);                  // ehhez a legjobb utat ismerjük
                emit nodeStateChange(ExamineAdj, graph->getId(u));
                ended = !(distances[u] < INT32_MAX && !queue.isEmpty());

            } else {        // belső ciklusmag
                int adj_index = graph->getAdjIndexInNodes(u,adj_ind_in_u);
                emit nodeStateChange(ExaminedNode, graph->getId(adj_index));
                emit edgeStateChange(ExaminedEdge, graph->getId(u), graph->getId(adj_index));
                int newDist = distances[u] + graph->getAdjWeight(u,adj_ind_in_u);
                if (distances[adj_index] > newDist) {
                    if (parents[adj_index] != -1 && (graph->getDirected() || parents[u] != adj_index)) {
//                        edgeSteps.push(edgeTypes[parents[adj_index]][adj_index]);
                        edgeTypes[parents[adj_index]][adj_index] = EdgeType::NotNeededEdge;
                        if (!graph->getDirected()) {
//                            edgeSteps.push(edgeTypes[parents[adj_index]][adj_index]);
                            edgeTypes[adj_index][parents[adj_index]] = EdgeType::NotNeededEdge;
                        }
                        emit edgeStateChange(EdgeType::NotNeededEdge, graph->getId(parents[adj_index]), graph->getId(adj_index));
                    }
                    parents[adj_index] = u;
                    distances[adj_index] = newDist;
                    edgeTypes[u][adj_index] = EdgeType::NeededEdge;
                    if (!graph->getDirected()) edgeTypes[adj_index][u] = EdgeType::NeededEdge;
                } else if ((graph->getDirected() || parents[u] != adj_index)){            // az újonnan talált él nem jobb az eddiginél, ezért nem kell
                    edgeTypes[u][adj_index] = EdgeType::NotNeededEdge;
                    if (!graph->getDirected()) edgeTypes[adj_index][u] = EdgeType::NotNeededEdge;
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
    qDebug() << "-----ODA----";
    qDebug() << "queue: " << queue;
    qDebug() << "parents: " << parents;
    qDebug() << "distances: " << distances;
    qDebug() << "u: " << u << ", adj_ind_in_u: " << adj_ind_in_u;
    addState();
    if (ended) {
        for(int i=0; i<graph->getSize(); i++) {
            for(int j=0; j<graph->getSize(); j++) {
                if (i != j && edgeTypes[i][j] == EdgeType::BaseEdge) {
                    edgeTypes[i][j] = EdgeType::NotNeededEdge;
                    emit edgeStateChange(EdgeType::NotNeededEdge, graph->getId(i), graph->getId(j));
                }
            }
        }
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
//    qDebug() << "steps: " << steps;
////    qDebug() << "node steps: " << nodeSteps;
//    if (steps.isEmpty()) return false;
//    int i = steps.pop();

//    if (i == 0) {
//        std::tuple<NodeType, int> a = nodeSteps.pop();
//        emit nodeStateChange(std::get<0>(a), std::get<1>(a));
//    } else if (i == 1) {

//    }
    if (steps.isEmpty() || steps.length() == 1) return false;
    steps.pop();            // utolsó állapot, nem kell
    AlgorithmState state = steps.top();
    distances = state.distances;
    parents = state.parents;
    queue = state.queue;
    qDebug() << "-----VISSZA----";
    qDebug() << "dist: " << distances;
    qDebug() << "par: " << parents;
    u = state.u;
    adj_ind_in_u = state.adj_ind_in_u;
    nodeTypes = state.nodeTypes;
    edgeTypes = state.edgeTypes;
    for(int i=0; i<graph->getSize(); i++) {
        emit nodeStateChange(nodeTypes[i], graph->getId(i));
        for(int j=0; j<graph->getAdjNum(i); j++) {
            int adj_in_nodes = graph->getAdjIndexInNodes(i,j);
            emit edgeStateChange(edgeTypes[i][adj_in_nodes], graph->getId(i), graph->getId(adj_in_nodes));
        }
    }
    emit nodeStateChange(NodeType::ExamineAdj, u);
    qDebug() << "u: " << u << ", adj_ind_in_u: " << adj_ind_in_u;
    if ((adj_ind_in_u-1) != -1) {
        int adj_index = graph->getAdjIndexInNodes(u,(adj_ind_in_u-1));
        emit nodeStateChange(NodeType::ExaminedNode, graph->getId(adj_index));
        emit edgeStateChange(EdgeType::ExaminedEdge, graph->getId(u), graph->getId(adj_index));
    }
    if (steps.length() == 1) return false;
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
                int adj_id = graph->getId(graph->getAdjIndexInNodes(i,j));
                emit edgeStateChange(EdgeType::BaseEdge, id, adj_id);
            }
    }
    timer.stop();
}

void Algorithm::init()
{
    steps.clear();

    switch (chosenAlgo) {
        case Szelessegi:
        break;

    case Melysegi:

        break;

    case Dijkstra:
        if (!graph->getWeighted()) emit needWeights();
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
                edgeTypes[i].resize(n);
                for(int j=0; j < graph->getAdjNum(i); j++) {
                    int index_in_nodes = graph->getAdjIndexInNodes(i,j);
                    edgeTypes[i][index_in_nodes] = EdgeType::BaseEdge;
                    emit edgeStateChange(EdgeType::BaseEdge, graph->getId(i), graph->getId(index_in_nodes));
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
    addState();
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

void Algorithm::addState()
{
    AlgorithmState state = AlgorithmState(distances, parents, queue, u, adj_ind_in_u, nodeTypes, edgeTypes);
    steps.push(state);
}

