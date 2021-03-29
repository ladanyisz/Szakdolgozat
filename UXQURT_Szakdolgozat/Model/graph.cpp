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

int Graph::getVectorPosition(int id)
{
    Node* node = findById(id);
    return nodes.indexOf(node);
}

int Graph::getMaxNodeNum() { return maxNodeNum; }

int Graph::getId(int i) {
    if (nodes.size() > i) return nodes.at(i)->getId();
    else return -1;
}

int Graph::getIndex(Node *node)
{
    for(int i=0; i<nodes.length(); i++) {
        if (nodes.at(i) == node) return i;
    }
    return -1;
}

int Graph::getIndex(QString name)
{
    for(int i=0; i<nodes.length(); i++) {
        if (nodes.at(i)->getName() == name) return i;
    }
    return -1;
}

int Graph::getWeight(int fromId, int toId)
{
    Node* fromNode = findById(fromId);
    Node* toNode = findById(toId);
    return fromNode->getWeight(toNode);
}

int Graph::getAdjNum(int ind) { return nodes.at(ind)->getAdjNum(); }

QChar Graph::getAdjName(int ind_this, int ind_ajd) { return nodes.at(ind_this)->getAjdNodeName(ind_ajd); }

int Graph::getAdjWeight(int ind_this, int ind_ajd) { return nodes.at(ind_this)->getAdjNodeWeight(ind_ajd); }

int Graph::getAdjIndexInNodes(int from_index, int to_adj_index)
{
    Node* node = nodes.at(from_index);
    for(int i=0; i<nodes.length(); i++) {
        if (i != from_index) {
            if (node->getAjdNodeName(to_adj_index) == nodes.at(i)->getName()) return i;
        }
    }
    return -1;
}

QStringList Graph::getNames()
{
    QStringList names;
    foreach(Node* node, nodes) {
        names.append(node->getName());
    }
    return names;
}

bool Graph::checkAllEdgesNonnegative()
{
    foreach(Node* node, nodes) {
        if (!node->checkAllEdgesNonnegative()) return false;
    }
    return true;
}

bool Graph::checkConnectivity()     // erre még lehet egy jobb algoritmus (szélességi) --> fekete csúcsok
{
    if (isDirected) {
        foreach(Node* node, nodes) {
            if (node->getAdjNum() == 0) {
                bool no_edges = true;
                int i = 0;
                while (i < nodes.size() && no_edges) {
                    if (nodes[i] != node) no_edges = !nodes[i]->hasEdge(node);
                    i++;
                }
                if (no_edges) return false;
            }
        }
        return true;
    } else {
        foreach(Node* node, nodes) {
            if (node->getAdjNum() == 0) return false;
        }
        return true;
    }
}


// Other public functions

bool Graph::setEdge(int fromId, int toId, int w)
{
    Node* fromNode = findById(fromId);
    Node* toNode = findById(toId);
    return setEdge(fromNode, toNode, w);
}

bool Graph::setEdge(QString fromName, QString toName, int w)
{
    Node* fromNode = findByName(fromName);
    Node* toNode = findByName(toName);
    return setEdge(fromNode, toNode, w);
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
        connect(newNode, &Node::edgeDeleted, this, [=](int f, int t) { emit edgeDeleted(f,t); });
        return newNode->getId();
    }
    emit nodesFull();
    return -1;
}

int Graph::addNode(QChar name)
{
    if (nodes.size() < maxNodeNum) {
        Node* newNode = new Node();
        nodes.append(newNode);
        QChar a = 'A';
        int ind = name.unicode() - a.unicode();
        newNode->setName(name);
        names[ind] = false;
        connect(newNode, &Node::edgeChanged, this, [=](int f, int t, int w, bool n) { emit edgeChanged(f,t,w, n); });
        connect(newNode, &Node::edgeDeleted, this, [=](int f, int t) { emit edgeDeleted(f,t); });
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
    deleteEdge(fromNode, toNode);
}

void Graph::deleteEdge(QString fromName, QString toName)
{
    Node* fromNode = findByName(fromName);
    Node* toNode = findByName(toName);
    deleteEdge(fromNode, toNode);
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
    Node::resetIds();
    emit graphDeleted();
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

bool Graph::saveGraph(QString path, QVector<QPointF> positions)
{
    QVector<QString> graph_representation;
    foreach(Node* node, nodes) {
        QString gr = "";
        int adj_num = node->getAdjNum();
        if (adj_num >= 1) {
            for(int i=0; i<adj_num-1; i++) {
                gr += QString(node->getAjdNodeName(i)) + "," + QString::number(node->getAdjNodeWeight(i)) + ";";
            }
            gr += QString(node->getAjdNodeName(adj_num-1)) + "," + QString::number(node->getAdjNodeWeight(adj_num-1));
        }
        graph_representation.append(gr);
    }
    graph_data data;
    data.size = nodes.size();
    data.isDirected = isDirected ? 1 : 0;
    data.isWeighted = hasWeights ? 1 : 0;
    data.names = getNames();
    data.graph_representation = graph_representation;
    data.positions = positions;
    return file_management.saveGraph(path, data );
}

QVector<std::tuple<int, QChar, QPointF> > Graph::loadGraph(QString path)
{
    QVector<std::tuple<int, QChar, QPointF>> nodes_data;
    graph_data data;
    if (!file_management.loadGraph(path, data)) return nodes_data;
    deleteAll();
    isDirected = data.isDirected == 1;
    hasWeights = data.isWeighted == 1;
    for(int i=0; i<data.size; i++) {
        int id = addNode((QChar)(data.names.at(i)).at(0));
        std::tuple<int, QChar, QPointF> node_data = std::make_tuple(id, QChar(data.names.at(i).at(0)), data.positions.at(i));
        nodes_data.append(node_data);
    }
    for(int i=0; i<data.size; i++) {
        QStringList edges = data.graph_representation.at(i).split(";");
        foreach(QString edge, edges) {
            QStringList edge_data = edge.split(",");
            if (edge_data.length() == 2) {
                Node* to = findByName(edge_data.at(0));
                int w = edge_data.at(1).toInt();
                nodes.at(i)->setEdge(to, w);
            }
        }
    }
    serializeGraph();
    return nodes_data;
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

bool Graph::setEdge(Node *from, Node *to, int w)
{
    if (from != nullptr && to != nullptr) {
        if (!isDirected) from->setReversedEdge(to, w);
        if (from->setEdge(to, w)) {
            return true;
        }
    }
    return false;
}

void Graph::deleteEdge(Node *from, Node *to)
{
    if (from != nullptr && to != nullptr) {
        from->deleteEdge(to);
        if (!isDirected) from->deleteReversed(to);
    }
}
