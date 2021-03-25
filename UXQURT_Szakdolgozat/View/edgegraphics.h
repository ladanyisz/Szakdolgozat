#ifndef EDGEGRAPHICS_H
#define EDGEGRAPHICS_H

#include <QGraphicsLineItem>
#include <QObject>
#include <QPainter>
#include <QPen>
#include "nodegraphics.h"
#include "../Model/node.h"
#include "../Model/algorithm.h"

class NodeGraphics;
class EdgeGraphics : public QObject, public QGraphicsLineItem   // QObject - események miatt
{
    Q_OBJECT

public:
    EdgeGraphics(NodeGraphics* from, NodeGraphics* to);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;                        // mi látszik
    QPainterPath shape() const;                         // ahol a kattintást is érzékeli

    void changePen(Algorithm::EdgeType type);
    int getFromNodeId();
    int getToNodeId();

    void setWeight(int w);
    void setSelected(bool s);


    void deleteEdgeWithNode(NodeGraphics* node);        // ha valamelyik csúcsot töröltük, a másik listájából töröljük csak (a törölt csűcs törli magának)
    void deleteEdgeFromNodes();                         // ha az élet töröltük mindkét csúcs listájából ki kell szedni
    void calculate();                                   // pozíciók kiszámítása, from és to összekötő egyenesét beállítja

public slots:
    void setDirected(bool d);
    void setWeighted(bool w);

private:
    QPen normalPen;
    QPen examinedPen;
    QPen neededPen;
    QPen notNeededPen;
    NodeGraphics* fromNode;
    NodeGraphics* toNode;
    QPoint fromPoint;
    QPoint toPoint;
    QPointF controlPoint;
    QLineF ghostLine;
    int weight;
    bool weightSelected;            // ha igaz, akkor pirossal rajzolja a súly körét
    bool isDirected;                // irányított-e
    bool hasWeight;                 // vannak-e súlyai (ha nincs, akkor egyet kell beállítani

};

#endif // EDGEGRAPHICS_H
