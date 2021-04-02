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

    void setAlgorithm(Algorithm::Algorithms a);

signals:

private:
    QLabel* proceccedNodeLabel;
    QLabel* unknownNodeLabel;
    QLabel* examinAdjNodeLabel;
    QLabel* examinedNodeLabel;
    QLabel* greyNodeLabel;

    Algorithm::Algorithms algo;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // ALGORITHMINFOS_H
