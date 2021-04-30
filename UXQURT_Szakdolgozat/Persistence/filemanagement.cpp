#include "filemanagement.h"

#include <QFile>
#include <QTextStream>

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
    if (stream.atEnd()) return false;
    data.isDirected = stream.readLine().toInt();
    if (stream.atEnd() || (data.isDirected != 0 && data.isDirected != 1)) return false;
    data.isWeighted = stream.readLine().toInt();
    if (stream.atEnd() || (data.isWeighted != 0 && data.isWeighted != 1)) return false;

    QString tmp;
    for(int i=0; i<data.size; i++) {
        tmp = stream.readLine();
        QStringList node_data = tmp.split(';');
        if (node_data.length() != 2) return false;
        if (node_data.at(0).length() != 1) return false;
        data.names.append(node_data.at(0));
        QStringList points = node_data.at(1).split(",");
        if (points.length() != 2) return false;
        data.positions.append(QPointF(points.at(0).toDouble(), points.at(1).toDouble()));

    }
    for(int i=0; i<data.size; i++) {
        if (stream.atEnd()) return false;
        data.graph_representation.append(stream.readLine());
        QStringList tmp = data.graph_representation.last().split(";");
        if (data.graph_representation.last() != "") {
            if (tmp.last() == "") return false;
            foreach(QString t, tmp) {
                QStringList tmp2 = t.split(",");
                if (tmp2.length() != 2) return false;
                if (tmp2.at(0).length() != 1) return false;
            }
        }
    }
    return true;
}
