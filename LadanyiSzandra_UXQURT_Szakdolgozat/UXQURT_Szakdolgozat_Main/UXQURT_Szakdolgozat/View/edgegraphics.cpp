#include "edgegraphics.h"
#include "math.h"
#include <QMatrix>


QColor EdgeGraphics::normal = QColor(0,0,0);
QColor EdgeGraphics::examined = QColor(255, 174, 0);
QColor EdgeGraphics::needed = QColor(63, 181, 113);
QColor EdgeGraphics::notNeeded = QColor(220,220, 220);

EdgeGraphics::EdgeGraphics(NodeGraphics* from, NodeGraphics* to) :
    fromNode(from),
    toNode(to)
{
    QPen pen;
    pen.setWidth(2);
    this->setPen(pen);

    normalPen = QPen(EdgeGraphics::normal);
    examinedPen = QPen(EdgeGraphics::examined);
    neededPen = QPen(EdgeGraphics::needed);
    notNeededPen = QPen(EdgeGraphics::notNeeded);
    isNotNeeded = false;

    calculate();

    fromNode->addEdge(this);
    toNode->addEdge(this);

    weightSelected = false;
    isDirected = true;
    hasWeight = true;
    setZValue(0);

    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}


// Getters

int EdgeGraphics::getFromNodeId() { return fromNode->getId(); }

int EdgeGraphics::getToNodeId() { return toNode->getId(); }


// Setters

void EdgeGraphics::setWeight(int w) {
    weight = w;
    update();
}

void EdgeGraphics::setSelected(bool s) {
    weightSelected = s;
    update();
}

void EdgeGraphics::setDirected(bool d) {
    isDirected = d;
    update();
}

void EdgeGraphics::setWeighted(bool w) {
    hasWeight = w;
    update();
}


// Other public functions

void EdgeGraphics::deleteEdgeWithNode(NodeGraphics *node)
{
    if (node == fromNode) {
        toNode->removeEdge(this);
    } else {
        fromNode->removeEdge(this);
    }
}

void EdgeGraphics::deleteEdgeFromNodes()
{
    fromNode->removeEdge(this);
    toNode->removeEdge(this);
}

void EdgeGraphics::calculate()
{
    double radius = (NodeGraphics::getSize() / 2);

    fromPoint = (fromNode->pos()).toPoint();
    fromPoint.setY(fromPoint.y() + radius);     // kör középpontjába betolni
    fromPoint.setX(fromPoint.x() + radius);
    toPoint = (toNode->pos()).toPoint();
    toPoint.setY(toPoint.y() + radius);
    toPoint.setX(toPoint.x() + radius);


    QLineF unit_vector = QLineF(fromPoint, toPoint).unitVector();
    if (unit_vector.length() > 0) {
        unit_vector.setLength(radius);
        QMatrix translate_points;
        translate_points.translate(unit_vector.dx(), unit_vector.dy());
        fromPoint = translate_points.map(fromPoint);
        translate_points.translate(-2*unit_vector.dx(), -2*unit_vector.dy());
        toPoint = translate_points.map(toPoint);
    }

    QMatrix matrix;
    QLineF newLine = QLineF(fromPoint, toPoint);
    QLineF normal = newLine.normalVector();
    normal.setLength(60);
    matrix.translate(normal.dx(), normal.dy());
    controlPoint = matrix.map(QLineF(fromPoint,toPoint).center());
    ghostLine = QLineF(controlPoint, toPoint);

    setLine(newLine);
    prepareGeometryChange();
}


// From base classes

void EdgeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    calculate();

    QPen pen = this->pen();
    pen.setWidth(2);
    painter->setPen(pen);


    // él íve
    QPainterPath myPath;
    myPath.moveTo(fromPoint);
    myPath.quadTo(controlPoint, ghostLine.p2());

    if (isDirected) {

        painter->drawPath(myPath);

        // nyíl vonalai
        qreal alpha = ghostLine.angle();
        painter->save();

        painter->translate(toPoint.x(), toPoint.y());
        painter->rotate(180 - alpha + 30);
        QLine line1;
        line1 = QLine(0,0,10,0);
        painter->drawLine(line1);
        painter->restore();
        painter->save();

        painter->translate(toPoint.x(), toPoint.y());
        painter->rotate(180 - alpha - 30);
        QLine line2 = QLine(0,0,10,0);
        painter->drawLine(line2);
        painter->restore();
    } else {
        painter->drawLine(line());
    }


    if (hasWeight) {

        // súly
        if (weightSelected) pen.setColor(Qt::red);
        painter->setPen(pen);
        if (isNotNeeded) painter->setBrush(QColor(notNeededPen.color()));
        else if (this->pen().color() != Qt::black) painter->setBrush(this->pen().color());
        else painter->setBrush(QColor(222, 222, 222));

        if (isDirected) painter->translate(myPath.pointAtPercent(0.5));
        else painter->translate(line().center());
        painter->drawEllipse(QPoint(0,0), 14,14);
        QFont font;
        font.setBold(true);
        font.setPointSize(12);
        if (isNotNeeded) painter->setPen(QColor(190,190,190));
        else painter->setPen(Qt::black);
        painter->setFont(font);
        if (weight < 10 && weight > -1) {
            painter->drawText(- 5 ,6, QString::number(weight));
        } else if ((weight >= 10 && weight < 100) || (weight < 0 && weight > -10)) {
           painter->drawText( - 9 , 6, QString::number(weight));
        } else {
            painter->drawText( - 15 , 6, QString::number(weight));
        }
    }

    // mélységi gráfkeresés esetén az él típusa
    if (edgeStr != "") {
        QFont font;
        font.setPointSize(9);
        painter->setPen(QColor(140, 140, 140));
        painter->setFont(font);
        if (isDirected) painter->translate(myPath.pointAtPercent(0.5));
        else painter->translate(line().center());
        painter->drawText(-10,0, edgeStr);
    }
}

QRectF EdgeGraphics::boundingRect() const
{
    qreal extra;
    if (isDirected) {
        extra = 60;
        return QRectF(line().p1(),
                     QSize(line().p2().x()-line().p1().x(), line().p2().y()-line().p1().y())).normalized().adjusted(-extra,-extra, extra, extra);
    }
    if (abs(toPoint.x() - fromPoint.x()) < 24 || abs(toPoint.y() - fromPoint.y())< 24) extra = 18;
    else extra = 8;
    return QRectF(line().p1(),
                 QSize(line().p2().x()-line().p1().x(), line().p2().y()-line().p1().y())).normalized().adjusted(-extra,-extra, extra, extra);

}

QPainterPath EdgeGraphics::shape() const
{
    QPainterPath path;
    if (isDirected) {
        QLineF normal = line().normalVector();
        normal.setLength(14);

        QMatrix matrix;
        matrix.translate(normal.dx(), normal.dy());
        QMatrix matrix2;
        matrix2.translate(-normal.dx(), -normal.dy());

        QPainterPath myPath;
        myPath.moveTo(fromPoint);
        myPath.quadTo(controlPoint, ghostLine.p2());

        path.addPath(matrix.map(myPath));
        path.lineTo(matrix2.map(toPoint));
        path.moveTo(matrix.map(fromPoint));
        path.lineTo(matrix2.map(fromPoint));
        path.addPath(matrix2.map(myPath));
    } else {

        qreal adjust = 14;
        qreal alpha = line().angle();
        qreal line_length = line().length();
        QPoint p1 = QPoint(-(line_length/2),-adjust);
        QPoint p2 = QPoint(line_length/2, -adjust);
        QPoint p3 = QPoint(line_length/2, adjust);
        QPoint p4 = QPoint(-(line_length/2),adjust);

        QVector<QPoint> points = { p1, p2, p3, p4,};
        QPolygon pol = (QPolygon(points));

        QMatrix matrix;
        matrix.translate(line().center().x(), line().center().y()).rotate(360-alpha);
        pol = matrix.map(pol);
        path.addPolygon(pol);
    }

    return path;
}

void EdgeGraphics::changePen(Algorithm::EdgeType type)
{
    switch (type) {
    case Algorithm::EdgeType::BaseEdge:
        this->setPen(normalPen);
        edgeStr = "";
        isNotNeeded = false;
        break;
    case Algorithm::EdgeType::ExaminedEdge:
        this->setPen(examinedPen);
        edgeStr = "";
        isNotNeeded = false;
        break;
    case Algorithm::EdgeType::NeededEdge:
        this->setPen(neededPen);
        edgeStr = "";
        isNotNeeded = false;
        break;
    case Algorithm::EdgeType::NotNeededEdge:
        this->setPen(notNeededPen);
        edgeStr = "";
        isNotNeeded = true;
        break;
    case Algorithm::EdgeType::BackEdge:
        this->setPen(notNeededPen);
        edgeStr = tr("vissza-él");
        isNotNeeded = true;
        break;
    case Algorithm::EdgeType::ForwardEdge:
        this->setPen(notNeededPen);
        edgeStr = tr("előre-él");
        isNotNeeded = true;
        break;
    case Algorithm::EdgeType::CrossEdge:
        this->setPen(notNeededPen);
        edgeStr = tr("kereszt-él");
        isNotNeeded = true;
        break;

    }
}
