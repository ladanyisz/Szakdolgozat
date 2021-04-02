#include "algorithminfos.h"

#include <QPainter>
#include <QVBoxLayout>

AlgorithmInfos::AlgorithmInfos(QWidget *parent) : QWidget(parent)
{
    algo = Algorithm::Algorithms::None;

    this->setMinimumWidth(200);
}

void AlgorithmInfos::setAlgorithm(Algorithm::Algorithms a)
{
    algo = a;
//    if (algo == Algorithm::Algorithms::Szelessegi || algo == Algorithm::Algorithms::Melysegi)
//        this->setMinimumWidth(220);
//    else
//        this->setMinimumWidth(200);
}

void AlgorithmInfos::paintEvent(QPaintEvent *)
{
//    if (algo == Algorithm::Algorithms::None) return;
    QFont font;
    font.setPixelSize(22);
    font.setBold(true);
    QPainter painter(this);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);

    int n = 16;
    int e = 13;
    int h = 25;
    int t = 30;
    painter.drawText(5,h, tr("Színkódok"));

    h += 30;
    font.setBold(false);
    font.setPixelSize(14);
    font.setUnderline(true);
    painter.setFont(font);
    painter.drawText(5, h, tr("Csúcsok"));
    h+= 25;
    font.setUnderline(false);
    painter.setFont(font);

    painter.setBrush(NodeGraphics::normalNode);
    painter.drawEllipse(5,h-e, n,n);
    painter.drawText(t, h, tr("Felfedezetlen csúcs"));

    if (algo == Algorithm::Algorithms::Szelessegi || algo == Algorithm::Algorithms::Melysegi) {
        h += 20;
        painter.setBrush(NodeGraphics::grey);
        painter.drawEllipse(5, h-e, n, n);
        painter.drawText(t, h, tr("Megtalált, de még"));
        h+= 17;
        painter.drawText(t, h, tr("feldolgozatlan csúcs"));
    }

    h += 20;
    painter.setBrush(NodeGraphics::examineAdj);
    painter.drawEllipse(5, h-e, n,n);
    painter.drawText(t, h, tr("Kiterjesztés alatt álló csúcs"));

    h+= 20;
    painter.setBrush(NodeGraphics::examined);
    painter.drawEllipse(5, h-e, n, n);
    painter.drawText(t, h, tr("Vizsgált szomszéd"));

    h += 20;
    painter.setBrush(NodeGraphics::processed);
    painter.drawEllipse(5, h-e, n,n);
    painter.drawText(t, h, tr("Feldolgozott csúcs"));

    h += 30;
    font.setUnderline(true);
    painter.setFont(font);

    painter.drawText(5, h, tr("Élek"));

    font.setUnderline(false);
    painter.setFont(font);

    h += 25;

    QPen pen;
    pen.setWidth(2);
    pen.setColor(EdgeGraphics::normal);
    painter.setPen(pen);

    painter.drawLine(5, h, 22, h - 10);
    painter.drawText(t, h, tr("Feldolgozatlan él"));

    h += 20;
    pen.setColor(EdgeGraphics::needed);
    painter.setPen(pen);
    painter.drawLine(5, h, 22, h-10);
    painter.setPen(Qt::black);
    painter.drawText(t, h, tr("Számolt feszítőfa része"));

    h += 20;
    pen.setColor(EdgeGraphics::notNeeded);
    painter.setPen(pen);
    painter.drawLine(5, h, 22, h-10);
    painter.setPen(Qt::black);
    painter.drawText(t, h, tr("Nem kell a feszítőfába"));
}
