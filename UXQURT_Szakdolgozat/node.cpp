#include "node.h"


int Node::node_ids = 0;

Node::Node(QObject *parent) : QObject(parent)
{
    id = Node::node_ids++;
}

Node::~Node()
{
    foreach(AdjNode* an, adjNodes) {
        delete an;
    }
}

// Getters

int Node::getId() {return id;}

QChar Node::getName() { return name; }

int Node::getAdjNum() {return adjNodes.size();}

int Node::getWeight(Node *to)
{
    int i = findAdjNode(to);
    return adjNodes.at(i)->getWeight();
}

QChar Node::getAjdNodeName(int i) {return adjNodes.at(i)->node->getName();}

int Node::getAdjNodeWeight(int i) {return adjNodes.at(i)->getWeight();}

int Node::findAdjNode(Node *to)
{
    int i = 0;
    while(i < adjNodes.size()) {
        if (adjNodes[i]->node == to) return i;
        i++;
    }
    return -1;
}

// Setters

bool Node::setEdge(Node *to, int w)
{
    int pos = findAdjNode(to);
    if (pos != -1) {
        adjNodes[pos]->setWeight(w);
        return false;
    }
    adjNodes.append(new AdjNode(to, w));
    return true;
}

void Node::setReversedEdge()
{
    foreach(AdjNode* adjNode, adjNodes) {
        setReversedEdge(adjNode->node, adjNode->getWeight());
    }
}

void Node::setReversedEdge(Node *from, int weight)
{
    bool is_new = from->setEdge(this, weight);
    emit edgeChanged(from->getId(), id, weight, is_new);
}

void Node::setName(QChar name) { this->name = name; }


// Other public functions

void Node::deleteEdge(Node *to)
{
    int pos = findAdjNode(to);
    AdjNode* adjNode = adjNodes[pos];
    adjNodes.remove(pos);
    delete adjNode;
}

void Node::deleteReversed(Node *from)
{
    from->deleteEdge(this);
    emit edgeDeleted(from->getId(), id);
}

bool Node::hasEdge(Node *to)
{
    return findAdjNode(to) != -1;
}
