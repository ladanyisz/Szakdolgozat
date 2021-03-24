#include "adjnode.h"

AdjNode::AdjNode(Node *n, int w, QObject *): node(n), weight(w)
{
}

int AdjNode::getWeight() { return weight; }

void AdjNode::setWeight(int w)
{
    weight = w;
}




