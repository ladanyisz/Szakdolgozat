#include "graphscene.h"

#include <QPixmap>
#include <QDebug>
#include <QGraphicsView>
#include <QMatrix>

GraphScene::GraphScene(Graph* graph, QObject *)
{
    mode = GraphMode::Move;
    this->graph = graph;
    currFrom = nullptr;
    currEdge = nullptr;

    connect(graph, &Graph::directedChanged, this, &GraphScene::setDirected);
    connect(graph, &Graph::weightedChanged, this, &GraphScene::setWeighted);
    connect(graph, &Graph::edgeChanged, this, &GraphScene::findEdgeNodes);
    connect(graph, &Graph::edgeDeleted, this, &GraphScene::deleteReversed);
    connect(graph, &Graph::graphDeleted, this, [=]() {
        QList<QGraphicsItem*> scene_items = items();
        foreach(QGraphicsItem* item, scene_items) {
            removeItem(item);
            delete item;
    }} );
}


// Public functions

QList<QPointF> GraphScene::originalPositions()
{
    QList<QGraphicsItem*> scene_items = items();
    QList<QPointF> positions;
    foreach(QGraphicsItem* item, scene_items) {
        QPointF pos = item->scenePos();
        positions.append(pos);
    }
    return positions;
}

void GraphScene::updatePositions(QList<QPointF> positions, float w_scale, float h_scale)
{
    QList<QGraphicsItem*> scene_items = items();
    for(int i=0; i<scene_items.size(); i++) {
        qreal x = positions[i].x() * w_scale;
        qreal y = positions[i].y() * h_scale;
        scene_items[i]->setPos(QPointF(x,y));
    }
}

void GraphScene::updateEdges()
{
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) {
            node->updateEdges();
        }
    }
}

void GraphScene::setWeight(int w)
{
    currEdge->setWeight(w);
    graph->setEdge(currEdge->getFromNodeId(), currEdge->getToNodeId(), w);
    resetCurrEdge();
}

QVector<QPointF> GraphScene::nodePositions()
{
    QVector<QPointF> points;
    points.resize(graph->getSize());
    qreal w = sceneRect().width();
    qreal h = sceneRect().height();
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) {
            int i = graph->getVectorPosition(node->getId());
            points[i] = QPointF(node->scenePos().x() / w, node->scenePos().y() / h);
        }
    }
    return points;
}


// Slots

void GraphScene::setMode(GraphScene::GraphMode mode)
{
    resetCurrFrom();
    resetCurrEdge();
//    if (this->mode != mode) {
        if (mode == GraphMode::AddEdge || mode ==GraphMode::Move || mode == GraphMode::SetWeight) {

            QList<QGraphicsItem*> scene_items = items();
            foreach(QGraphicsItem* item ,scene_items) {
                item->setCursor(Qt::PointingHandCursor);
            }
        } else if (this->mode != GraphMode::AddNode && mode == GraphMode::AddNode) {

            QList<QGraphicsItem*> scene_items = items();
            foreach(QGraphicsItem* item ,scene_items) {
                item->setCursor(Qt::ArrowCursor);
            }
        } else if (this->mode != GraphMode::Delete && mode == GraphMode::Delete) {

            QList<QGraphicsItem*> scene_items = items();
            foreach(QGraphicsItem* item ,scene_items) {
                QPixmap delete_cursor_pic = QPixmap(":/img/delete-cursor.png");
                QCursor delete_cursor = QCursor(delete_cursor_pic, 0,0);
                item->setCursor(delete_cursor);
            }
        }
        this->mode = mode;
//    }
}

void GraphScene::updateNodes()
{
    QList<NodeGraphics*> nodes;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) nodes.append(node);
    }
    if (nodes.size() < graph->getSize()) {
        while(nodes.size() < graph->getSize()) {
            NodeGraphics* newNode = new NodeGraphics();
            newNode->setRect(0,0,50,50);
            newNode->setPos(0,0);
            int id = graph->getId(nodes.size());
            newNode->setId(id);
            newNode->setName(graph->getName(id));
            nodes.append(newNode);
            addItem(newNode);
        }
        updateNodePositions(nodes);             // ha több csúcs lett, újrarendezzük

    } else if (nodes.size() > graph->getSize()) {
        while(nodes.size() > graph->getSize()) {
            NodeGraphics* removeNode = nodes.first();
            nodes.removeAll(removeNode);
            deleteNode(removeNode);
        }
    }
}

void GraphScene::resetNodeLayout()
{
    QList<NodeGraphics*> nodes;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) nodes.append(node);
    }
    updateNodePositions(nodes);
}

void GraphScene::findEdgeNodes(int from, int to, int w, bool is_new)
{
    NodeGraphics* fromNode = nullptr;
    NodeGraphics* toNode = nullptr;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (is_new) {
            if (node) {
                if (node->getId() == from) fromNode = node;
                else if (node->getId() == to) toNode = node;
            }
        } else {
            EdgeGraphics* _edge = qgraphicsitem_cast<EdgeGraphics*>(item);
            if (_edge) {
                if (_edge->getFromNodeId() == from && _edge->getToNodeId() == to) {
                    _edge->setWeight(w);
                }
            }
        }

    }
    if (fromNode != nullptr && toNode != nullptr) {
        addEdge(fromNode, toNode, w);
    }
}

void GraphScene::deleteReversed(int from, int to)
{
    EdgeGraphics* edge = nullptr;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        EdgeGraphics* _edge = qgraphicsitem_cast<EdgeGraphics*>(item);
        if (_edge) {
            if (_edge->getFromNodeId() == from && _edge->getToNodeId() == to) {
                edge = _edge;
            }
        }
    }
   deleteEdgeGraphics(edge);
}

void GraphScene::addNewEdge(QString fromName, QString toName, int w)
{
    NodeGraphics* fromNode = nullptr;
    NodeGraphics* toNode = nullptr;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) {
            if (graph->getName(node->getId()) == fromName) fromNode = node;
            else if (graph->getName(node->getId()) == toName) toNode = node;
        }

    }
    if (fromNode != nullptr && toNode != nullptr) {
        addEdge(fromNode, toNode, w);
    }
}

void GraphScene::deleteEdge(QString fromName, QString toName)
{
    EdgeGraphics* edge = nullptr;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        EdgeGraphics* _edge = qgraphicsitem_cast<EdgeGraphics*>(item);
        if (_edge) {
            if (graph->getName(_edge->getFromNodeId()) == fromName && graph->getName(_edge->getToNodeId()) == toName) {
                edge = _edge;
            }
        }
    }
    deleteEdgeGraphics(edge);
}

void GraphScene::setEdgeWeight(QString fromName, QString toName, int w)
{
    EdgeGraphics* edge = nullptr;
    QList<QGraphicsItem*> scene_items = items();
    foreach(QGraphicsItem* item, scene_items) {
        EdgeGraphics* _edge = qgraphicsitem_cast<EdgeGraphics*>(item);
        if (_edge) {
            if (graph->getName(_edge->getFromNodeId()) == fromName && graph->getName(_edge->getToNodeId()) == toName) {
                edge = _edge;
            }
        }
    }
    if (edge != nullptr) {
        edge->setWeight(w);
    }
}

void GraphScene::setDirected(bool d) { emit graphDirectedChanged(d); }

void GraphScene::setWeighted(bool w) { emit graphWeightedChanged(w); }

void GraphScene::changeNodeState(Node::NodeType type, int id)
{
    NodeGraphics* node = nullptr;
    QList<QGraphicsItem*> scene_items = items();
    int i = 0;
    while (i < scene_items.length() && node == nullptr) {
        NodeGraphics* n = qgraphicsitem_cast<NodeGraphics*>(scene_items.at(i));
        if (n && n->getId() == id) node = n;
        i++;
    }
    if (node != nullptr) node->changeBrush(type);
}

void GraphScene::changeEdgeState(AdjNode::EdgeType type, int from_id, int to_id)
{

}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
            return;

    if (mode == GraphMode::Move) {

        QGraphicsScene::mousePressEvent(event);

    } else if (mode == GraphMode::AddNode) {

        int id = graph->addNode();
        if (id > -1) {
            addNode(id, graph->getName(id), QPointF(event->scenePos().rx() - NodeGraphics::getSize()/2, event->scenePos().ry() - NodeGraphics::getSize()/2));
            emit nodeAdded();
            emit nodesChanged();
        }

    } else if (mode == GraphMode::AddEdge) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) {

            if (currFrom == nullptr) {
                currFrom = node;
                QPen pen;
                pen.setColor(Qt::red);
                pen.setWidth(3);
                currFrom->setPen(pen);
            } else {
                if (currFrom != node) {
                    if (graph->setEdge(currFrom->getId(), node->getId())) {
                        addEdge(currFrom, node, graph->getWeight(currFrom->getId(), node->getId()));
                    }
                }
                resetCurrFrom();
            }

        }

    } else if (mode == GraphMode::SetWeight) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        EdgeGraphics* edge = qgraphicsitem_cast<EdgeGraphics*>(item);
        if (edge) {
            if (currEdge != nullptr) currEdge->setSelected(false);
            currEdge = edge;
            currEdge->setSelected(true);
            emit edgeSelected();
        }

    } else if (mode == GraphMode::Delete) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        NodeGraphics* node = qgraphicsitem_cast<NodeGraphics*>(item);
        if (node) {
            graph->deleteNode(node->getId());
            deleteNode(node);
            emit nodesChanged();
            if (graph->getSize() == 0) emit allNodesDeleted();
        } else {
            EdgeGraphics* edge = qgraphicsitem_cast<EdgeGraphics*>(item);
            if (edge) {
                graph->deleteEdge(edge->getFromNodeId(), edge->getToNodeId());
                edge->deleteEdgeFromNodes();
                removeItem(edge);
                delete edge;
            }
        }
    }
    graph->serializeGraph();
}


// Private functions

void GraphScene::resetCurrFrom()
{
    if (currFrom != nullptr) {
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(2);
        currFrom->setPen(pen);
        currFrom = nullptr;
    }
}

void GraphScene::resetCurrEdge()
{
    if (currEdge != nullptr) {
        currEdge->setSelected(false);
        currEdge = nullptr;
    }
}

void GraphScene::updateNodePositions(QList<NodeGraphics *> nodes)
{
    int n = nodes.size();
    qreal a = 360.0 / n;
    QGraphicsView* view = views().at(0);
    QPointF center = QPointF(view->width()/2 - NodeGraphics::getSize() / 2, view->height()/2 - NodeGraphics::getSize() / 2); // sceneRect().center();
    qreal r = 0.75 * sceneRect().height() / 2;
    qreal b = 0;
    foreach(NodeGraphics* node, nodes) {
        QMatrix matrix;
        matrix.translate(center.x(), center.y()).rotate(b);
        b = b + a;
        QPointF pos = QPointF(r,0);
        pos = matrix.map(pos);
        node->setPos(pos);
    }
    updateEdges();
}

void GraphScene::addNode(int id, QChar name, QPointF pos)
{
    NodeGraphics* newNode = new NodeGraphics();
    newNode->setRect(0,0,50,50);
    newNode->setPos(pos);
    newNode->setId(id);
    newNode->setName(name);
    addItem(newNode);
}

void GraphScene::deleteNode(NodeGraphics *node)
{
    removeItem(node);
    for(int i=0; i<node->getEdgeNum(); i++) {
        EdgeGraphics* edge = node->getEdge(i);
        edge->deleteEdgeWithNode(node);
        removeItem(edge);
        delete edge;
    }
    delete node;
}

void GraphScene::addEdge(NodeGraphics *from, NodeGraphics *to, int weight)
{
    EdgeGraphics* edge = new EdgeGraphics(from, to);
    edge->setWeight(weight);
    edge->setDirected(graph->getDirected());
    edge->setWeighted(graph->getWeighted());
    connect(this, &GraphScene::graphDirectedChanged, edge, &EdgeGraphics::setDirected);
    connect(this, &GraphScene::graphWeightedChanged, edge, &EdgeGraphics::setWeighted);
    addItem(edge);
}

void GraphScene::deleteEdgeGraphics(EdgeGraphics *edge)
{
    if (edge != nullptr) {
        edge->deleteEdgeFromNodes();
        removeItem(edge);
        delete edge;
    }
}
