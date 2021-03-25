#ifndef ADJNODE_H
#define ADJNODE_H

#include <QObject>
#include "node.h"

class Node;
class AdjNode : public QObject
{
    Q_OBJECT
public:
    AdjNode(Node* n, int w = 1, QObject *parent = nullptr);

    Node* node;         // az a csúcs, amibe az él mutat

    int getWeight();

    void setWeight(int w);



private:

    int weight;     // az él súlya
//    EdgeType type;

};

#endif // ADJNODE_H
