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

    enum NodeType {None, Examined, Processed, ExamineAdj };

    int getId();
    QChar getName();
    int getAdjNum();                    // gyerekek számát adja meg
    int getWeight(Node* to);            // él súlya a megfelelő Node-hoz
    QChar getAjdNodeName(int i);        // serializeGraph-ban használt
    int getAdjNodeWeight(int i);        // serializeGraph-ban használt

    bool setEdge(Node *to, int w);      // ha új él, akkor igazzal, ha nem, akkor hamissal tér vissza
    void setReversedEdge();             // ha valamelyik csúcsba megy ebből él, akkor a csúcsból ebbe is beállít egy élt (irányított -> irányítatlan váltás)
    void deleteEdge(Node* to);
    void setName(QChar name);
    bool hasEdge(Node* to);

signals:
    void edgeChanged(int fromId, int toId, int w, int new_edge);


private:
    static int node_ids;
    int id;
    QChar name;
    QVector<AdjNode*> adjNodes;
    NodeType type;

    int findAdjNode(Node* to);
};


#endif // NODE_H
