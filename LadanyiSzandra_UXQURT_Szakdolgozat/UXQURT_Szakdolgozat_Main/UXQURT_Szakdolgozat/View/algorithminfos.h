#ifndef ALGORITHMINFOS_H
#define ALGORITHMINFOS_H

#include <QFontMetrics>
#include <QLabel>
#include <QWidget>
#include "Model/algorithm.h"
#include "edgegraphics.h"
#include "nodegraphics.h"

class AlgorithmInfos : public QWidget
{
    Q_OBJECT
public:
    explicit AlgorithmInfos(QWidget *parent = nullptr);
    enum InAlgorithm { None, Init, Outer, IfTrue, IfFalse, MelysegiFirst, MelysegiLast };

    void setAlgorithm(Algorithm::Algorithms a);


public slots:
    void setColorPart(InAlgorithm b);


private:

    // stukik kirajzolásához szükséges értékek
    int width;
    int rowHeight;
    int colWidth;
    int textMarginLeft;
    double textVert;
    int pixSize;
    QFont font;
    QBrush brush;
    QBrush c_brush;

    QString static const pi;
    QString static const element_of;
    QString static const for_all;
    QString static const infinity;
    QString static const empty_set;
    QString static const logical_and;
    QString static const neg;

    Algorithm::Algorithms algo;

    void paintColorHelp(int w, int h, QPainter &painter);
    int paintSzelessegi(int w, int h, QPainter &painter);       // a stuki magasságával tér vissza
    int paintDijkstra(int w, int h, QPainter &painter);
    int paintPrim(int w, int h, QPainter &painter);
    int paintMelysegi(int w, int h, QPainter &painter);

    void paintStatement(QPainter &painter, int &w, int &h, int indent, int width, int row_count, QString s);
    void paintLoop(QPainter &painter, int &w, int &h, int indent, int rows_num, QString condition);
    void paintConditional(QPainter &painter, int &w, int &h, int indent, QString condition/*, QStringList trueStatements, QStringList falseStatements, bool paintTrue, bool paintFalse*/);

    InAlgorithm rowsToColor;


protected:
    void paintEvent(QPaintEvent *event);


};

#endif // ALGORITHMINFOS_H
