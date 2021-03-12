#ifndef EDGEGRAPHICS_H
#define EDGEGRAPHICS_H

#include <QGraphicsLineItem>
#include <QObject>
#include <QPainter>
#include <QPen>
#include "nodegraphics.h"

class NodeGraphics;
class EdgeGraphics : public QGraphicsLineItem
{
public:
    EdgeGraphics(NodeGraphics* from, NodeGraphics* to);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;                        // mi látszik
    QPainterPath shape() const;                         // ahol a kattintást is érzékeli

    int getFromNodeId();
    int getToNodeId();

    void setWeight(int w);
    void setSelected(bool s);

    void deleteEdgeWithNode(NodeGraphics* node);        // ha valamelyik csúcsot töröltük, a másik listájából töröljük csak (a törölt csűcs törli magának)
    void deleteEdgeFromNodes();                         // ha az élet töröltük mindkét csúcs listájából ki kell szedni
    void calculate();                                   // pozíciók kiszámítása, from és to összekötő egyenesét beállítja


private:
    NodeGraphics* fromNode;
    NodeGraphics* toNode;
    QPoint fromPoint;
    QPoint toPoint;
    QPointF controlPoint;
    QLineF ghostLine;
    int weight;
    bool weightSelected;            // ha igaz, akkor pirossal rajzolja a súly körét

};

#endif // EDGEGRAPHICS_H
