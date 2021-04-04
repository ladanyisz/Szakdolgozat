#ifndef ALGORITHMINFOS_H
#define ALGORITHMINFOS_H

#include <QLabel>
#include <QWidget>
#include "../Model/algorithm.h"
#include "edgegraphics.h"
#include "nodegraphics.h"

class AlgorithmInfos : public QWidget
{
    Q_OBJECT
public:
    explicit AlgorithmInfos(QWidget *parent = nullptr);
    enum InAlgorithm { None, Init, Outer, IfTrue, IfFalse };

    void setAlgorithm(Algorithm::Algorithms a);


public slots:
    void setStep(int);
    void setColorPart(InAlgorithm b);


private:
    QLabel* proceccedNodeLabel;
    QLabel* unknownNodeLabel;
    QLabel* examinAdjNodeLabel;
    QLabel* examinedNodeLabel;
    QLabel* greyNodeLabel;

    Algorithm::Algorithms algo;
    int algorithmStep;

    void paintColorHelp(int w, int h, QPainter &painter);
    int paintSzelessegi(int w, int h, QPainter &painter);       // a stuki magasságával tér vissza
    int paintDijkstra(int w, int h, QPainter &painter);
    int paintPrim(int w, int h, QPainter &painter);
    int paintMelysegi(int w, int h, QPainter &painter);

    InAlgorithm rowsToColor;


protected:
    void paintEvent(QPaintEvent *event);


};

#endif // ALGORITHMINFOS_H
