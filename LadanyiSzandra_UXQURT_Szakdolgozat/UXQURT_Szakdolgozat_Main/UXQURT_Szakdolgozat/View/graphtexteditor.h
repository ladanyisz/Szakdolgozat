#ifndef GRAPHTEXTEDITOR_H
#define GRAPHTEXTEDITOR_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include "Model/graph.h"
#include "graphtextline.h"

class GraphTextEditor : public QWidget
{
    Q_OBJECT



public:
    explicit GraphTextEditor(Graph* graph, QWidget *parent = nullptr);
    void initEditor();

public slots:
    void addNewEdge();
    void refreshGraphSize();
    void deleteEdgeLine();
    void setEdge();
    void fromNodeChanged(QString fromText);


signals:
    void nodesChanged();
    void graphReady();
    void newEdge(QString, QString, int);
    void edgeSet(QString, QString, int);
    void edgeDeleted(QString, QString);
    void edgesFull();


private:
    Graph* graph;

    QVBoxLayout* graphTextLayout;
    QHBoxLayout* nodeLayout;
    QLabel* nodeLabel;
    QSpinBox* nodeSpinBox;
    QLabel* edgeLabel;
    QVBoxLayout* edgeLayout;
    QList<GraphTextLine*> edgeLines;
    QHBoxLayout* buttonLayout;
    QPushButton* newEdgeButton;
    QPushButton* okButton;
    QStringList names;


    GraphTextLine* addNewLine();
    void updateNames();
};

#endif // GRAPHTEXTEDITOR_H
