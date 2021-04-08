#include "algorithminfos.h"

#include <QPainter>
#include <QVBoxLayout>

//QString const AlgorithmInfos::pi = QString::fromUtf8("\u1d6d1");
QString const AlgorithmInfos::pi = QString::fromUtf8("\u03C0");
QString const AlgorithmInfos::element_of = QString::fromUtf8("\u2208");
QString const AlgorithmInfos::for_all = QString::fromUtf8("\u2200");
QString const AlgorithmInfos::infinity = QString::fromUtf8("\u221E");
QString const AlgorithmInfos::empty_set = QString::fromUtf8("\u2205");
QString const AlgorithmInfos::logical_and =  QString::fromUtf8("\u2227");
QString const AlgorithmInfos::neg = QString::fromUtf8("\u00AC");

AlgorithmInfos::AlgorithmInfos(QWidget *parent) : QWidget(parent)
{
    algo = Algorithm::Algorithms::None;
    this->setMinimumWidth(400);

    width = 300;
    rowHeight = 30;
    colWidth = 25;
    textMarginLeft = 12;
    textVert = 0.7;
    pixSize = 14;
    int pixSize = 14;
    font.setPixelSize(pixSize);
    brush = Qt::white;
    c_brush = QColor(247, 230, 203);

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
}


void AlgorithmInfos::paintEvent(QPaintEvent *)
{
//    if (algo == Algorithm::Algorithms::None) return;

    QPainter painter(this);
    QFont font_title;
    font_title.setPixelSize(16);
    font_title.setBold(true);
    painter.setFont(font_title);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawText(5,20, tr("Algoritmus"));
    int w = 45;
    int h = 30;

    painter.setFont(font);
    painter.setBrush(brush);
//    painter.setPen(Qt::black);

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
    painter.setFont(font_title);
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
    QString str;

    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    str = for_all + "u " + element_of + " G.V";
    paintLoop(painter,w, h, 0, 2, str);

    h += rowHeight;
    str = "d(u) := " + infinity + "; " + pi + "(u) := " + empty_set;
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);

    // értékadások
    h += rowHeight;
    str = "d(s) := 0";
    paintStatement(painter, w, h, 0, width, 1, str);
    h += rowHeight;
    str = "Q: Queue; Q.add(s)";
    paintStatement(painter, w, h, 0, width, 1, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    str = neg + "Q.isEmpty()";
    paintLoop(painter, w, h, 0, 6, str);

    h += rowHeight;

    if (rowsToColor == Outer) painter.setBrush(c_brush);
    str = "u := Q.rem()";
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);

    painter.setBrush(brush);

    // belső ciklus
    h += rowHeight;
    str = for_all + "v: (u,v) " + element_of + " G.E";
    paintLoop(painter, w, h, 1, 4, str);

    // elágazás
    QString cond = "d(v) = " + infinity;

    h += rowHeight;
    paintConditional(painter, w, h, 2, cond);

    int w_l = (width-2*colWidth) * 0.75;
    h += rowHeight;
    int h_orig = h;
    str = "SKIP";
    int w_f = w + w_l;
    int width_f = (width-2*colWidth)-w_l;
    if (rowsToColor == IfFalse) painter.setBrush(c_brush);
    paintStatement(painter, w_f, h, 2, width_f, 2, str);
    painter.setBrush(brush);

    if (rowsToColor == IfTrue)  painter.setBrush(c_brush);
    str = "d(v) := d(u) + 1; " + pi + "(v) := u";
    h = h_orig;

    paintStatement(painter, w, h, 2, w_l, 1, str);

    h += rowHeight;
    str = "Q.add(v)";
    paintStatement(painter, w, h, 2, w_l, 1, str);

    return h + rowHeight;
}

int AlgorithmInfos::paintDijkstra(int w, int h, QPainter &painter)
{

    QString str;

    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    str = for_all + "u " + element_of + " G.V";
    paintLoop(painter, w, h, 0, 2, str);
    h += rowHeight;
    str = "d(u) := " + infinity + "; " + pi + "(u) := " + empty_set;
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);

    // értékadások
    h += rowHeight;
    str = "d(s) := 0";
    paintStatement(painter, w, h, 0, width, 1, str);
    h += rowHeight;
    str = "Q: minPrQ(G.V\\{s}, d)";
    paintStatement(painter, w, h, 0, width, 1, str);
    h += rowHeight;
    str = "u := s";
    paintStatement(painter, w, h, 0, width, 1, str);

    painter.setBrush(brush);


    // második ciklus
    h += rowHeight;
    str = "d(u) < " + infinity + " " + logical_and + " " + neg + "Q.isEmpty()";
    paintLoop(painter, w, h, 0, 7, str);


    // belső ciklus
    h += rowHeight;
    str = for_all + "v: (u,v)" + element_of + "G.E";
    paintLoop(painter, w, h, 1, 5, str);

    // elágazás
    h += rowHeight;
    str = "d(v) > d(u) + G.w(u,v)";
    paintConditional(painter, w, h, 2, str);

    int w_l = (width-2*colWidth) * 0.75;
    h += rowHeight;
    int h_orig = h;
    str = "SKIP";
    int w_f = w + w_l;
    int width_f = (width-2*colWidth)-w_l;
    if (rowsToColor == IfFalse) painter.setBrush(c_brush);
    paintStatement(painter, w_f, h, 2, width_f, 3, str);

    painter.setBrush(brush);

    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    h = h_orig;
    str = "d(v) := d(u) + G.w(u,v); " + pi + "(v) := u";
    paintStatement(painter, w, h, 2, w_l, 1, str);

    h+= rowHeight;
    str = "Q.adjust()";
    paintStatement(painter, w, h, 2, w_l, 1, str);

    painter.setBrush(brush);


    // második ciklus - utolsó értékadás
    if (rowsToColor == Outer) painter.setBrush(c_brush);

    h += rowHeight;
    str = "u := Q.remMin()";
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);

    return h + rowHeight;
}

int AlgorithmInfos::paintPrim(int w, int h, QPainter &painter)
{

    QString str;


    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    str = for_all + "u " + element_of + " G.V";
    paintLoop(painter, w, h, 0, 2, str);
    h += rowHeight;
    str = "d(u) := " + infinity + "; " + pi + "(u) := " + empty_set;
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);


    // értékadások
    h += rowHeight;
    str = "d(s) := 0";
    paintStatement(painter, w, h, 0, width, 1, str);
    h += rowHeight;
    str = "Q: minPrQ(G.V\\{s}, d)";
    paintStatement(painter, w, h, 0, width, 1, str);
    h += rowHeight;
    str = "u := s";
    paintStatement(painter, w, h, 0, width, 1, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    str = neg + "Q.isEmpty()";
    paintLoop(painter, w, h, 0, 6, str);


    // belső ciklus
    h += rowHeight;
    str = for_all + "v: (u,v) " + element_of + " G.E";
    paintLoop(painter, w, h, 1, 4, str);

    // elágazás
    h += rowHeight;
    str = "v" + element_of + "Q " +logical_and +  " d(v) > G.w(u,v)";
    paintConditional(painter, w, h, 2, str);


    int w_l = (width-2*colWidth) * 0.75;
    h += rowHeight;
    int h_orig = h;
    str = "SKIP";
    int w_f = w + w_l;
    int width_f = (width-2*colWidth)-w_l;
    if (rowsToColor == IfFalse) painter.setBrush(c_brush);
    paintStatement(painter, w_f, h, 2, width_f, 2, str);

    painter.setBrush(brush);


    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    h = h_orig;
    str = "d(v) := G.w(u,v); " + pi + "(v) := u";
    paintStatement(painter, w, h, 2, w_l, 1, str);

    h+= rowHeight;
    str = "Q.adjust()";
    paintStatement(painter, w, h, 2, w_l, 1, str);

    painter.setBrush(brush);

    // második ciklus - utolsó értékadás
    if (rowsToColor == Outer) painter.setBrush(c_brush);
    h += rowHeight;
    str = "u := Q.remMin()";
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);

    return h + rowHeight;
}

int AlgorithmInfos::paintMelysegi(int w, int h, QPainter &painter)
{

    QString str;


    if (rowsToColor ==  Init) painter.setBrush(c_brush);

    // első ciklus
    str = for_all + "u " + element_of + " G.V";
    paintLoop(painter, w, h, 0, 2, str);

    h += rowHeight;
    str = "color(u) := white";
    paintStatement(painter, w, h, 1, width-colWidth, 1, str);

    // értékadások
    h += rowHeight;
    str = "time := 0";
    paintStatement(painter, w, h, 0, width, 1, str);

    painter.setBrush(brush);

    // második ciklus
    h += rowHeight;
    str = for_all + "r " + element_of + " G.V";
    paintLoop(painter, w, h, 0, 4, str);

    if (rowsToColor == Outer) painter.setBrush(c_brush);
    // elágazás
    h += rowHeight;
    str = "color(r) = white";
    paintConditional(painter, w, h, 1, str);

    int w_l = (width-colWidth) * 0.75;
    h += rowHeight;
    int h_orig = h;
    str = "SKIP";
    int w_f = w + w_l;
    int width_f = (width-colWidth)-w_l;

    paintStatement(painter, w_f, h, 1, width_f, 2, str);


    h = h_orig;
    str = pi + "(r) := " + empty_set;
    paintStatement(painter, w, h, 1, w_l, 1, str);
    h+= rowHeight;
    str = "DFSvisit(G, r, time)";
    paintStatement(painter, w, h, 1, w_l, 1, str);

    painter.setBrush(brush);


    h += 2 * rowHeight;


    // DFS VISIT

    painter.drawRoundedRect(w + 20, h, width - 40, rowHeight, 10,10);
    str = "DFSvisit(G:Graph, u:Vertex, &time:" + QString::fromUtf8("\u2115");
    painter.drawText(w + width/2 -115, textVert*rowHeight + h,  str);
    painter.drawLine(w + width/2, h + rowHeight, w + width/2, h + 1.5*rowHeight);

    if (rowsToColor == MelysegiFirst) painter.setBrush(c_brush);

    h += rowHeight * 1.5;
    str = "d(u) := ++time; color(u) := grey";
    paintStatement(painter, w, h, 0, width, 1, str);

    painter.setBrush(brush);

    // DFS visit ciklus

    h += rowHeight;
    str = for_all + "v: (u,v)" + element_of + "G.E";
    paintLoop(painter, w, h, 0, 4, str);


    // elágazás
    h += rowHeight;
    str = "color(v) = white";
    paintConditional(painter, w, h, 1, str);

    w_l = (width-colWidth) * 0.75;
    h += rowHeight;
    h_orig = h;
    str = "SKIP";
    w_f = w + w_l;
    width_f = (width-colWidth)-w_l;
    if (rowsToColor == IfFalse) painter.setBrush(c_brush);
    paintStatement(painter, w_f, h, 1, width_f, 2, str);

    painter.setBrush(brush);


    if (rowsToColor == IfTrue) painter.setBrush(c_brush);

    h = h_orig;
    str = pi + "(v) := u";
    paintStatement(painter, w, h, 1, w_l, 1, str);

    h+= rowHeight;
    str = "DFSvisit(G, v, time)";
    paintStatement(painter, w, h, 1, w_l, 1, str);

    painter.setBrush(brush);

    // második ciklus - utolsó értékadás
    if (rowsToColor == MelysegiLast) painter.setBrush(c_brush);
    h += rowHeight;
    str = "f(u) := ++time; color(u) := black";
    paintStatement(painter, w, h, 0, width, 1, str);

    return h + rowHeight;
}

void AlgorithmInfos::paintStatement(QPainter &painter, int &w, int &h, int indent, int width, int row_count, QString s)
{
    QFontMetrics fm(font);
    QStringList rows;
    rows.append(s);
    while (fm.horizontalAdvance(rows.last()) > (width - textMarginLeft - 1)) {        // amíg nem fér bele a megadott dobozba új sorokba töredljük
        int ind = rows.length()-1;
        QStringList new_rows = rows.at(ind).split(" ");
        rows.removeLast();
        while(!new_rows.isEmpty()) {
            QString new_row = "";

            while (!new_rows.isEmpty() &&
                   (fm.horizontalAdvance(new_row + new_rows.first() + QString(" ")) < width - textMarginLeft - 1
                                || new_rows.length() == 1)) {      // a tördelt szavakból maximális hosszúságú sorokat képezünk
                new_row += new_rows.takeFirst() + " ";
            }
            rows.append(new_row);
        }
    }
    painter.drawRect(w + indent*colWidth, h, width, rows.length() * rowHeight * row_count);
    double i = 1.0;// értékadás doboza
    foreach(QString str, rows) {
        painter.drawText(w + indent*colWidth + textMarginLeft, textVert * rowHeight * i + h, str);
        i = 0.75;
        h += rowHeight;
    }
    h -= rowHeight;
}

void AlgorithmInfos::paintLoop(QPainter &painter, int &w, int &h, int indent, int rows_num, QString condition)
{
    QFontMetrics fm(font);
    painter.drawRect(w + indent*colWidth, h, width - indent*colWidth, rowHeight * rows_num);            // akkora magasságú doboz, ahány sort tartalmaz a ciklus (feltétellel együtt)
    painter.drawText(w + indent*colWidth + (width-indent*colWidth)/2 - fm.horizontalAdvance(condition)/2, textVert * rowHeight + h, condition);    // feltétel szövege

}

void AlgorithmInfos::paintConditional(QPainter &painter, int &w, int &h, int indent, QString condition/*, QStringList trueStatements, QStringList falseStatements, bool paintTrue, bool paintFalse*/)
{
    QFontMetrics fm(font);
    painter.drawRect(w + indent*colWidth, h, width - indent*colWidth, rowHeight);                   // feltétel doboza
    painter.drawLine(w + indent*colWidth, h , w + indent*colWidth + 10, h + rowHeight);             // feltételt jelző ferde vonalak
    painter.drawLine(w + width, h, w + width-10, h + rowHeight);
    painter.drawText(w + indent*colWidth + (width-indent*colWidth)/2 - fm.horizontalAdvance(condition)/2, textVert * rowHeight + h, condition);     // feltétel szövege
}

int AlgorithmInfos::findMaxLengthString(QStringList strs)
{
    QFontMetrics fm(font);
    int max = 0;
    foreach(QString str, strs) {
        if (fm.horizontalAdvance(str) > max) max = fm.horizontalAdvance(str);
    }
    return max;
}
