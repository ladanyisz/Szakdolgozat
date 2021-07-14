#include <QtTest>

#include "Model/graph.h"
#include "Model/algorithm.h"


class GraphTest : public QObject
{
    Q_OBJECT

public:
    GraphTest();
    ~GraphTest();

private slots:
    void init();
    void cleanup();

    // Graph tesztjei
    void testChangeDirected();
    void testChangeWeighted();
    void testAddNode();
    void testAddNodes();
    void testGetId();
    void testGetName();
    void testGetNames();
    void testAjdNodes();
    void testDeleteNode();
    void testDeleteNodes();
    void testSetEdge();
    void testDeleteEdge();
    void testDeleteAll();
    void testSort();
    void testCheckAllEdgesNonnegative();
    void testCheckConnectivity();

    // Algorithm tesztjei
    void testSelections();
    void testStepAlgorithm();


private:
    Graph* graph;
    Algorithm* algo;

    void buildGraph(bool isDirected, bool isWeighted);
};

GraphTest::GraphTest()
{

}

GraphTest::~GraphTest()
{

}

void GraphTest::init()
{
    graph = new Graph();
    algo = new Algorithm(graph);
}

void GraphTest::cleanup()
{
    delete algo;
    delete graph;
}

// Graph

void GraphTest::testChangeDirected()
{
    bool d = graph->getDirected();
    QCOMPARE(d, graph->getDirected());
    graph->changeDirected(!d);
    QCOMPARE(!d, graph->getDirected());
    graph->changeDirected(d);
    QCOMPARE(d, graph->getDirected());
    graph->changeDirected(d);
    QCOMPARE(d, graph->getDirected());
}

void GraphTest::testChangeWeighted()
{
    bool w = graph->getWeighted();
    QCOMPARE(w, graph->getWeighted());
    graph->changeWeighted(!w);
    QCOMPARE(!w, graph->getWeighted());
    graph->changeWeighted(w);
    QCOMPARE(w, graph->getWeighted());
    graph->changeWeighted(w);
    QCOMPARE(w, graph->getWeighted());
}

void GraphTest::testAddNode()
{
    // maxNodeNum-nál több csúcs nem lehet
    for(int i = 0; i<=Graph::getMaxNodeNum(); i++) {
        QCOMPARE(i, graph->getSize());
        graph->addNode();
    }
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());
    graph->addNode();
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());

    graph->deleteAll();
    QChar a = 'A';
    QCOMPARE(0, graph->getSize());
    for(int i = 0; i<Graph::getMaxNodeNum(); i++) {
        QCOMPARE(i, graph->getSize());
        graph->addNode(a.unicode() + i);
    }
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());
    graph->addNode(a.unicode() + Graph::getMaxNodeNum());
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());


    graph->deleteAll();
    QChar name = 'E';
    graph->addNode(name);
    QCOMPARE(1, graph->getSize());
    QCOMPARE('E', graph->getName(0));
    // két ugyanolyan nevűt nem lehet hozzáadni
    graph->addNode(name);
    QVERIFY('E' != graph->getName(1));
    // csak'A' + max. csúcsszámig mehetnek a nevek
    QChar too_much = QChar(a.unicode() + Graph::getMaxNodeNum());
    graph->addNode(too_much);
    QVERIFY(too_much != graph->getName(2));

}

void GraphTest::testAddNodes()
{
    QCOMPARE(0, graph->getSize());
    graph->addNodes(-1);
    QCOMPARE(0, graph->getSize());
    graph->addNodes(0);
    QCOMPARE(0, graph->getSize());
    graph->addNodes(Graph::getMaxNodeNum() - 2);
    QCOMPARE(Graph::getMaxNodeNum() - 2, graph->getSize());
    graph->addNodes(2);
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());
    graph->addNodes(1);
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());

    graph->deleteAll();
    QCOMPARE(0, graph->getSize());
    graph->addNodes(Graph::getMaxNodeNum() + 2);
    QCOMPARE(Graph::getMaxNodeNum(), graph->getSize());
}

void GraphTest::testGetId()
{
    graph->addNodes(Graph::getMaxNodeNum());
    QCOMPARE(-1, graph->getId(-1));
    QCOMPARE(-1, graph->getId(Graph::getMaxNodeNum()));
    for(int i=0; i<graph->getSize(); i++) {
        QVERIFY(graph->getId(i) != -1);
        QVERIFY(graph->getId(i) != graph->getId(i-1));
    }
}

void GraphTest::testGetName()
{
    graph->addNode();
    QCOMPARE(QChar(), graph->getName(graph->getId(0)-1));
    QCOMPARE(QChar(), graph->getName(graph->getId(0)+1));
    QCOMPARE('A', graph->getName(graph->getId(0)));
}

void GraphTest::testGetNames()
{
    QCOMPARE(QStringList(), graph->getNames());
    QStringList names = {"C", "E"};
    graph->addNode('C');
    graph->addNode('E');
    QCOMPARE(names, graph->getNames());
}

void GraphTest::testAjdNodes()
{
    // getWeight
    QCOMPARE(INT32_MAX, graph->getWeight(0,1));
    graph->addNodes(5);
    graph->setEdge(graph->getId(0), graph->getId(1));
    QCOMPARE(INT32_MAX, graph->getWeight(graph->getId(0), graph->getId(2)));
    QCOMPARE(1, graph->getWeight(graph->getId(0), graph->getId(1)));
    graph->setEdge(graph->getId(1), graph->getId(2), -6);
    QCOMPARE(-6, graph->getWeight(graph->getId(1), graph->getId(2)));
    graph->setEdge(graph->getId(1), graph->getId(2), 15);
    QCOMPARE(15, graph->getWeight(graph->getId(1), graph->getId(2)));

    // getAdjNum
    graph->deleteAll();
    QCOMPARE(-1, graph->getAdjNum(0));
    graph->addNode();
    QCOMPARE(0, graph->getAdjNum(0));
    QCOMPARE(-1, graph->getAdjNum(-1));
    graph->deleteNode(graph->getId(0));
    graph->addNodes(4);
    QVector<int> ids;
    for(int i=0; i<graph->getSize(); i++) {
        ids.append(graph->getId(i));
    }
    graph->setEdge(ids.at(0), ids.at(1), 3);
    graph->setEdge(ids.at(0), ids.at(2));
    graph->setEdge(ids.at(0), ids.at(3));
    graph->setEdge(ids.at(1), ids.at(0));
    QCOMPARE(3, graph->getAdjNum(0));
    QCOMPARE(1, graph->getAdjNum(1));

    // getAdjName
    graph->deleteAll();
    QCOMPARE(QChar(), graph->getAdjName(0,0));
    graph->addNode('A');
    graph->addNode('B');
    graph->setEdge(graph->getId(0), graph->getId(1));
    QCOMPARE(QChar(), graph->getAdjName(0,1));
    QCOMPARE(QChar(), graph->getAdjName(0,-1));
    QCOMPARE(QChar(), graph->getAdjName(-1,0));
    QCOMPARE(QChar(), graph->getAdjName(2,0));
    QCOMPARE('B', graph->getAdjName(0,0));

    // getAdjWeight
    graph->deleteAll();
    QCOMPARE(INT32_MAX, graph->getAdjWeight(0,0));
    graph->addNode();
    graph->addNode();
    graph->setEdge(graph->getId(0), graph->getId(1), 20);
    QCOMPARE(INT32_MAX, graph->getAdjWeight(0,1));
    QCOMPARE(INT32_MAX, graph->getAdjWeight(0,-1));
    QCOMPARE(INT32_MAX, graph->getAdjWeight(-1,0));
    QCOMPARE(INT32_MAX, graph->getAdjWeight(2,0));
    QCOMPARE(20, graph->getAdjWeight(0,0));

    // getAdjIndexInNodes
    graph->deleteAll();
    QCOMPARE(-1, graph->getAdjIndexInNodes(-1,-1));
    graph->addNode();
    graph->addNode();
    graph->setEdge(graph->getId(0), graph->getId(1));
    QCOMPARE(-1, graph->getAdjIndexInNodes(0,-1));
    QCOMPARE(-1, graph->getAdjIndexInNodes(0,1));
    QCOMPARE(-1, graph->getAdjIndexInNodes(-1,0));
    QCOMPARE(-1, graph->getAdjIndexInNodes(3,0));
    QCOMPARE(1, graph->getAdjIndexInNodes(0,0));
}

void GraphTest::testDeleteNode()
{
    // rossz bemenetek
    graph->addNode();
    QCOMPARE(1,graph->getSize());
    graph->deleteNode(-1);
    QCOMPARE(1,graph->getSize());
    graph->deleteNode(graph->getId(1));
    QCOMPARE(1, graph->getSize());
    graph->deleteNode(graph->getId(0));
    QCOMPARE(0, graph->getSize());

    // több csúcs törlése a nulladik pozícióról
    graph->addNodes(5);
    QCOMPARE(5, graph->getSize());
    graph->deleteNode(graph->getId(0));
    graph->deleteNode(graph->getId(0));
    graph->deleteNode(graph->getId(0));
    QCOMPARE(2, graph->getSize());

}

void GraphTest::testDeleteNodes()
{
    graph->deleteNodes(2);
    QCOMPARE(0, graph->getSize());
    graph->addNodes(5);
    graph->deleteNodes(-1);
    QCOMPARE(5, graph->getSize());
    graph->deleteNodes(6);
    QCOMPARE(0, graph->getSize());
    graph->addNodes(5);
    graph->deleteNodes(2);
    QCOMPARE(3, graph->getSize());
    graph->deleteNodes(5);
    QCOMPARE(0, graph->getSize());
}

void GraphTest::testSetEdge()
{
    // rossz bemenetek
    QVERIFY(!graph->setEdge(0,1));
    graph->addNode();
    int id = graph->getId(0);
    QVERIFY(!(graph->setEdge(id, id+1)));

    // hurokél
    QVERIFY(!graph->setEdge(id,id));
    graph->addNode();
    int id2 = graph->getId(1);
    // él hozzáadása
    QVERIFY(graph->setEdge(id, id2));
    // párhuzamos él
    QVERIFY(!graph->setEdge(id, id2));

    // irányított eset
    graph->deleteAll();
    graph->changeDirected(true);
    graph->addNodes(2);
    id = graph->getId(0);
    id2 = graph->getId(1);
    QVERIFY(graph->setEdge(id, id2));
    QVERIFY(graph->setEdge(id2, id));

    // irányítatlan eset
    graph->deleteAll();
    graph->changeDirected(false);
    graph->addNodes(2);
    id = graph->getId(0);
    id2 = graph->getId(1);
    QVERIFY(graph->setEdge(id, id2));
    QVERIFY(!graph->setEdge(id2, id));

    // él hozzáadása név alapján
    graph->deleteAll();
    graph->addNodes(2);
    QChar first = graph->getName(graph->getId(0));
    QChar second = graph->getName(graph->getId(1));
    QVERIFY(!graph->setEdge(first, first));
    QVERIFY(graph->setEdge(first, second));

    // élsúly módosítása
    graph->deleteAll();
    graph->addNodes(2);
    QVERIFY(graph->setEdge(graph->getId(0), graph->getId(1), 2));
    QCOMPARE(2, graph->getWeight(graph->getId(0), graph->getId(1)));
    QVERIFY(!graph->setEdge(graph->getId(0), graph->getId(1), 4));
    QCOMPARE(4, graph->getWeight(graph->getId(0), graph->getId(1)));
}

void GraphTest::testDeleteEdge()
{
    // rossz bemenet
    graph->deleteEdge(0,1);
    graph->deleteEdge('A', 'B');
    QCOMPARE(0, graph->getSize());
    graph->addNodes(3);
    graph->deleteEdge(graph->getId(0), graph->getId(1));
    QCOMPARE(3, graph->getSize());

    // irányított eset
    graph->changeDirected(true);
    graph->setEdge(graph->getId(0), graph->getId(1));
    graph->deleteEdge(graph->getId(0), graph->getId(1));
    QCOMPARE(INT32_MAX, graph->getWeight(graph->getId(0), graph->getId(1)));
    QVERIFY(graph->setEdge(graph->getId(0), graph->getId(1)));

    // irányítatlan eset
    graph->changeDirected(false);
    graph->deleteAll();
    graph->addNodes(2);
    graph->setEdge(graph->getId(0), graph->getId(1));
    graph->deleteEdge(graph->getId(0), graph->getId(1));
    QCOMPARE(INT32_MAX, graph->getWeight(graph->getId(0), graph->getId(1)));
    QCOMPARE(INT32_MAX, graph->getWeight(graph->getId(1), graph->getId(0)));

    // nevekkel
    graph->deleteAll();
    graph->addNodes(2);
    graph->setEdge(graph->getId(0), graph->getId(1));
    QChar n1 = graph->getName(graph->getId(0));
    QChar n2 = graph->getName(graph->getId(1));
    graph->deleteEdge(n1, n2);
    QCOMPARE(INT32_MAX, graph->getWeight(graph->getId(0), graph->getId(1)));
}

void GraphTest::testDeleteAll()
{
    graph->deleteAll();
    QCOMPARE(0, graph->getSize());
    graph->addNode();
    graph->deleteAll();
    QCOMPARE(0, graph->getSize());
    graph->addNodes(5);
    graph->setEdge(graph->getId(0), graph->getId(1));
    graph->setEdge(graph->getId(0), graph->getId(2));
    graph->setEdge(graph->getId(0), graph->getId(3));
    graph->setEdge(graph->getId(4), graph->getId(1));
    graph->deleteAll();
    QCOMPARE(0, graph->getSize());
}

void GraphTest::testSort()
{
    // sortAllChildren
    graph->addNodes(5);
    for(int i=1; i<graph->getSize(); i++) {
        QVERIFY(graph->getName(graph->getId(i-1)) < graph->getName(graph->getId(i)));
    }
    graph->setEdge(graph->getId(0), graph->getId(3));
    graph->setEdge(graph->getId(0), graph->getId(4));
    graph->setEdge(graph->getId(0), graph->getId(1));
    graph->setEdge(graph->getId(0), graph->getId(2));
    graph->sortAllChildren();
    for(int i=1; i<graph->getAdjNum(0); i++) {
        QVERIFY(graph->getAdjName(0,i-1) < graph->getAdjName(0,i));
    }

    // sortNodes
    graph->deleteAll();
    graph->addNode('B');
    graph->addNode('A');
    graph->addNode('E');
    graph->addNode('C');
    bool sorted = true;
    int i = 1;
    while (i < graph->getSize() && sorted) {
        sorted = graph->getName(graph->getId(i-1)) < graph->getName(graph->getId(i));
        i++;
    }
    QVERIFY(!sorted);
    graph->sortNodes();
    sorted = true;
    i = 1;
    while (i < graph->getSize() && sorted) {
        sorted = graph->getName(graph->getId(i-1)) < graph->getName(graph->getId(i));
        i++;
    }
    QVERIFY(sorted);
}

void GraphTest::testCheckAllEdgesNonnegative()
{
    QVERIFY(graph->checkAllEdgesNonnegative());
    graph->addNodes(3);
    QVERIFY(graph->checkAllEdgesNonnegative());
    graph->setEdge(graph->getId(0), graph->getId(1));
    graph->setEdge(graph->getId(0), graph->getId(2));
    graph->setEdge(graph->getId(2), graph->getId(1));
    QVERIFY(graph->checkAllEdgesNonnegative());
    graph->setEdge(graph->getId(0), graph->getId(2), -1);
    QVERIFY(!graph->checkAllEdgesNonnegative());
}

void GraphTest::testCheckConnectivity()
{
    QVERIFY(graph->checkConnectivity());
    graph->addNode();
    QVERIFY(graph->checkConnectivity());
    graph->addNode();
    QVERIFY(!graph->checkConnectivity());
    graph->setEdge(graph->getId(0), graph->getId(1));
    QVERIFY(graph->checkConnectivity());
}


// Algorithm

void GraphTest::testSelections()
{
    buildGraph(true, false);

    // selectAlgorithm
    QCOMPARE(Algorithm::Algorithms::None, algo->getChosenAlgo());
    algo->selectAlgorithm(Algorithm::Algorithms::Dijkstra);
    QCOMPARE(Algorithm::Algorithms::Dijkstra, algo->getChosenAlgo());
    algo->selectAlgorithm(Algorithm::Algorithms(7));
    QCOMPARE(Algorithm::Algorithms(7), algo->getChosenAlgo());
    algo->selectAlgorithm(Algorithm::Algorithms::Szelessegi);
    QCOMPARE(Algorithm::Algorithms::Szelessegi, algo->getChosenAlgo());

    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());

    // selectStartNode
    algo->selectStartNode(-2);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());
    algo->selectStartNode(0);
    algo->stepAlgorithm();
    QVERIFY(algo->getInitState());

}

void GraphTest::testStepAlgorithm()
{
    // SZÉLESSÉGI
    algo->selectAlgorithm(Algorithm::Szelessegi);
    // súlyozott gráf
    buildGraph(true, true);
    algo->selectStartNode(0);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());

    // súlyozatlan
    buildGraph(true, false);
    algo->stepAlgorithm();
    QVERIFY(algo->getInitState());

    // irányítatlan
    algo->reset();
    algo->selectAlgorithm(Algorithm::Szelessegi);
    buildGraph(false,false);
    algo->selectStartNode(0);
    algo->stepAlgorithm();
    QVERIFY(algo->getInitState());


    // MÉLYSÉGI
    algo->reset();
    algo->selectAlgorithm(Algorithm::Melysegi);
    // sulyozott
    buildGraph(true, true);
    algo->selectStartNode(0);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());

    // irányítatlan
    buildGraph(false, false);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());

    // irányított, súlyozatlan
    buildGraph(true, false);
    algo->stepAlgorithm();
    QVERIFY(algo->getInitState());

    // DIJKSTRA
    algo->reset();
    buildGraph(true, false);
    algo->selectAlgorithm(Algorithm::Dijkstra);
    algo->selectStartNode(0);
    // súlyozatlan
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());

    // negatív súly
    buildGraph(true, true);
    graph->setEdge(graph->getId(0), graph->getId(1), -3);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());


    // csak nemnegatív súlyok
    graph->setEdge(graph->getId(0), graph->getId(1), 5);
    algo->stepAlgorithm();
    QVERIFY(algo->getInitState());


    // PRIM
    algo->reset();
    buildGraph(true, false);
    algo->selectAlgorithm(Algorithm::Prim);
    algo->selectStartNode(0);
    // súlyozatlan
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());
    // irányított
    buildGraph(true, true);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());
    // nem összefüggő
    graph->deleteAll();
    graph->addNodes(2);
    graph->changeDirected(false);
    graph->changeWeighted(true);
    algo->stepAlgorithm();
    QVERIFY(!algo->getInitState());

    // súlyozott, irányítatlan, összefüggő
    buildGraph(false, true);
    algo->stepAlgorithm();
    QVERIFY(algo->getInitState());
}


// Private functions

void GraphTest::buildGraph(bool isDirected, bool isWeighted)
{
    graph->deleteAll();
    graph->addNodes(4);
    graph->changeDirected(isDirected);
    graph->changeWeighted(isWeighted);
    graph->setEdge(graph->getId(0), graph->getId(1), 5);
    graph->setEdge(graph->getId(0), graph->getId(2), 10);
    graph->setEdge(graph->getId(1), graph->getId(2));
    graph->setEdge(graph->getId(1), graph->getId(3), 3);
    graph->setEdge(graph->getId(3), graph->getId(2));
}




QTEST_APPLESS_MAIN(GraphTest)

#include "tst_graphtest.moc"
