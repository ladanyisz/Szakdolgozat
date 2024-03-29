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
    enum NodeType { BaseNode, ExaminedNode , ProcessedNode, ExamineAdj, ReachedButNotProcessed };
    enum EdgeType { BaseEdge, NeededEdge, NotNeededEdge, ExaminedEdge, BackEdge, ForwardEdge, CrossEdge };
    enum Algorithms { None, Szelessegi, Melysegi, Dijkstra, Prim};
    enum SignalForStuki { Init, Outer, True, False, First, Last };

    struct AlgorithmState {
        AlgorithmState() {}
        AlgorithmState(QVector<int> d, QVector<int> p, QVector<int> q,
                       int u, int aiiu, QVector<NodeType> n, QVector<QVector<EdgeType>> e,
                       QVector<int>disc, QVector<int>fin, QVector<int>aiiius, int r, int time, int prev_u, bool dfs_v,
                       SignalForStuki s) {
            distances = QVector<int>(d);
            parents = QVector<int>(p);
            queue = QVector<int>(q);
            this->u = u;
            adj_ind_in_u = aiiu;
            nodeTypes = QVector<NodeType>(n);
            edgeTypes = QVector<QVector<EdgeType>>(e);

            discovery_time = QVector<int>(disc);
            finishing_time = QVector<int>(fin);
            adj_ind_in_us = QVector<int>(aiiius);
            this->r = r;
            this->time = time;
            this->prev_u = prev_u;
            this->in_dfs_visit = dfs_v;
            sig = s;
        }
        QVector<int> distances;
        QVector<int> parents;
        QVector<int> queue;
        int u;
        int adj_ind_in_u;
        QVector<NodeType> nodeTypes;
        QVector<QVector<EdgeType>> edgeTypes;

        QVector<int> discovery_time;
        QVector<int> finishing_time;
        QVector<int> adj_ind_in_us;
        int r;
        int time;
        int prev_u;
        bool in_dfs_visit;
        SignalForStuki sig;
    };


    Algorithm(Graph* graph);
    void selectAlgorithm(Algorithms algo);
    void selectStartNode(int s_ind);

    void reset();
    bool stepBackAlgorithm();
    void startAlgorithm();
    void pauseAlgrotithm();
    bool getInitState();
    Algorithms getChosenAlgo();

signals:
    // szükséges gráf típusának jelzése
    void needOnlyNonnegativeEdges();
    void noWeights();
    void needWeights();
    void needsToBeUndirected();
    void needsToBeDirected();
    void needsToBeConnected();

    // algoritmus változásai
    void initReady(int);                // index
    void step_start();
    void algorithmEnded();
    void parentChanged(int, QChar);     // index, name
    void distChanged(int, int);         // index, dist
    void queueChanged(QString);
    void discoveryFinishChanged(int, int, int); // index, discovery, finish time

    void nodeStateChange(NodeType, int id);
    void edgeStateChange(EdgeType, int from_id, int to_id);
    void outerLoop();
    void ifTrue();
    void ifFalse();
    void melysegiVisitFirst();
    void melysegiVisitLast();


public slots:
    bool stepAlgorithm();           // második ciklus magja


private:
    Graph* graph;
    QVector<int> distances;
    QVector<int> parents;           // parent node-ok index-e
    QVector<int> queue;             // graph nodes index
    QVector<int> discovery_time;
    QVector<int> finishing_time;
    int u;                          // vizsgált node indexe a graph nodes-ban
    int adj_ind_in_u;               // u hanyadik szomszédos elemét vizsgáljuk (az u indexű Node listájában)
    int time;
    int r;
    bool in_dfs_visit;
    int prev_u;
    QVector<int> adj_ind_in_us;
    bool init_ready;
    QVector<NodeType> nodeTypes;
    QVector<QVector<EdgeType>> edgeTypes;
    QStack<AlgorithmState> steps;

    Algorithms chosenAlgo;
    int start_node_ind;

    void init();
    void initNode();

    bool stepDijkstra();
    bool stepSzelessegi();
    bool stepPrim();
    bool stepMelysegi();
    void stepMelysegiVisit();

    int remMin(QVector<int>&);       // megadja (és eltávolítja a vektorból) a graph nodes megfelelő indexét, ahol a legkisebb a dest
    void addState();
    QString queueToVectorMin();
    QString queueToVector();

    QTimer* timer;
    SignalForStuki sig;
};

#endif // ALGORITHM_H
