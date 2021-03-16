#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QVector>
#include "node.h"
#include <QDebug>

class Graph : public QObject
{
    Q_OBJECT
public:
    explicit Graph(QObject *parent = nullptr);
    ~Graph();

    bool getDirected();
    bool getWeighted();
    int getSize();                                  // csúcsok száma
    QChar getName(int id);                          // Node id-ja alapján
    int getMaxNodeNum();
    int getId(int i);                               // a listában az i-ik csúcs id-ja
    int getWeight(int fromId, int toId);
    int getAdjNum(int ind);                         // szomszédok száma - index alapján
    QChar getAdjName(int ind_this, int ind_ajd);    // szomszéd neve - index és index alapján
    int getAdjWeight(int ind_this, int ind_ajd);    // él súlya szomszédhoz - index és index alapján
    QStringList getNames();                         // használt nevek

    int addNode();
    void addNodes(int n);                           // n új csúcs létrehozása
    bool setEdge(int fromId, int toId, int w = 1);  // ha új él, akkor igazzal, ha nem, akkor hamissal tér vissza
    bool setEdge(QString fromName, QString toName, int w = 1);
    void deleteEdge(int fromId, int toId);          // él (adjNode) törlése (id-k alapján)
    void deleteEdge(QString fromName, QString toName);
    void deleteNode(int id);                        // csúcs törlése id alapján
    void deleteNodes(int n);                        // n db csúcs törlése
    void deleteAll();                               // összes csúcs törlése
    void serializeGraph();

public slots:
    void changeDirected(bool d);
    void changeWeighted(bool w);

signals:
    void nodesFull();                               // a csúcsok száma elérte a maxNodeNum-ot
    void directedChanged(bool d);
    void weightedChanged(bool w);

private:
    static const int maxNodeNum = 15;
    QVector<Node*> nodes;
    QVector<bool> names;
    bool isDirected;
    bool hasWeights;


    Node* findById(int id);
    Node* findByName(QString name);


};

#endif // GRAPH_H
