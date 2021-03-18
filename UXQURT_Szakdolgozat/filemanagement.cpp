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
