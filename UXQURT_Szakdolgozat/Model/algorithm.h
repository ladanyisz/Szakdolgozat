#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QObject>
#include <QStack>
#include <QTimer>
#include <qvector.h>
#include "graph.h"
#include "tuple"

class Algorithm: public QObject
{
    Q_OBJECT

public:
    enum NodeType { BaseNode, ExaminedNode , ProcessedNode, ExamineAdj };
    enum EdgeType { BaseEdge, NeededEdge, NotNeededEdge, ExaminedEdge };
    enum Algorithms { None, Szelessegi, Melysegi, Dijkstra, Prim};

    struct AlgorithmState {
        AlgorithmState() {}
        AlgorithmState(QVector<int> d, QVector<int> p, QVector<int> q, int u, int aiiu, QVector<NodeType> n, QVector<QVector<EdgeType>> e) {
            distances = QVector<int>(d);
            parents = QVector<int>(p);
            queue = QVector<int>(q);
            this->u = u;
            adj_ind_in_u = aiiu;
            nodeTypes = QVector<NodeType>(n);
            edgeTypes = QVector<QVector<EdgeType>>(e);
        }
        QVector<int> distances;
        QVector<int> parents;
        QVector<int> queue;
        int u;
        int adj_ind_in_u;
        QVector<NodeType> nodeTypes;
        QVector<QVector<EdgeType>> edgeTypes;
    };

    Algorithm(Graph* graph);
    void selectAlgorithm(Algorithms algo);
    void selectStartNode(int s_ind);
    void init();
    void reset();
    bool stepBackAlgorithm();
    void startAlgorithm();
    void pauseAlgrotithm();
    bool getInitState();

signals:
    void needOnlyNonnegativeEdges();
    void noWeights();
    void needWeights();
    void needsToBeUndirected();
    void needsToBeDirected();
    void algorithmEnded();

    void nodeStateChange(NodeType, int id);
    void edgeStateChange(EdgeType, int from_id, int to_id);

public slots:
    bool stepAlgorithm();           // második ciklus magja


private:
    Graph* graph;
    QVector<int> distances;
    QVector<int> parents;           // parent node-ok index-e
    QVector<int> queue;             // graph nodes index
    int u;                          // vizsgált node indexe a graph nodes-ban
    int adj_ind_in_u;               // u hanyadik szomszédos elemét vizsgáljuk (az u indexű Node listájában)
    bool init_ready;
    QVector<NodeType> nodeTypes;
    QVector<QVector<EdgeType>> edgeTypes;
    QStack<AlgorithmState> steps;
//    QStack<int> steps;
//    QStack<std::tuple<NodeType, int>> nodeSteps;
//    QStack<std::tuple<EdgeType, int, int>> edgeSteps;

    Algorithms chosenAlgo;
    int start_node_ind;

    void initNode();
    int remMin(QVector<int>&);       // megadja (és eltávolítja a vektorból) a graph nodes megfelelő indexét, ahol a legkisebb a dest
    void addState();

    QTimer* timer;
};

#endif // ALGORITHM_H
