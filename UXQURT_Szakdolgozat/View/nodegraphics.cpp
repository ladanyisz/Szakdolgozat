#include "nodegraphics.h"

#include <QColor>
#include <QFont>
#include <QDebug>

NodeGraphics::NodeGraphics()
{
    normalBrush = QBrush(QColor(124, 183, 214));
    examinedBrush = QBrush(QColor(255, 174, 0));
    processedBrush = QBrush(QColor(33, 143, 79));
    examineAdjBrush = QBrush(QColor(230, 94, 92));
    setRect(0,0,size,size);
    setBrush(normalBrush);
    QPen pen;
    pen.setWidth(2);
    setPen(pen);

    setFlag(GraphicsItemFlag::ItemIsMovable);
    setFlag(GraphicsItemFlag::ItemSendsGeometryChanges);
}


// Getters

int NodeGraphics::getSize() { return size; }

int NodeGraphics::getId() { return id; }

int NodeGraphics::getEdgeNum() { return edges.size(); }

EdgeGraphics *NodeGraphics::getEdge(int i) { return edges.at(i); }


// Setters

void NodeGraphics::setId(int i) { id = i; }

void NodeGraphics::setName(QChar n) { name = n; }


// Public functions

void NodeGraphics::changeBrush(Algorithm::NodeType type)
{
    switch (type) {
        case Algorithm::NodeType::BaseNode:
            this->setBrush(normalBrush);
            break;

        case Algorithm::NodeType::ExaminedNode:
            this->setBrush(examinedBrush);
            break;

        case Algorithm::NodeType::ProcessedNode:
            this->setBrush(processedBrush);
            break;

        case Algorithm::NodeType::ExamineAdj:
            this->setBrush(examineAdjBrush);
            break;
    }
}

void NodeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsEllipseItem::paint(painter, option, widget);
    QFont font;
    font.setPixelSize(32);
    QPen pen;
    pen.setColor(QColor(Qt::black));
    painter->setFont(font);
    painter->setPen(pen);
    painter->drawText(QPoint(size/4 + 4, (4.0/5)*size -2), name);
}

QVariant NodeGraphics::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        updateEdges();

        // value is the new position.
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();
        int s = NodeGraphics::getSize();
        if (!rect.contains(newPos) || newPos.x() >= rect.right()-s || newPos.y() >= rect.bottom() -s) {
            // Keep the item inside the scene rect.
            newPos.setX(qMin(rect.right()-s, qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom()-s, qMax(newPos.y(), rect.top())));
            return newPos;
        }

    }
    return QGraphicsItem::itemChange(change, value);
}


// Private functions

void NodeGraphics::addEdge(EdgeGraphics *edge) { edges.append(edge); }

void NodeGraphics::removeEdge(EdgeGraphics *edge) { edges.removeAll(edge); }

void NodeGraphics::updateEdges()
{
    foreach(EdgeGraphics* edge, edges) {
        edge->calculate();
    }
}


