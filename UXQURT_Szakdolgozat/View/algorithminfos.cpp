#include "algorithminfos.h"

#include <QPainter>
#include <QVBoxLayout>

AlgorithmInfos::AlgorithmInfos(QWidget *parent) : QWidget(parent)
{
    algo = Algorithm::Algorithms::None;
    this->setMinimumWidth(400);
}

void AlgorithmInfos::setAlgorithm(Algorithm::Algorithms a)
{
    algo = a;
    if (algo == Algorithm::Algorithms::Szelessegi || algo == Algorithm::Algorithms::Prim)
        this->setMinimumHeight(520);
    else if (algo == Algorithm::Algorithms::Dijkstra)
        this->setMinimumHeight(550);
    else if (algo == Algorithm::Algorithms::Melysegi) {
        this->setMinimumHeight(670);
    }
}

void AlgorithmInfos::setStep(int s) { algorithmStep =  s; }

void AlgorithmInfos::setColorPart(AlgorithmInfos::InAlgorithm b)
{
    rowsToColor = b;
    update();
    qDebug() << "rowstocolor: " << rowsToColor;
}


void AlgorithmInfos::paintEvent(QPaintEvent *)
{
//    if (algo == Algorithm::Algorithms::None) return;

    QPainter painter(this);
    QFont font;
    font.setPixelSize(16);
    font.setBold(true);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(5,20, tr("Algoritmus"));
    int w = 45;
    int h = 30;
    switch (algo) {
    case Algorithm::None:
        return;
        break;
    case Algorithm::Szelessegi:
        h = paintSzelessegi(w,h,painter);
        break;
    case Algorithm::Melysegi:
        h = paintMelysegi(w,h, painter);
        break;
    case Algorithm::Dijkstra:
        h = paintDijkstra(w,h, painter);
        break;
    case Algorithm::Prim:
        h = paintPrim(w, h, painter);
        break;

    }
    w = 10;
    h += 30;
    painter.setFont(font);
    painter.drawText(5,h, tr("Színkódok"));
    paintColorHelp(w, h + 20, painter);
}

void AlgorithmInfos::paintColorHelp(int w, int h, QPainter &painter)
{
    QFont font;

    int n = 16;
    int e = 13;
    int t = 30;
    int h_orig = h;


    font.setPixelSize(14);
    font.setUnderline(true);
    painter.setFont(font);
    painter.drawText(w, h, tr("Csúcsok"));
    h+= 25;
    font.setUnderline(false);
    painter.setFont(font);

    painter.setBrush(NodeGraphics::normalNode);
    painter.drawEllipse(w,h-e, n,n);
    painter.drawText(w + t, h, tr("Felfedezetlen csúcs"));

    if (algo == Algorithm::Algorithms::Szelessegi || algo == Algorithm::Algorithms::Melysegi) {
        h += 20;
        painter.setBrush(NodeGraphics::grey);
        painter.drawEllipse(w, h-e, n, n);
        painter.drawText(w + t, h, tr("Megtalált, de még"));
        h+= 17;
        painter.drawText(w + t, h, tr("feldolgozatlan csúcs"));
    }

    h += 20;
    painter.setBrush(NodeGraphics::examineAdj);
    painter.drawEllipse(w, h-e, n,n);
    painter.drawText(w + t, h, tr("Kiterjesztés alatt álló csúcs"));

    h+= 20;
    painter.setBrush(NodeGraphics::examined);
    painter.drawEllipse(w, h-e, n, n);
    painter.drawText(w + t, h, tr("Vizsgált szomszéd"));

    h += 20;
    painter.setBrush(NodeGraphics::processed);
    painter.drawEllipse(w, h-e, n,n);
    painter.drawText(w + t, h, tr("Feldolgozott csúcs"));

//    h += 30;
    h = h_orig;
    w = w + 210;
    font.setUnderline(true);
    painter.setFont(font);

    painter.drawText(w, h, tr("Élek"));

    font.setUnderline(false);
    painter.setFont(font);

    h += 25;

    QPen pen;
    pen.setWidth(2);
    pen.setColor(EdgeGraphics::normal);
    painter.setPen(pen);

    painter.drawLine(w, h, w + 17, h - 10);
    painter.drawText(w + t, h, tr("Feldolgozatlan él"));

    h += 20;
    pen.setColor(EdgeGraphics::needed);
    painter.setPen(pen);
    painter.drawLine(w, h, w + 17, h-10);
    painter.setPen(Qt::black);
    painter.drawText(w + t, h, tr("Számolt feszítőfa része"));

    h+= 20;
    pen.setColor(EdgeGraphics::examined);
    painter.setPen(pen);
    painter.drawLine(w, h, w + 17, h-10);
    painter.setPen(Qt::black);
    painter.drawText(w + t, h, tr("Vizsgált él"));

    h += 20;
    pen.setColor(EdgeGraphics::notNeeded);
    painter.setPen(pen);
    painter.drawLine(w, h, w + 17, h-10);
    painter.setPen(Qt::black);
    painter.drawText(w + t, h, tr("Nem kell a feszítőfába"));
}

int AlgorithmInfos::paintSzelessegi(int w, int h, QPainter &painter)
{
    QFont font;
    int pixSize = 14;
    font.setPixelSize(pixSize);
    painter.setFont(font);
    painter.setPen(Qt::black);
    int width = 300;
    int rowHeight = 30;
    int colWidth = 25;
    int textMarginLeft = 15;
    double textVert = 0.7;
    QString str;

    QBrush brush = Qt::white;
    QBrush c_brush = QColor(247, 230, 203);
    painter.setBrush(brush);

    if (rowsToColor ==  Init) painter.setBrush(c_brush);
    // első ciklus
    painter.drawRect(w, h, width, 2 * rowHeight);
                    // forall                               in
    str = QString::fromUtf8("\u2200") + "u" + QString::fromUtf8("\u2208") + "G.V";
    painter.drawText(w + width / 2 - str.length()*pixSize / 2, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
                                // infinity                         pi                                          empty set
    str = "d(u) := " + QString::fromUtf8("\u221E") + "; " + QString::fromUtf8("\u03C0") + "(u) := " + QString::fromUtf8("\u2205");
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    // értékadások
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "d(s) := 0";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "Q: Queue; Q.add(s)";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight*6);
                // neg
    str = QString::fromUtf8("\u00AC") + "Q.isEmpty()";
    painter.drawText(w + width / 2 - 45, textVert * rowHeight + h, str);

    h += rowHeight;

    if (rowsToColor == Outer) painter.setBrush(c_brush);
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
    str = "u := Q.rem()";
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    painter.setBrush(brush);

    // belső ciklus
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight * 4);
    str = QString::fromUtf8("\u2200") + "v: (u,v)" + QString::fromUtf8("\u2208") + "G.E";
    painter.drawText(w + colWidth + (width-colWidth)/2 - 60, textVert * rowHeight + h, str);

    // elágazás
    h += rowHeight;
    painter.drawRect(w + 2*colWidth, h, width-2*colWidth, rowHeight);
    painter.drawLine(w + 2*colWidth, h, w + 2*colWidth + 10, h + rowHeight);
    painter.drawLine(w + width, h, w + width - 10, h + rowHeight);
    str = "d(v) = " + QString::fromUtf8("\u221E");
    painter.drawText(w + 2*colWidth + (width-2*colWidth) / 2 - 40, textVert * rowHeight + h, str);

    h += rowHeight;
    int w_l = (width-2*colWidth) * 0.75;


    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    painter.drawRect(w + 2*colWidth, h, w_l, rowHeight);
    str = "d(v) := d(u) + 1; " + QString::fromUtf8("\u03C0") + "(v) := u";
    painter.drawText(w + 2*colWidth + 4, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w + 2*colWidth, h, w_l,rowHeight);
    str = "Q.add(v)";
    painter.drawText(w + 2*colWidth + 4, textVert * rowHeight + h, str);

    painter.setBrush(brush);
    if (rowsToColor == IfFalse) painter.setBrush(c_brush);

    painter.drawRect(w + 2*colWidth + w_l, h-rowHeight, (width-2*colWidth)-w_l, 2*rowHeight);
    str = "SKIP";
    painter.drawText(w + 2* colWidth + w_l + textMarginLeft , h, str);

    return h + rowHeight;
}

int AlgorithmInfos::paintDijkstra(int w, int h, QPainter &painter)
{
    QFont font;
    int pixSize = 14;
    font.setPixelSize(pixSize);
    painter.setFont(font);
    painter.setPen(Qt::black);
    int width = 300;
    int rowHeight = 30;
    int colWidth = 25;
    int textMarginLeft = 15;
    double textVert = 0.7;
    QString str;

    QBrush brush = Qt::white;
    QBrush c_brush = QColor(247, 230, 203);
    painter.setBrush(brush);

    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    painter.drawRect(w, h, width, 2 * rowHeight);
                    // forall                               in
    str = QString::fromUtf8("\u2200") + "u" + QString::fromUtf8("\u2208") + "G.V";
    painter.drawText(w + width / 2 - str.length()*pixSize / 2, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
                                // infinity                         pi                                          empty set
    str = "d(u) := " + QString::fromUtf8("\u221E") + "; " + QString::fromUtf8("\u03C0") + "(u) := " + QString::fromUtf8("\u2205");
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    // értékadások
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "d(s) := 0";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "Q: minPrQ(G.V\{s}, d)";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "u := s";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight*7);
                                                            // neg
    str = "d(u) < " + QString::fromUtf8("\u221E") + QString::fromUtf8("\u00AC") + "Q.isEmpty()";
    painter.drawText(w + width / 2 - 70, textVert * rowHeight + h, str);


    // belső ciklus
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight * 5);
    str = QString::fromUtf8("\u2200") + "v: (u,v)" + QString::fromUtf8("\u2208") + "G.E";
    painter.drawText(w + colWidth + (width-colWidth)/2 - 60, textVert * rowHeight + h, str);

    // elágazás
    h += rowHeight;
    painter.drawRect(w + 2*colWidth, h, width-2*colWidth, rowHeight);
    painter.drawLine(w + 2*colWidth, h, w + 2*colWidth + 10, h + rowHeight);
    painter.drawLine(w + width, h, w + width - 10, h + rowHeight);
    str = "d(v) > d(u) + G.w(u,v)";
    painter.drawText(w + 2*colWidth + (width-2*colWidth) / 2 - 70, textVert * rowHeight + h, str);

    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    h += rowHeight;
    int w_l = (width-2*colWidth) * 0.75;
    painter.drawRect(w + 2*colWidth, h, w_l, 2 * rowHeight);
    str = "d(v) := d(u) + d(u)";
    painter.drawText(w + 2*colWidth + textMarginLeft, textVert * rowHeight + h, str);
    h += rowHeight;
    str = " + G.w(u,v); " + QString::fromUtf8("\u03C0") + "(v) := u";
    painter.drawText(w + 2*colWidth + textMarginLeft + 3, (textVert * rowHeight) * 0.75 + h, str);

    h+= rowHeight;
    painter.drawRect(w + 2*colWidth, h, w_l,rowHeight);
    str = "Q.adjust()";
    painter.drawText(w + 2*colWidth + textMarginLeft, textVert * rowHeight + h, str);

    painter.setBrush(brush);
    if (rowsToColor == IfFalse) painter.setBrush(c_brush);

    painter.drawRect(w + 2*colWidth + w_l, h-2*rowHeight, (width-2*colWidth)-w_l, 3*rowHeight);
    str = "SKIP";
    painter.drawText(w + 2* colWidth + w_l + textMarginLeft , h, str);

    painter.setBrush(brush);
    if (rowsToColor == Outer) painter.setBrush(c_brush);

    // második ciklus - utolsó értékadás
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
    str = "u := Q.remMin()";
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    return h + rowHeight;
}

int AlgorithmInfos::paintPrim(int w, int h, QPainter &painter)
{
    QFont font;
    int pixSize = 14;
    font.setPixelSize(pixSize);
    painter.setFont(font);
    painter.setPen(Qt::black);
    int width = 300;
    int rowHeight = 30;
    int colWidth = 25;
    int textMarginLeft = 15;
    double textVert = 0.7;
    QString str;

    QBrush brush = Qt::white;
    QBrush c_brush = QColor(247, 230, 203);
    painter.setBrush(brush);

    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    painter.drawRect(w, h, width, 2 * rowHeight);
                    // forall                               in
    str = QString::fromUtf8("\u2200") + "u" + QString::fromUtf8("\u2208") + "G.V";
    painter.drawText(w + width / 2 - str.length()*pixSize / 2, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
                                // infinity                         pi                                          empty set
    str = "d(u) := " + QString::fromUtf8("\u221E") + "; " + QString::fromUtf8("\u03C0") + "(u) := " + QString::fromUtf8("\u2205");
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    // értékadások
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "d(s) := 0";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "Q: minPrQ(G.V\{s}, d)";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "u := s";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight*6);
                                                            // neg
    str = QString::fromUtf8("\u00AC") + "Q.isEmpty()";
    painter.drawText(w + width / 2 - 45, textVert * rowHeight + h, str);


    // belső ciklus
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight * 4);
    str = QString::fromUtf8("\u2200") + "v: (u,v)" + QString::fromUtf8("\u2208") + "G.E";
    painter.drawText(w + colWidth + (width-colWidth)/2 - 60, textVert * rowHeight + h, str);

    // elágazás
    h += rowHeight;
    painter.drawRect(w + 2*colWidth, h, width-2*colWidth, rowHeight);
    painter.drawLine(w + 2*colWidth, h, w + 2*colWidth + 10, h + rowHeight);
    painter.drawLine(w + width, h, w + width - 10, h + rowHeight);
                                                                // logical and
    str = "v(e)" + QString::fromUtf8("\u2208") + "Q " + QString::fromUtf8("\u2227") +  "d(v) > G.w(u,v)";
    painter.drawText(w + 2*colWidth + (width-2*colWidth) / 2 - 85, textVert * rowHeight + h, str);

    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    h += rowHeight;
    int w_l = (width-2*colWidth) * 0.75;
    painter.drawRect(w + 2*colWidth, h, w_l, rowHeight);
    str = "d(v) := G.w(u,v); " + QString::fromUtf8("\u03C0") + "(v) := u";
    painter.drawText(w + 2*colWidth + 4, textVert * rowHeight + h, str);

    h+= rowHeight;
    painter.drawRect(w + 2*colWidth, h, w_l,rowHeight);
    str = "Q.adjust()";
    painter.drawText(w + 2*colWidth + 4, textVert * rowHeight + h, str);

    painter.setBrush(brush);
    if(rowsToColor == IfFalse) painter.setBrush(c_brush);

    painter.drawRect(w + 2*colWidth + w_l, h-rowHeight, (width-2*colWidth)-w_l, 2*rowHeight);
    str = "SKIP";
    painter.drawText(w + 2* colWidth + w_l + textMarginLeft , h, str);

    painter.setBrush(brush);
    if (rowsToColor == Outer) painter.setBrush(c_brush);

    // második ciklus - utolsó értékadás
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
    str = "u := Q.remMin()";
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    return h + rowHeight;
}

int AlgorithmInfos::paintMelysegi(int w, int h, QPainter &painter)
{
    QFont font;
    int pixSize = 14;
    font.setPixelSize(pixSize);
    painter.setFont(font);
    painter.setPen(Qt::black);
    int width = 300;
    int rowHeight = 30;
    int colWidth = 25;
    int textMarginLeft = 15;
    double textVert = 0.7;
    QString str;

    QBrush brush = Qt::white;
    QBrush c_brush = QColor(247, 230, 203);
    painter.setBrush(brush);

    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    painter.drawRect(w, h, width, 2 * rowHeight);
                    // forall                               in
    str = QString::fromUtf8("\u2200") + "u" + QString::fromUtf8("\u2208") + "G.V";
    painter.drawText(w + width / 2 - str.length()*pixSize / 2, textVert * rowHeight + h, str);
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);

    str = "color(u) := white";
    painter.drawText(w + colWidth + textMarginLeft, textVert * rowHeight + h, str);

    // értékadások
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "time := 0";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight*4);
    // forall                               in
    str = QString::fromUtf8("\u2200") + "r" + QString::fromUtf8("\u2208") + "G.V";
    painter.drawText(w + width / 2 - 45, textVert * rowHeight + h, str);

    // elágazás
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
    painter.drawLine(w + colWidth, h, w + colWidth + 10, h + rowHeight);
    painter.drawLine(w + width, h, w + width - 10, h + rowHeight);
    str = "color(r) = white";
    painter.drawText(w + 2*colWidth + (width-2*colWidth) / 2 - 70, textVert * rowHeight + h, str);

    h += rowHeight;

    int w_l = (width-colWidth) * 0.75;
    painter.drawRect(w + colWidth, h, w_l, rowHeight);
                // pi                                           empty set
    str = QString::fromUtf8("\u03C0") + "(v) := " + QString::fromUtf8("\u2205");
    painter.drawText(w + colWidth + 4, textVert * rowHeight + h, str);

    h+= rowHeight;
    painter.drawRect(w + colWidth, h, w_l,rowHeight);
    str = "DFSvisit(G, r, time)";
    painter.drawText(w + colWidth + 4, textVert * rowHeight + h, str);

//    painter.setBrush(brush);
//    if(rowsToColor == IfFalse) painter.setBrush(c_brush);

    painter.drawRect(w + colWidth + w_l, h-rowHeight, (width-colWidth)-w_l, 2*rowHeight);
    str = "SKIP";
    painter.drawText(w + colWidth + w_l + textMarginLeft , h, str);

    h += 2 * rowHeight;

//    h = orig_h - 15;
//    w = w + 320;

    // DFS VISIT

    painter.drawRoundedRect(w + 20, h, width - 40, rowHeight, 10,10);
    str = "DFSvisit(G:Graph, u:Vertice, &time:" + QString::fromUtf8("\u2115");
    painter.drawText(w + width/2 -115, textVert*rowHeight + h,  str);
    painter.drawLine(w + width/2, h + rowHeight, w + width/2, h + 1.5*rowHeight);

    h += rowHeight * 1.5;
    painter.drawRect(w, h, width, rowHeight * 4);
    str = "d(u) := ++time; color(u) := grey";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);

    // DFS visit ciklus
    h += rowHeight;
    painter.drawRect(w , h, width, rowHeight * 4);
    str = QString::fromUtf8("\u2200") + "v: (u,v)" + QString::fromUtf8("\u2208") + "G.E";
    painter.drawText(w + (width)/2 - 55, textVert * rowHeight + h, str);

    // elágazás
    h += rowHeight;
    painter.drawRect(w + colWidth, h, width-colWidth, rowHeight);
    painter.drawLine(w + colWidth, h, w + colWidth + 10, h + rowHeight);
    painter.drawLine(w + width, h, w + width - 10, h + rowHeight);
    str = "color(v) = white";
    painter.drawText(w + colWidth + (width-colWidth) / 2 - 60, textVert * rowHeight + h, str);

    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    h += rowHeight;
    w_l = (width-colWidth) * 0.75;
    painter.drawRect(w + colWidth, h, w_l, rowHeight);
    str = QString::fromUtf8("\u03C0") + "(v) := u";
    painter.drawText(w + colWidth + 4, textVert * rowHeight + h, str);

    h+= rowHeight;
    painter.drawRect(w + colWidth, h, w_l,rowHeight);
    str = "DFSvisit(G, v, time)";
    painter.drawText(w + colWidth + 4, textVert * rowHeight + h, str);

    painter.setBrush(brush);
    if(rowsToColor == IfFalse) painter.setBrush(c_brush);

    painter.drawRect(w + colWidth + w_l, h-rowHeight, (width-colWidth)-w_l, 2*rowHeight);
    str = "SKIP";
    painter.drawText(w + colWidth + w_l + textMarginLeft , h, str);

    painter.setBrush(brush);
    if (rowsToColor == Outer) painter.setBrush(c_brush);

    // második ciklus - utolsó értékadás
    h += rowHeight;
    painter.drawRect(w, h, width, rowHeight);
    str = "f(u) := ++time; color(u) := black";
    painter.drawText(w + textMarginLeft, textVert * rowHeight + h, str);


    return h + rowHeight;
}
