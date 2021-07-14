#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H

#include <QString>
#include <QPointF>
#include <QStringList>

struct  graph_data {
    int size;
    int isDirected;
    int isWeighted;
    QStringList names;
    QVector<QString> graph_representation;
    QVector<QPointF> positions;
};


class FileManagement
{
public:
    FileManagement();

    bool saveGraph(QString path, graph_data data);
    bool loadGraph(QString path, graph_data &data);

};

#endif // FILEMANAGEMENT_H
