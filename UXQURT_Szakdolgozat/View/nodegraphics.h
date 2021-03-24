#ifndef NODEGRAPHICS_H
#define NODEGRAPHICS_H

#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QObject>
#include <QCursor>
#include <QPen>
#include <QPainter>
#include "edgegraphics.h"
#include <QGraphicsScene>
#include "../Model/node.h"

class EdgeGraphics;
class NodeGraphics : public QGraphicsEllipseItem
{
    //Q_OBJECT
public:
    NodeGraphics();

    static int getSize();
    int getId();
    int getEdgeNum();
    EdgeGraphics* getEdge(int i);

    void setId(int i);
    void setName(QChar n);
    void addEdge(EdgeGraphics* edge);                   // felveszi az él grafikáját az élek listájába
    void removeEdge(EdgeGraphics* edge);                // leveszi az él grafikáját az élek listájából
    void updateEdges();                                 // újraszámlja a hozzá tartozó élek grafikáját
    void changeBrush(Node::NodeType);                   // algoritmusokhoz - kitöltés színét változtatja

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


private:
    static const int size = 50;
    int id;
    QChar name;
    QBrush normalBrush;
    QBrush examinedBrush;
    QBrush processedBrush;
    QBrush examineAdjBrush;

    QVector<EdgeGraphics*> edges;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);          // módosításkor (move) fut
};

#endif // NODEGRAPHICS_H
