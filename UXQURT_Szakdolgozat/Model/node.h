#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QVector>
#include "adjnode.h"

class AdjNode;
class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QObject *parent = nullptr);
    ~Node();


    int getId();
    QChar getName();
    int getAdjNum();                    // gyerekek számát adja meg
    int getWeight(Node* to);            // él súlya a megfelelő Node-hoz
    QChar getAdjNodeName(int i);        // i. indexű gyereke neve
    int getAdjNodeWeight(int i);        // i. indexű gyereke élsúlya

    bool setEdge(Node *to, int w);      // ha új él, akkor igazzal, ha nem, akkor hamissal tér vissza
    void setReversedEdge();             // ha valamelyik csúcsba megy ebből él, akkor a csúcsból ebbe is beállít egy élt (irányított -> irányítatlan váltás)
    void setReversedEdge(Node* from, int weight);
    void deleteEdge(Node* to);
    void deleteReversed(Node* from);
    void setName(QChar name);
    bool hasEdge(Node* to);
    bool checkAllEdgesNonnegative();
    void sortChildren();
    static void resetIds();

signals:
    void edgeChanged(int fromId, int toId, int w, int new_edge);    // létrejött új vagy módosult a súly
    void edgeDeleted(int fromId, int toId);


private:
    static int node_ids;
    int id;
    QChar name;
    QVector<AdjNode*> adjNodes;

    int findAdjNode(Node* to);
};


#endif // NODE_H
