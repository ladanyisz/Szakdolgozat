#include "filemanagement.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

FileManagement::FileManagement()
{

}

bool FileManagement::saveGraph(QString path, graph_data data)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) return false;

    QTextStream stream(&file);
    stream << data.size << Qt::endl;
    stream << data.isDirected << Qt::endl << data.isWeighted << Qt::endl;
    for(int i=0; i<data.names.size(); i++) {
        stream << data.names.at(i) << ";" << QString::number(data.positions.at(i).x()) << "," << QString::number(data.positions.at(i).y()) << Qt::endl;
    }
    foreach(QString gr, data.graph_representation) {
        stream << gr << Qt::endl;
    }
    file.close();
    return true;
}

bool FileManagement::loadGraph(QString path, graph_data &data)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) return false;

    QTextStream stream(&file);
    data.size = stream.readLine().toInt();
    qDebug() << QString::number(data.size);
    if (stream.atEnd()) return false;
    data.isDirected = stream.readLine().toInt();
    qDebug() << QString::number(data.isDirected);
    if (stream.atEnd()) return false;
    data.isWeighted = stream.readLine().toInt();
    qDebug() << QString::number(data.isWeighted);
    if (stream.atEnd()) return false;
    QString tmp;
    for(int i=0; i<data.size; i++) {
        tmp = stream.readLine();
        QStringList node_data = tmp.split(';');
        if (node_data.length() != 2) return false;
        data.names.append(node_data.at(0));
        QStringList points = node_data.at(1).split(",");
        if (points.length() != 2) return false;
        data.positions.append(QPointF(points.at(0).toDouble(), points.at(1).toDouble()));

    }
    for(int i=0; i<data.size; i++) {
        if (stream.atEnd()) return false;
        data.graph_representation.append(stream.readLine());
    }
    return true;
}
