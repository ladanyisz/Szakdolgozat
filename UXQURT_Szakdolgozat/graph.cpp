#include "graph.h"

Graph::Graph(QObject *parent) : QObject(parent)
{
    isDirected = true;
    hasWeights = true;
    for(int i=0; i<maxNodeNum; i++) {
        names.append(true);
    }
}

Graph::~Graph()
{
    foreach(Node* node, nodes) {
        delete node;
    }
}


// Getters

bool Graph::getDirected() { return isDirected; }

bool Graph::getWeighted() { return hasWeights; }

int Graph::getSize() { return nodes.size(); }

QChar Graph::getName(int id) { return findById(id)->getName(); }

int Graph::getMaxNodeNum() { return maxNodeNum; }

int Graph::getId(int i) { return nodes.at(i)->getId(); }

int Graph::getWeight(int fromId, int toId)
{
    Node* fromNode = findById(fromId);
    Node* toNode = findById(toId);
    return fromNode->getWeight(toNode);
}

int Graph::getAdjNum(int ind) { return nodes.at(ind)->getAdjNum(); }

QChar Graph::getAdjName(int ind_this, int ind_ajd) { return nodes.at(ind_this)->getAjdNodeName(ind_ajd); }

int Graph::getAdjWeight(int ind_this, int ind_ajd) { return nodes.at(ind_this)->getAdjNodeWeight(ind_ajd); }

QStringList Graph::getNames()
{
    QStringList names;
    foreach(Node* node, nodes) {
        names.append(node->getName());
    }
    return names;
}


// Other public functions

bool Graph::setEdge(int fromId, int toId, int w)
{
    Node* fromNode = findById(fromId);
    Node* toNode = findById(toId);
    if (fromNode != nullptr && toNode != nullptr) {
        if (!isDirected) fromNode->setReversedEdge(toNode, w);
        return fromNode->setEdge(toNode, w);
    }
    return false;
}

bool Graph::setEdge(QString fromName, QString toName, int w)
{
    Node* fromNode = findByName(fromName);
    Node* toNode = findByName(toName);
    if (fromNode != nullptr && toNode != nullptr) {
        return fromNode->setEdge(toNode, w);
    }
    return false;

}

int Graph::addNode()
{
    if (nodes.size() < maxNodeNum) {
        Node* newNode = new Node();
        nodes.append(newNode);
        int ind = names.indexOf(true);
        QChar a = 'A';
        QChar n = QChar(ind + a.unicode());
        names[ind] = false;
        newNode->setName(n);
        connect(newNode, &Node::edgeChanged, this, [=](int f, int t, int w, bool n) { emit edgeChanged(f,t,w, n); });
        return newNode->getId();
    }
    emit nodesFull();
    return -1;
}

void Graph::addNodes(int n)
{
    for(int i=0; i<n; i++) {
        addNode();
    }
}

void Graph::deleteEdge(int fromId, int toId)
{
    Node* fromNode = findById(fromId);
    Node* toNode = findById(toId);
    if (fromNode != nullptr && toNode != nullptr) {
        fromNode->deleteEdge(toNode);
    }
}

void Graph::deleteEdge(QString fromName, QString toName)
{
    Node* fromNode = findByName(fromName);
    Node* toNode = findByName(toName);
    if (fromNode != nullptr && toNode != nullptr) {
        fromNode->deleteEdge(toNode);
    }
}

void Graph::deleteNode(int id)
{
    Node* node = findById(id);
    foreach(Node* other_node, nodes) {
        if (other_node != node) {
            if (other_node->hasEdge(node)) {
                other_node->deleteEdge(node);
            }
        }
    }
    QChar name = node->getName();
    nodes.removeAll(node);
    QChar a = 'A';
    names[name.unicode() - a.unicode()] = true;
    delete node;
}

void Graph::deleteNodes(int n)
{
    int size = getSize();
    for(int i=size-1; i >= size-n; i--) {
        deleteNode(nodes.at(i)->getId());
    }
}

void Graph::deleteAll()
{
    foreach(Node* node, nodes) {
        delete node;
    }
    nodes.resize(0);
    for(int i=0; i<Graph::getMaxNodeNum(); i++) {
        names[i] = true;
    }
}

void Graph::serializeGraph()
{
    qDebug() << "Num of nodes: " << getSize();
    foreach(Node* node, nodes) {
        QString msg = "";
        msg += QString(node->getName()) + "     |     ";
        for(int i=0; i<node->getAdjNum(); i++) {
            msg += "(" + QString(node->getAjdNodeName(i)) + "," + QString::number(node->getAdjNodeWeight(i)) + "), ";
        }
        qDebug() << msg;
    }

}


// Public slots

void Graph::changeDirected(bool d)
{
    if (isDirected && !d) {
        foreach(Node* node, nodes) {
            node->setReversedEdge();
        }
    }
    isDirected = d;
    emit directedChanged(d);
}

void Graph::changeWeighted(bool w)
{
    hasWeights = w;
    emit weightedChanged(w);
}


// Private functions

Node *Graph::findById(int id)
{
    foreach(Node* node, nodes) {
        if (node->getId() == id) {
            return node;
        }
    }
    return nullptr;
}

Node *Graph::findByName(QString name)
{
    foreach(Node* node, nodes) {
        if (QString(node->getName()) == name) {
            return node;
        }
    }
    return nullptr;
}
