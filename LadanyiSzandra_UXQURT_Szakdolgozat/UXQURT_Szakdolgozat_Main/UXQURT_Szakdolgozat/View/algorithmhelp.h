#ifndef ALGORITHMHELP_H
#define ALGORITHMHELP_H

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "Model/algorithm.h"

class AlgorithmHelp : public QWidget
{
    Q_OBJECT
public:
    explicit AlgorithmHelp(Algorithm::Algorithms a, QWidget *parent = nullptr);

    static bool SzelessegiWindow;
    static bool MelysegiWindow;
    static bool PrimWindow;
    static bool DijkstraWindow;

signals:

public slots:


private:

    Algorithm::Algorithms a;

    QVBoxLayout* layout;
    QFont titleFont;
    QFont normalFont;

    QString static const pi;

    QLabel* label;
    QLabel* graphLabel;
    QLabel* stukiLabel;
    QVBoxLayout* data_layout;

    void createSzelessegi();
    void createMelysegi();
    void createPrim();
    void createDijkstra();



protected:
    void closeEvent(QCloseEvent *event);
};

#endif // ALGORITHMHELP_H
