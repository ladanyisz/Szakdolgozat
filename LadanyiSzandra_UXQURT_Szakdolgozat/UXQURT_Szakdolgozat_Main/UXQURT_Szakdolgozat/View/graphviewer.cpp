#include "graphviewer.h"
//#include "ui_graphviewer.h"

#include <QSizePolicy>
#include <QTimer>
#include <QMatrix>
#include <QGraphicsItem>
#include<QMenuBar>
#include <QCheckBox>
#include <QDir>
#include <QItemSelectionModel>
#include <QShortcut>
#include <QMessageBox>

GraphViewer::GraphViewer(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Gráfok");

    graph = new Graph();
    algo = new Algorithm(graph);

    path = "";

    initViews();
    initAlgoViews();
    initMenu();
    initEditToolbar();
    initAlgorithmToolbar();
    initWarningLabel();
    initWeightGroup();


    pointerButton->click();

    connect(&timer, &QTimer::timeout, this, [=]() {warningLabel->setHidden(true);});

    connect(graph, &Graph::nodesFull,this, &GraphViewer::nodesFull);
    connect(graph, &Graph::newEdge, scene, &GraphScene::addNewEdge);

    algoConnections();

    connect(scene, &GraphScene::edgeSelected, this, &GraphViewer::showWeightGroup);
    connect(scene, &GraphScene::nodeAdded, this, &GraphViewer::enableAlgorithms);
    connect(scene, &GraphScene::allNodesDeleted, this, &GraphViewer::disableAlgorithms);
    connect(scene, &GraphScene::nodesChanged, this, &GraphViewer::enableAlgorithms);
    setStyles();

}


// SLOTS


void GraphViewer::deleteGraph()
{
    graph->deleteAll();
    disableAlgorithms();
}

// algoritmus kezelése

void GraphViewer::algorithmSelected(QString selectedAlgorithm)
{
    nodeSelector->setEnabled(true);
    Algorithm::Algorithms selectedAlgo = Algorithm::Algorithms::None;
    if (selectedAlgorithm == "Szélességi bejárás")
        selectedAlgo = Algorithm::Algorithms::Szelessegi;
    else if (selectedAlgorithm == "Mélységi bejárás") {
        selectedAlgo = Algorithm::Algorithms::Melysegi;
        nodeSelector->setEnabled(false);
    }
    else if (selectedAlgorithm == "Prim-algoritmus")
        selectedAlgo = Algorithm::Algorithms::Prim;
    else if (selectedAlgorithm == "Dijkstra-algoritmus")
        selectedAlgo = Algorithm::Algorithms::Dijkstra;
    algo->selectAlgorithm(selectedAlgo);
}

void GraphViewer::nodeSelected(int selectedIndex)
{

    if (selectedIndex != -1) algo->selectStartNode(selectedIndex);
}

void GraphViewer::algorithmStopped()
{
    enableEdit();
    enableAlgorithms();
    algo->reset();
    previousButton->setEnabled(false);
    pauseButton->setEnabled(false);

    algoInfos->setVisible(false);

    while (algoValues->layout()->count() > 0) {
        QLayoutItem* i = algoValues->itemAt(0);
        if (i->widget()) {
            algoValues->removeWidget(i->widget());
            delete i->widget();
        }
    }
    if (queue->isVisible()) view->resize(view->width(), view->height() + 92);
    queue->setVisible(false);
    view->resize(width()-18, view->height());
    updateSceneRect();
}

void GraphViewer::algorithmStarted()
{
    disableEdit();
    disableSelectors();
    pointerButton->click();
    algo->startAlgorithm();
    if (algo->getInitState()) {
        playButton->setEnabled(false);
        pauseButton->setEnabled(true);
        previousButton->setEnabled(true);
        algoInfos->setAlgorithm(algo->getChosenAlgo());
        algoInfos->setVisible(true);
        view->resize(view->width(), height()-162);
        updateSceneRect();
    } else {
        pauseButton->setEnabled(true);
        pauseButton->click();
        enableEdit();
        enableSelectors();
    }

}

void GraphViewer::algorithmPaused()
{
    algo->pauseAlgrotithm();
    playButton->setEnabled(true);
    pauseButton->setEnabled(false);
}

void GraphViewer::nextPressed()
{
    disableEdit();
    disableSelectors();
    bool not_first = algo->getInitState();
    bool step_success = algo->stepAlgorithm();
    if (algo->getInitState()) {
        if (!step_success) nextButton->setEnabled(false);
        if (!previousButton->isEnabled()) previousButton->setEnabled(true);
        algoInfos->setAlgorithm(algo->getChosenAlgo());
        algoInfos->setVisible(true);
        if (!not_first) {
            view->resize(view->width(), height() - 162);
            updateSceneRect();
        }
    } else {
        enableEdit();
        enableSelectors();
    }
}

void GraphViewer::previousPressed()
{
    if (!algo->stepBackAlgorithm()) previousButton->setEnabled(false);
    if (!nextButton->isEnabled()) nextButton->setEnabled(true);
}


// szerkesztő-elemek megjelenítése / elrejtése

void GraphViewer::showGraphTextEditor()
{
    graphTextEditor->initEditor();
    graphTextEditor->setHidden(false);
    disableEdit();
    disableAlgorithms();
    setupGraphAction->setEnabled(false);
    updateSceneRect();
}

void GraphViewer::hideGraphTextEditor()
{
    graphTextEditor->setHidden(true);
    enableEdit();
    setupGraphAction->setEnabled(true);
    if (graph->getSize() >= 1) enableAlgorithms();
    view->resize(width()-18, view->height());
    updateSceneRect();
}

void GraphViewer::showWeightGroup()
{
    weightLineEdit->setText("");
    weightLineEdit->setFocus();
    weightGroupBox->setVisible(true);
}


// mentés-betöltés

void GraphViewer::saveFile()
{
    QString dir;
    if (path == "")
        dir = QDir::rootPath();
    else
        dir = path;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Gráf mentése"), dir, tr("Gráfok (*.graph)"));

    if (fileName != "" && fileName != QString()) {

        if (QFileInfo(fileName).suffix() != "graph") {
            QMessageBox::warning(this, "Gráf mentése", "Helytelen kiterjesztés miatt a fájl mentése sikertelen.");
            return;
        }

        path = QFileInfo(fileName).path();
        QVector<QPointF> positions = scene->nodePositions();
        if (!graph->saveGraph(fileName, positions)) QMessageBox::warning(this, "Gráf mentése", "A gráf mentése sikertelen!");
    }
}

void GraphViewer::openFile()
{
    QString dir;
    if (path == "")
        dir = QDir::rootPath();
    else
        dir = path;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Gráf betöltése"), dir, tr("Gráfok (*.graph)"));
    if (fileName != "" && fileName != QString()) {

        if (QFileInfo(fileName).suffix() != "graph") {
            QMessageBox::warning(this, "Gráf betöltése", "Helytelen kiterjesztés miatt a fájl megnyitása sikertelen.");
            return;
        }

        path = QFileInfo(fileName).path();
        QVector<std::tuple<int, QChar, QPointF>> nodes_data = graph->loadGraph(fileName);

        if (nodes_data.isEmpty()) {
            QMessageBox::warning(this, "Gráf betöltése", "Gráf betöltése sikertelen!");
            return;
        }

        for(int i=0; i<graph->getSize(); i++) {
            std::tuple<int, QChar, QPointF> node_data = nodes_data.at(i);
            qreal w = scene->sceneRect().width();
            qreal h = scene->sceneRect().height();
            QPointF point;
            point.setX(std::get<2>(node_data).x()*w);
            point.setY(std::get<2>(node_data).y()*h);
            scene->addNode(std::get<0>(node_data), std::get<1>(node_data), point);
        }
        for(int i=0; i<graph->getSize(); i++) {
            for(int j=0; j<graph->getAdjNum(i); j++) {
                scene->addNewEdge(graph->getName(graph->getId(i)), graph->getAdjName(i,j), graph->getAdjWeight(i,j));
            }
        }
        if (graph->getDirected() != directedCheckBox->isChecked()) directedCheckBox->click();
        if (graph->getWeighted() != weightedCheckBox->isChecked()) weightedCheckBox->click();
        algo->reset();
        if (graph->getSize() > 0) enableAlgorithms();
        algorithmStopped();
        pointerButton->click();
    }
}


// eszközsáv elemeinek engedélyezése / letiltása

void GraphViewer::enableAlgorithms()
{
    enableSelectors();
    playButton->setEnabled(true);
    nextButton->setEnabled(true);
    stopButton->setEnabled(true);

    updateNodeSelector();

    algorithmSelected(algorithmSelector->currentText());
    nodeSelected(nodeSelector->currentIndex());
}

void GraphViewer::disableAlgorithms()
{
    disableSelectors();
    playButton->setEnabled(false);
    pauseButton->setEnabled(false);
    previousButton->setEnabled(false);
    nextButton->setEnabled(false);
    stopButton->setEnabled(false);
}

void GraphViewer::enableSelectors()
{
    nodeSelector->setEnabled(true);
    algorithmSelector->setEnabled(true);
}

void GraphViewer::disableSelectors()
{
    nodeSelector->setEnabled(false);
    algorithmSelector->setEnabled(false);
}

void GraphViewer::enableEdit()
{
    nodeButton->setEnabled(true);
    edgeButton->setEnabled(true);
    if (graph->getWeighted()) weightButton->setEnabled(true);
    deleteButton->setEnabled(true);

    deleteGraphAction->setEnabled(true);
    directedCheckBox->setEnabled(true);
    weightedCheckBox->setEnabled(true);
    setupGraphAction->setEnabled(true);
}

void GraphViewer::disableEdit()
{
    nodeButton->setEnabled(false);
    edgeButton->setEnabled(false);
    weightButton->setEnabled(false);
    deleteButton->setEnabled(false);
    deleteGraphAction->setEnabled(false);
    directedCheckBox->setEnabled(false);
    weightedCheckBox->setEnabled(false);
    setupGraphAction->setEnabled(false);

    weightLineEdit->setText("");
    weightGroupBox->setVisible(false);
    pointerButton->click();
}


// figyelmeztetések

void GraphViewer::nodesFull()
{
    warningLabel->setText("Elérted a maximálisan elhelyezhető csúcsok számát! (" +  QString::number(Graph::getMaxNodeNum()) + ")");
    showWarningLabel();
}

void GraphViewer::edgesFull()
{
    warningLabel->setText("Nincs több lehetséges él!");
    showWarningLabel();
}

void GraphViewer::needWeights()
{
    warningLabel->setText("Az algoritmus csak élsúlyozott gráfon futtatható!");
    showWarningLabel();
}

void GraphViewer::noWeights()
{
    warningLabel->setText("Az algoritmus csak élsúlyozatlan gráfon futtatható!");
    showWarningLabel();
}

void GraphViewer::needOnlyNonnegativeEdges()
{
    warningLabel->setText("Az algoritmus csak nemnegatív élköltségű gráfon futtatható!");
    showWarningLabel();
}

void GraphViewer::needsToBeConnected()
{
    warningLabel->setText("Az algoritmus csak összefüggő gráfon futtatható!");
    showWarningLabel();
}

void GraphViewer::needToBeDirected()
{
    warningLabel->setText("Az algoritmus csak irányított gráfon futtatható!");
    showWarningLabel();
}

void GraphViewer::needsToBeUndirected()
{
    warningLabel->setText("Az algoritmus csak irányítatlan gráfon futtatható!");
    showWarningLabel();
}


// algoritmus változásai

void GraphViewer::algoInitReady(int ind)
{
    if (ind == -1) return;
    int w = 30;
    queue->setVisible(true);
    if (algo->getChosenAlgo() != Algorithm::Algorithms::Melysegi){

        QLabel* d = new QLabel("d");
        d->setFixedWidth(w);
        d->setFont(tableFont);
        algoValues->addWidget(d, 1,0, Qt::AlignLeft);

    } else {
        queue->setText("");

        QLabel* df = new QLabel("d/f");
        df->setFixedWidth(60);
        df->setFont(tableFont);
        algoValues->addWidget(df, 1,0, Qt::AlignLeft);
    }

    QLabel* p = new QLabel();
    QPixmap pixmap(":/img/pi.png");

    p->setPixmap(pixmap.scaledToHeight(18));
    p->setFixedWidth(w);
    p->setFont(tableFont);
    algoValues->addWidget(p, 2,0, Qt::AlignLeft);

    QStringList names = graph->getNames();
    names.sort();
    int i = 1;
    foreach(QString name, names) {
        QLabel* n = new QLabel(name);
        n->setFont(tableFont);
        n->setFixedWidth(w);
        n->setAlignment(Qt::AlignCenter);
        algoValues->addWidget(n, 0, i, Qt::AlignHCenter);

        int index = graph->getIndex(name);

        if (algo->getChosenAlgo() != Algorithm::Algorithms::Melysegi) {
            QLabel* d = new QLabel();
            d->setFont(tableDataFont);
            d->setFixedWidth(w);
            d->setAlignment(Qt::AlignCenter);
            if (index == ind) d->setText("0");
            else {
                QPixmap pmap(":/img/infinity.png");
                d->setPixmap(pmap);
            }
            algoValues->addWidget(d, 1, i, Qt::AlignHCenter);

        } else {

            QLabel* df = new QLabel();
            df->setFont(tableDataFont);
            df->setFixedWidth(60);
            df->setAlignment(Qt::AlignCenter);
            df->setText("0/0");
            algoValues->addWidget(df, 1, i, Qt::AlignHCenter);

        }

        QLabel* p = new QLabel();
        QPixmap pix(":/img/empty-set.png");
        p->setAlignment(Qt::AlignCenter);
        p->setPixmap(pix);
        p->setFixedWidth(w);
        p->setFont(tableDataFont);
        algoValues->addWidget(p, 2, i, Qt::AlignHCenter);
        i++;
    }


    algoValues->setSpacing(0);
    algoValues->setHorizontalSpacing(12);
    view->resize(view->width(), height() - 162);
    updateSceneRect();
}

void GraphViewer::parentChanged(int ind, QChar n)
{
    QChar name = graph->getName(graph->getId(ind));
    bool found = false;
    int i = 1;
    while (i < graph->getSize() + 1 && !found) {
        QLayoutItem* item = algoValues->itemAtPosition(0,i);
        QLabel* label = qobject_cast<QLabel*>(item->widget());
        if (label && label->text() == QString(name)) {
            found = true;
            QLayoutItem* item = algoValues->itemAtPosition(2,i);
            QLabel* label = qobject_cast<QLabel*>(item->widget());
            label->setStyleSheet("background-color: rgb(255, 174, 0);");
            if (n == QChar()) {
                QPixmap pix(":/img/empty-set.png");
                label->setPixmap(pix);
                label->setText("");
            } else
                label->setText(n);
        }
        i++;
    }
}

void GraphViewer::distChanged(int ind, int d)
{
    QChar name = graph->getName(graph->getId(ind));
    bool found = false;
    int i = 1;
    while (i < graph->getSize() + 1 && !found) {
        QLayoutItem* item = algoValues->itemAtPosition(0,i);
        QLabel* label = qobject_cast<QLabel*>(item->widget());
        if (label && label->text() == QString(name)) {
            found = true;
            QLayoutItem* item = algoValues->itemAtPosition(1,i);
            QLabel* label = qobject_cast<QLabel*>(item->widget());
            label->setStyleSheet("background-color: rgb(255, 174, 0);");
            if (d != INT32_MAX)
                label->setText(QString::number(d));
            else {
                QPixmap pmap(":/img/infinity.png");
                label->setPixmap(pmap);
                label->setText("");
            }
        }
        i++;
    }
}

void GraphViewer::queueChanged(QString q)
{
    queue->setText("Q - " + q);
}

void GraphViewer::discoveryFinishChanged(int ind, int d, int f)
{
    QChar name = graph->getName(graph->getId(ind));
    bool found = false;
    int i = 1;
    while (i < graph->getSize() + 1 && !found) {
        QLayoutItem* item = algoValues->itemAtPosition(0,i);
        QLabel* label = qobject_cast<QLabel*>(item->widget());
        if (label && label->text() == QString(name)) {
            found = true;
            QLayoutItem* item = algoValues->itemAtPosition(1,i);
            QLabel* label = qobject_cast<QLabel*>(item->widget());
            label->setStyleSheet("background-color: rgb(255, 174, 0);");
            label->setText(QString::number(d) + "/" + QString::number(f));
        }
        i++;
    }
}

void GraphViewer::showWarningLabel()
{
    warningLabel->setHidden(false);
    timer.setSingleShot(true);
    timer.start(5000);
}

void GraphViewer::clearColorsInAlgTable()
{
    for(int i=1; i<graph->getSize() + 1; i++) {
        for(int j=1; j<algoValues->rowCount(); j++) {
            QLayoutItem* item = algoValues->itemAtPosition(j,i);
            QLabel* label = qobject_cast<QLabel*>(item->widget());
            label->setStyleSheet("background-color: none;");
        }
    }

}

// ---------------


// PRIVATE FUNCTIONS


void GraphViewer::algoConnections()
{
    connect(algo, &Algorithm::nodeStateChange, scene, &GraphScene::changeNodeState);
    connect(algo, &Algorithm::edgeStateChange, scene, &GraphScene::changeEdgeState);
    connect(algo, &Algorithm::algorithmEnded, this, [=]() {
        pauseButton->click();
        nextButton->setEnabled(false);
        clearColorsInAlgTable();
    });
    connect(algo, &Algorithm::needWeights, this, &GraphViewer::needWeights);
    connect(algo, &Algorithm::needOnlyNonnegativeEdges, this, &GraphViewer::needOnlyNonnegativeEdges);
    connect(algo, &Algorithm::noWeights, this, &GraphViewer::noWeights);
    connect(algo, &Algorithm::needsToBeConnected, this, &GraphViewer::needsToBeConnected);
    connect(algo, &Algorithm::needsToBeUndirected, this, &GraphViewer::needsToBeUndirected);
    connect(algo, &Algorithm::needsToBeDirected, this, &GraphViewer::needToBeDirected);
    connect(algo, &Algorithm::initReady, this, &GraphViewer::algoInitReady);
    connect(algo, &Algorithm::initReady, algoInfos, [=]() {algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::Init); });
    connect(algo, &Algorithm::distChanged, this, &GraphViewer::distChanged);
    connect(algo, &Algorithm::parentChanged, this, &GraphViewer::parentChanged);
    connect(algo, &Algorithm::queueChanged, this, &GraphViewer::queueChanged);
    connect(algo, &Algorithm::discoveryFinishChanged, this, &GraphViewer::discoveryFinishChanged);
    connect(algo, &Algorithm::step_start, this, &GraphViewer::clearColorsInAlgTable);
    connect(algo, &Algorithm::outerLoop, algoInfos, [=]() { algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::Outer); });
    connect(algo, &Algorithm::ifTrue, algoInfos, [=]() { algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::IfTrue); });
    connect(algo, &Algorithm::ifFalse, algoInfos, [=]() { algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::IfFalse); });
    connect(algo, &Algorithm::melysegiVisitFirst, algoInfos, [=]() { algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::MelysegiFirst); });
    connect(algo, &Algorithm::melysegiVisitLast, algoInfos, [=]() { algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::MelysegiLast); });
    connect(algo, &Algorithm::algorithmEnded, algoInfos, [=]() { algoInfos->setColorPart(AlgorithmInfos::InAlgorithm::None); });
}

void GraphViewer::setStyles()
{
    this->setStyleSheet(
                "QComboBox {"
                         "font-size: 14px;"
                         "padding: 2px 4px;"
                        "}"
                        "QCombobox::drop-down {padding: 56px;}");

    nodeSelector->setStyleSheet("margin-right: 2px; width: 10px;");

    warningLabel->setStyleSheet("font-size: 16px; color: red; background-color: rgba(255,255,255,0.7);");
    weightGroupBox->setStyleSheet("background-color: white; border: 1px solid black;");
    weightLabel->setStyleSheet("border: unset;");
}

void GraphViewer::initMenu()
{
    fileMenu = menuBar()->addMenu(tr("&Fájl"));
    saveFileAction = new QAction(QIcon(":/img/save.png"),tr("Mentés"));
    saveFileAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveFileAction);
    connect(saveFileAction, &QAction::triggered, this, &GraphViewer::saveFile);
    openFileAction = new QAction(QIcon(":/img/open.png"), tr("Megnyitás"));
    openFileAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openFileAction);
    connect(openFileAction, &QAction::triggered, this, &GraphViewer::openFile);
    deleteGraphAction = new QAction(tr("Gráf törlése"));

    menuBar()->addAction(deleteGraphAction);
    connect(deleteGraphAction, &QAction::triggered, this, &GraphViewer::deleteGraph);

    setupGraphAction = new QAction(tr("Szöveges megadás"));
    menuBar()->addAction(setupGraphAction);
    connect(setupGraphAction, &QAction::triggered, this, &GraphViewer::showGraphTextEditor);

    resetLayout = new QAction(tr("Rendezés"));
    menuBar()->addAction(resetLayout);
    connect(resetLayout, &QAction::triggered, scene, &GraphScene::resetNodeLayout);

    QMenu* algoHelpMenu = menuBar()->addMenu("&Algoritmus segítség");
    szelessegiHelpAction = new QAction(tr("Szélességi gráfkeresés"));
    algoHelpMenu->addAction(szelessegiHelpAction);
    connect(szelessegiHelpAction, &QAction::triggered, this, [=]() {
        if (AlgorithmHelp::SzelessegiWindow) {
            AlgorithmHelp* a = new AlgorithmHelp(Algorithm::Algorithms::Szelessegi);
            a->show();
        }
    });
    melysegiHelpAction = new QAction(tr("Mélységi gráfkeresés"));
    algoHelpMenu->addAction(melysegiHelpAction);
    connect(melysegiHelpAction, &QAction::triggered, this, [=]() {
        if (AlgorithmHelp::MelysegiWindow) {
            AlgorithmHelp* a = new AlgorithmHelp(Algorithm::Algorithms::Melysegi);
            a->show();
        }
    });
    primHelpAction = new QAction(tr("Prim algoritmus"));
    algoHelpMenu->addAction(primHelpAction);
    connect(primHelpAction, &QAction::triggered, this, [=]() {
        if (AlgorithmHelp::PrimWindow) {
            AlgorithmHelp* a = new AlgorithmHelp(Algorithm::Algorithms::Prim);
            a->show();
        }
    });
    dijkstraHelpAction = new QAction(tr("Dijkstra algoritmus"));
    algoHelpMenu->addAction(dijkstraHelpAction);
    connect(dijkstraHelpAction, &QAction::triggered, this, [=]() {
        if (AlgorithmHelp::DijkstraWindow) {
            AlgorithmHelp* a = new AlgorithmHelp(Algorithm::Algorithms::Dijkstra);
            a->show();
        }
    });

    aboutMenu = new QAction(tr("Névjegy"));
    menuBar()->addAction(aboutMenu);

    connect(aboutMenu, &QAction::triggered, this, [=]() {
        QMessageBox::information(this, "Névjegy",
                                 "<p style='font-size: 18px'><b>Gráfalgorimtusok grafikus szimulációja</i></p>"
                                 "<p style='font-size: 16px'>A programot készítette: <b>Ladányi Szandra</b></p>"
                                 "<p>Eötvös Loránd Tudományegyetem<br>Programtevező informatikus Bsc.<br>2021</p>"
                                );
    });
}

void GraphViewer::initEditToolbar()
{
    editToolbar = addToolBar(tr("Szerkesztés"));
    pointerButton = new QToolButton();
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/img/up-sign.png"));
    pointerButton->setToolTip(tr("Csúcsok mozgatása | Ctrl+M"));
    pointerButton->setShortcut(QKeySequence("Ctrl+M"));
    nodeButton = new QToolButton();
    nodeButton->setCheckable(true);
    nodeButton->setIcon(QIcon(":/img/circle.png"));
    nodeButton->setToolTip(tr("Csúcsok hozzáadása | Ctrl+N"));
    nodeButton->setShortcut(QKeySequence("Ctrl+N"));
    edgeButton = new QToolButton();
    edgeButton->setCheckable(true);
    edgeButton->setIcon(QIcon(":/img/arrow.png"));
    edgeButton->setToolTip(tr("Élek hozzáadása | Ctrl+E"));
    edgeButton->setShortcut(QKeySequence("Ctrl+E"));
    weightButton = new QToolButton();
    weightButton->setCheckable(true);
    weightButton->setIcon(QIcon(":/img/weight.png"));
    weightButton->setToolTip(tr("Súlyok beállítása | Ctrl+W"));
    weightButton->setShortcut(QKeySequence("Ctrl+W"));
    deleteButton = new QToolButton();
    deleteButton->setCheckable(true);
    deleteButton->setIcon(QIcon(":/img/delete.png"));
    deleteButton->setToolTip(tr("Csúcsok, élek törlése | Ctrl+D"));
    deleteButton->setShortcut(QKeySequence("Ctrl+D"));

    editButtonGroup = new QButtonGroup(this);
    editButtonGroup->addButton(pointerButton);
    editButtonGroup->addButton(nodeButton);
    editButtonGroup->addButton(edgeButton);
    editButtonGroup->addButton(weightButton);
    editButtonGroup->addButton(deleteButton);

    editToolbar->addWidget(pointerButton);
    editToolbar->addWidget(nodeButton);
    editToolbar->addWidget(edgeButton);
    editToolbar->addWidget(weightButton);
    editToolbar->addWidget(deleteButton);
    editToolbar->setMinimumHeight(30);

    connect(pointerButton, &QToolButton::clicked, scene, [=]() {scene->setMode(GraphScene::GraphMode::Move);});
    connect(nodeButton, &QToolButton::clicked, scene, [=]() {scene->setMode(GraphScene::GraphMode::AddNode);});
    connect(edgeButton, &QToolButton::clicked, scene, [=]() {scene->setMode(GraphScene::GraphMode::AddEdge);});
    connect(weightButton, &QToolButton::clicked, scene, [=]() {scene->setMode(GraphScene::GraphMode::SetWeight);});
    connect(deleteButton, &QToolButton::clicked, scene, [=]() {scene->setMode(GraphScene::GraphMode::Delete);});
    connect(editButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, [=](){ weightGroupBox->setHidden(true); });

    directedCheckBox = new QCheckBox(tr("Irányíott"));
    directedCheckBox->setChecked(true);
    directedCheckBox->setToolTip("Ctrl+A");
    directedCheckBox->setShortcut(QKeySequence("Ctrl+A"));
    editToolbar->addWidget(directedCheckBox);
    connect(directedCheckBox, &QCheckBox::toggled, graph, &Graph::changeDirected);


    weightedCheckBox = new QCheckBox(tr("Súlyozott"));
    weightedCheckBox->setChecked(true);
    weightedCheckBox->setToolTip("Ctrl+Q");
    weightedCheckBox->setShortcut(QKeySequence("Ctrl+Q"));
    editToolbar->addWidget(weightedCheckBox);
    connect(weightedCheckBox, &QCheckBox::toggled, graph, &Graph::changeWeighted);
    connect(weightedCheckBox, &QCheckBox::toggled, this, [=](bool d) {
        if (d && !graphTextEditor->isVisible()) weightButton->setEnabled(true);
        else {
            weightButton->setEnabled(false);
            weightLineEdit->setText("");
            weightGroupBox->setVisible(false);
            if (weightButton->isChecked()) pointerButton->click();
        }
    });
}

void GraphViewer::initAlgorithmToolbar()
{
    nodeSelector = new QComboBox();
    algorithmSelector = new QComboBox();
    algorithmSelector->addItem(tr("Szélességi bejárás"), QVariant("szelessegi"));
    algorithmSelector->addItem(tr("Mélységi bejárás"), QVariant("melysegi"));
    algorithmSelector->addItem(tr("Prim-algoritmus"), QVariant("prim"));
    algorithmSelector->addItem(tr("Dijkstra-algoritmus"), QVariant("szelessegi"));
    algorithmSelector->setMinimumHeight(20);
    playButton = new QToolButton();
    playButton->setIcon(QIcon(":/img/play-button.png"));
    playButton->setCheckable(true);
    pauseButton = new QToolButton();
    pauseButton->setIcon(QIcon(":/img/pause.png"));
    pauseButton->setCheckable(true);
    pauseButton->setChecked(true);
    previousButton = new QToolButton();
    previousButton->setIcon(QIcon(":/img/previous.png"));
    nextButton = new QToolButton();
    nextButton->setIcon(QIcon(":/img/next.png"));
    stopButton = new QToolButton();
    stopButton->setIcon(QIcon(":/img/stop.png"));
    algorithmButtonGroup = new QButtonGroup(this);
    algorithmButtonGroup->addButton(playButton);
    algorithmButtonGroup->addButton(pauseButton);
    stepButtonGroup = new QButtonGroup(this);
    stepButtonGroup->addButton(previousButton);
    stepButtonGroup->addButton(nextButton);

    disableAlgorithms();

    algorithmToolbar = addToolBar(tr("Algoritmusok kezezlése"));
    algorithmToolbar->addWidget(nodeSelector);
    algorithmToolbar->addWidget(algorithmSelector);
    algorithmToolbar->addSeparator();
    algorithmToolbar->addWidget(playButton);
    algorithmToolbar->addWidget(pauseButton);
    algorithmToolbar->addWidget(previousButton);
    algorithmToolbar->addWidget(nextButton);
    algorithmToolbar->addWidget(stopButton);

    connect(stopButton, &QToolButton::clicked, this, &GraphViewer::algorithmStopped);
    connect(playButton, &QToolButton::clicked, this, &GraphViewer::algorithmStarted);
    connect(pauseButton, &QToolButton::clicked, this, &GraphViewer::algorithmPaused);
    connect(nextButton, &QToolButton::clicked, this, &GraphViewer::nextPressed);
    connect(previousButton, &QToolButton::clicked, this, &GraphViewer::previousPressed);
    connect(algorithmSelector, &QComboBox::currentTextChanged, this, &GraphViewer::algorithmSelected);
    connect(nodeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){ nodeSelected(index); });

}

void GraphViewer::initViews()
{
    scene = new GraphScene(graph, this);
    view = new QGraphicsView(scene);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    updateSceneRect();


    QWidget *widget = new QWidget;
    setCentralWidget(widget);
    layout = new QHBoxLayout();
    widget->setLayout(layout);
    v_layout = new QVBoxLayout();
    v_layout->addWidget(view);
    layout->addLayout(v_layout);

    graphTextEditor = new GraphTextEditor(graph, this);
    graphTextEditor->setHidden(true);
    layout->addWidget(graphTextEditor);

    connect(graphTextEditor, &GraphTextEditor::nodesChanged, scene, &GraphScene::updateNodes);
    connect(graphTextEditor, &GraphTextEditor::newEdge, scene, &GraphScene::addNewEdge);
    connect(graphTextEditor, &GraphTextEditor::edgeSet, scene, &GraphScene::setEdgeWeight);
    connect(graphTextEditor, &GraphTextEditor::edgeDeleted, scene, &GraphScene::deleteEdge);
    connect(graphTextEditor, &GraphTextEditor::graphReady, this, &GraphViewer::hideGraphTextEditor);
    connect(graphTextEditor, &GraphTextEditor::edgesFull, this, &GraphViewer::edgesFull);

}

void GraphViewer::initAlgoViews()
{
    QHBoxLayout* h_layout = new QHBoxLayout();
    algoValues = new QGridLayout();
    v_layout->addLayout(h_layout);
    h_layout->addLayout(algoValues);
    h_layout->setSpacing(40);

    tableFont.setPixelSize(24);
    tableFont.setBold(true);
    tableDataFont.setPixelSize(24);

    queue = new QLabel("Q - < >");
    queue->setFont(tableFont);
    queue->setTextFormat(Qt::PlainText);
    h_layout->addWidget(queue, Qt::AlignLeft);
    queue->setVisible(false);

    algoInfos = new AlgorithmInfos();
    layout->addWidget(algoInfos);
    algoInfos->setVisible(false);
}

void GraphViewer::initWeightGroup()
{
    weightGroupBox = new QGroupBox(this);
    weightGroupBox->setGeometry(40,80,90,50);
    weightGroupBox->setHidden(true);
    weightLayout = new QHBoxLayout();
    weightGroupBox->setLayout(weightLayout);
    weightLabel = new QLabel("Súly: ");
    weightLineEdit = new QLineEdit();
    QValidator *validator = new QIntValidator(this);
    weightLineEdit->setValidator(validator);
    weightLayout->addWidget(weightLabel);
    weightLayout->addWidget(weightLineEdit);

    connect(weightLineEdit, &QLineEdit::returnPressed, this, [=](){ scene->setWeight(weightLineEdit->text().toInt()); weightGroupBox->setHidden(true); });
}

void GraphViewer::initWarningLabel()
{
    warningLabel = new QLabel(this);
    warningLabel->setGeometry(20,70,500,30);
    warningLabel->setHidden(true);
}

void GraphViewer::updateNodeSelector()
{
    QString name = nodeSelector->currentText();
    nodeSelector->clear();
    QStringList names = graph->getNames();
    names.sort();
    nodeSelector->addItems(names);
    if (nodeSelector->findText(name) != -1) nodeSelector->setCurrentText(name);
}

void GraphViewer::updateSceneRect()
{
    QList<QPointF> positions = scene->originalPositions();
    float w_old = scene->sceneRect().width();
    float h_old = scene->sceneRect().height();
    scene->setSceneRect(view->geometry());
    float w_new = scene->sceneRect().width();
    float h_new = scene->sceneRect().height();
    scene->updatePositions(positions, w_new/w_old, h_new/h_old);
    view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);
}

// ---------------

// PROTECTED FUNCTIONS

void GraphViewer::resizeEvent(QResizeEvent *)
{
    updateSceneRect();
}

void GraphViewer::changeEvent(QEvent * event)
{

    if (event->type() == QEvent::WindowStateChange) {
        scene->updateEdges();
    }
}

