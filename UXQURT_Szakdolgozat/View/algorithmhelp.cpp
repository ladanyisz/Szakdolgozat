#include "algorithmhelp.h"

QString const AlgorithmHelp::pi = QString::fromUtf8("\u03C0");
bool AlgorithmHelp::SzelessegiWindow = true;
bool AlgorithmHelp::MelysegiWindow = true;
bool AlgorithmHelp::PrimWindow = true;
bool AlgorithmHelp::DijkstraWindow = true;

AlgorithmHelp::AlgorithmHelp(Algorithm::Algorithms a, QWidget *parent) : QWidget(parent)
{

    this->a = a;

    setWindowTitle("Algoritmus segítség");
    layout = new QVBoxLayout();
    setLayout(layout);
    layout->setAlignment(Qt::AlignTop);

    titleFont.setPixelSize(18);
    titleFont.setBold(true);

    normalFont.setPixelSize(14);

    label = new QLabel();
    label->setFont(titleFont);
    layout->addWidget(label);

    data_layout = new QVBoxLayout();
    data_layout->setMargin(6);

    graphLabel = new QLabel();
    graphLabel->setTextFormat(Qt::RichText);
    graphLabel->setFont(normalFont);
    graphLabel->setWordWrap(true);
    graphLabel->setMinimumSize(1, 1);
    graphLabel->setAlignment(Qt::AlignJustify);

    data_layout->addWidget(graphLabel);

    stukiLabel = new QLabel();

    data_layout->addWidget(stukiLabel);
    layout->addLayout(data_layout);

    switch (a) {
    case Algorithm::None:
        close();
        break;
    case Algorithm::Szelessegi:
        createSzelessegi();
        break;
    case Algorithm::Melysegi:
        createMelysegi();
        break;
    case Algorithm::Dijkstra:
        createDijkstra();
        break;
    case Algorithm::Prim:
        createPrim();
        break;

    }

    setAttribute(Qt::WA_DeleteOnClose);

}

void AlgorithmHelp::createSzelessegi()
{
    SzelessegiWindow = false;
    label->setText("Szélességi gráfkeresés");
    label->setFont(titleFont);

    graphLabel->setText("A szélességi gráfkeresést  <b>élsúlyozatlan</b> gráfokon alkalmazhatjuk, "
                        "melyek <b>irányítottak</b> és <b>irányítatlanok</b> is lehetnek.<br/>"
                        "Az algoritmus megadja egy meghatározott startcsúcsból a gráf összes többi csúcsába vezető utakat és azok hosszát.<br/><br/>"
                        "Az egyes csúcsokhoz vezető utak hosszát <i>d</i>, míg a szülőcsúcsokat <i>"+pi+"</i> adja meg a kapott feszítőfában.");


    QPixmap pixmap(":/img/szelessegi.PNG");
    stukiLabel->setPixmap(pixmap);

}

void AlgorithmHelp::createMelysegi()
{
    MelysegiWindow = false;
    label->setText("Mélységi gráfkeresés");

    graphLabel->setText("A mélységi gráfkeresés <b>irányított</b>, <b>élsúlyozatlan</b> gráfokon alkalmazható.<br/>"
                        "Az algoritmus futtatásával mélységi feszítőerdőt kapunk.<br/><br/>"
                        "Egy csúcs felfedezésének idejét (discovery time) <i>d</i>, a befejezési idejét (finishing time) <i>f</i>, míg a szülőcsúcsokat a feszítőerdőben <i>"+pi+"</i> jelöli.<br/><br/>"
                        "Éltípusok: egy (u,v) " + QString::fromUtf8("\u2208") + "G.E él<br/>"
                        " - fa-él, ha valamelyik mélységi fa éle,<br/>"
                        " - vissza-él, ha v u őse egy mélységi fában,<br/>"
                        " - előre-él, ha (u,v) nem faél, de u v őse egy mélységi fában,<br/>"
                        " - kereszt-él, ha u és v ugyanannak a mélységi fának két különböző ágán vannak, vagy két különböző mélységi fában vannak.");


    QPixmap pixmap(":/img/melysegi.PNG");
    stukiLabel->setPixmap(pixmap);

}

void AlgorithmHelp::createPrim()
{
    PrimWindow = false;
    label->setText("Prim algoritmus");

    graphLabel->setText("Az algoritmus <b>összefüggő</b>, <b>élsúlyozott</b>, <b>irányítalan</b> gráfokon futtatható.<br/>"
                        "Az algoritmus futtatása után a gráf egy minimális feszítőfáját kapjuk meg.<br/>"
                        "A megadott kezdőcsúcs a kezdeti minimális feszítőfa."
                        " Az algoritmus minden lépésben egy új biztonságos él és a hozzá tartozó csúcs hozzávételével növeli az addigi minimális feszítőfát, hogy továbbra is minimális feszítőfát kapjunk.<br/><br/>"
                        "A <i>d</i> címke meghatározza, hogy az adott él milyen költséggel került a feszítőfába, <i>"+pi+"</i> pedig a szülőket jelöli ki a feszítőfában.");


    QPixmap pixmap(":/img/prim.PNG");
    stukiLabel->setPixmap(pixmap);
}

void AlgorithmHelp::createDijkstra()
{
    DijkstraWindow = false;
    label->setText("Dijkstra algoritmus");

    graphLabel->setText("Az algoritmus olyan <b>élsúlyozott</b> gráfon futtatható, melynek minden súlya nemnegatív. A gráf lehet <b>irányítalan</b> és <b>irányított</b> is.<br/>"
                        "Az algoritmus a legrövidebb utakat határozza meg egy megadott kezdőcsúcsból.<br/><br/>"
                        "<i>d</i> a számolt távolságokat, <i>"+pi+"</i> az utakban a szülőket adja meg.");


    QPixmap pixmap(":/img/dijkstra.png");
    stukiLabel->setPixmap(pixmap);
}

void AlgorithmHelp::closeEvent(QCloseEvent *event)
{
    switch (a) {
    case Algorithm::None:
        break;
    case Algorithm::Szelessegi:
        SzelessegiWindow = true;
        break;
    case Algorithm::Melysegi:
        MelysegiWindow = true;
        break;
    case Algorithm::Dijkstra:
        DijkstraWindow = true;
        break;
    case Algorithm::Prim:
        PrimWindow = true;
        break;

    }
    QWidget::closeEvent(event);
}
