#include "graphviewer.h"
//#include "ui_graphviewer.h"

#include <QSizePolicy>
#include <QTimer>
#include <QDebug>
#include <QMatrix>
#include <QGraphicsItem>
#include<QMenuBar>
#include <QCheckBox>
#include <QDir>

GraphViewer::GraphViewer(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Gráfok");
//    setMinimumSize(800,500);
//    setBaseSize(800,500);

    graph = new Graph();

    initViews();
    resize(800, 700);
    initMenu();
    initEditToolbar();
    initAlgorithmToolbar();
    initWarningLabel();
    initWeightGroup();


    pointerButton->click();

    connect(graph, &Graph::nodesFull,this, &GraphViewer::nodesFull);
    connect(graph, &Graph::newEdge, scene, &GraphScene::addNewEdge);
    connect(scene, &GraphScene::edgeSelected, this, &GraphViewer::showWeightGroup);



    setStyles();
}

void GraphViewer::setStyles()
{
    this->setStyleSheet("QToolBar {spacing: 6px; padding: 6px 3px;}"
                        "QComboBox {"
                         "font-size: 14px;"
                         "padding: 3px 6px;"
                        "}"
                        "QCombobox::drop-down {padding: 56px;}");
    warningLabel->setStyleSheet("font-size: 16px; color: red;");
    weightGroupBox->setStyleSheet("background-color: white; border: 1px solid black;");
    weightLabel->setStyleSheet("border: unset;");
}

void GraphViewer::initMenu()
{
    fileMenu = menuBar()->addMenu(tr("&Fájl"));
    saveFileAction = new QAction(QIcon(":/img/save.png"),tr("Mentés"));
    fileMenu->addAction(saveFileAction);
    connect(saveFileAction, &QAction::triggered, this, &GraphViewer::saveFile);
    openFileAction = new QAction(QIcon(":/img/open.png"), tr("Megnyitás"));
    fileMenu->addAction(openFileAction);
    connect(openFileAction, &QAction::triggered, this, &GraphViewer::openFile);
    //graphMenu = menuBar()->addMenu(tr("&Szereksztés"));

//    deleteGraphAction = new QAction(QIcon(":/img/delete.png"),tr("Gráf törlése"));
//    graphMenu->addAction(deleteGraphAction);
    deleteGraphAction = new QAction(tr("Gráf törlése"));
    menuBar()->addAction(deleteGraphAction);
    connect(deleteGraphAction, &QAction::triggered, this, &GraphViewer::deleteGraph);

    //setupGraphAction = new QAction(QIcon(":/img/circle.png"), tr("Gráf szöveges megadása"));
    setupGraphAction = new QAction(tr("Szöveges megadás"));
    menuBar()->addAction(setupGraphAction);
    //graphMenu->addAction(setupGraphAction);
    connect(setupGraphAction, &QAction::triggered, this, &GraphViewer::showGraphTextEditor);

    resetLayout = new QAction(tr("Rendezés"));
    menuBar()->addAction(resetLayout);
    connect(resetLayout, &QAction::triggered, scene, &GraphScene::updateNodes);
}

void GraphViewer::initEditToolbar()
{
    editToolbar = addToolBar(tr("Szerkesztés"));
    pointerButton = new QToolButton();
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/img/up-sign.png"));
    nodeButton = new QToolButton();
    nodeButton->setCheckable(true);
    nodeButton->setIcon(QIcon(":/img/circle.png"));
    edgeButton = new QToolButton();
    edgeButton->setCheckable(true);
    edgeButton->setIcon(QIcon(":/img/arrow.png"));
    weightButton = new QToolButton();
    weightButton->setCheckable(true);
    weightButton->setIcon(QIcon(":/img/weight.png"));
    deleteButton = new QToolButton();
    deleteButton->setCheckable(true);
    deleteButton->setIcon(QIcon(":/img/delete.png"));

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
    editToolbar->addWidget(directedCheckBox);
    connect(directedCheckBox, &QCheckBox::toggled, graph, &Graph::changeDirected);


    weightedCheckBox = new QCheckBox(tr("Súlyozott"));
    weightedCheckBox->setChecked(true);
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
    nodeSelector->setEnabled(false);
    algorithmSelector = new QComboBox();
    algorithmSelector->addItem(tr("Szélességi bejárás"), QVariant("szelessegi"));
    algorithmSelector->addItem(tr("Mélységi bejárás"), QVariant("melysegi"));
    algorithmSelector->addItem(tr("Prim-algoritmus"), QVariant("prim"));
    algorithmSelector->addItem(tr("Dijkstra-algoritmus"), QVariant("szelessegi"));
    algorithmSelector->setMinimumHeight(20);
    algorithmSelector->setEnabled(false);
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
    algorithmButtonGroup = new QButtonGroup(this);
    algorithmButtonGroup->addButton(playButton);
    algorithmButtonGroup->addButton(pauseButton);
    stepButtonGroup = new QButtonGroup(this);
    stepButtonGroup->addButton(previousButton);
    stepButtonGroup->addButton(nextButton);

    algorithmToolbar = addToolBar(tr("Algoritmusok kezezlése"));
    algorithmToolbar->addWidget(nodeSelector);
    algorithmToolbar->addWidget(algorithmSelector);
    algorithmToolbar->addSeparator();
    algorithmToolbar->addWidget(playButton);
    algorithmToolbar->addWidget(pauseButton);
    algorithmToolbar->addWidget(previousButton);
    algorithmToolbar->addWidget(nextButton);
}

void GraphViewer::initViews()
{
    scene = new GraphScene(graph, this);
    view = new QGraphicsView(scene);
//    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    view->updateGeometry();


    QWidget *widget = new QWidget;
    setCentralWidget(widget);
    layout = new QHBoxLayout();
    layout->addWidget(view);
    widget->setLayout(layout);

    graphTextEditor = new GraphTextEditor(graph, this);
    graphTextEditor->setHidden(true);
    layout->addWidget(graphTextEditor);

    scene->setSceneRect(view->geometry());
    view->fitInView(scene->sceneRect(), Qt::IgnoreAspectRatio);

    connect(graphTextEditor, &GraphTextEditor::nodesChanged, scene, &GraphScene::updateNodes);
    connect(graphTextEditor, &GraphTextEditor::newEdge, scene, &GraphScene::addNewEdge);
    connect(graphTextEditor, &GraphTextEditor::edgeSet, scene, &GraphScene::setEdgeWeight);
    connect(graphTextEditor, &GraphTextEditor::edgeDeleted, scene, &GraphScene::deleteEdge);
    connect(graphTextEditor, &GraphTextEditor::graphReady, this, &GraphViewer::hideGraphTextEditor);
    connect(graphTextEditor, &GraphTextEditor::edgesFull, this, &GraphViewer::edgesFull);

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
    int w = width();
    warningLabel->setGeometry(20,70,w,30);
    warningLabel->setHidden(true);
}

void GraphViewer::nodesFull()
{
    warningLabel->setText("Elérted a maximálisan elhelyezhető csúcsok számát! (" +  QString::number(graph->getMaxNodeNum()) + ")");
    showWarningLabel();
}

void GraphViewer::edgesFull()
{
    warningLabel->setText("Nincs több lehetséges él!");
    showWarningLabel();
}

void GraphViewer::deleteGraph()
{
    graph->deleteAll();

}

void GraphViewer::showGraphTextEditor()
{
    graphTextEditor->initEditor();
    graphTextEditor->setHidden(false);
    nodeButton->setEnabled(false);
    edgeButton->setEnabled(false);
    weightButton->setEnabled(false);
    deleteButton->setEnabled(false);
    playButton->setEnabled(false);
    pauseButton->setEnabled(false);
    previousButton->setEnabled(false);
    nextButton->setEnabled(false);
    setupGraphAction->setEnabled(false);

    weightLineEdit->setText("");
    weightGroupBox->setVisible(false);
    pointerButton->click();
}

void GraphViewer::hideGraphTextEditor()
{
    graphTextEditor->setHidden(true);
    nodeButton->setEnabled(true);
    edgeButton->setEnabled(true);
    if (graph->getWeighted()) weightButton->setEnabled(true);
    deleteButton->setEnabled(true);
    playButton->setEnabled(true);
    pauseButton->setEnabled(true);
    previousButton->setEnabled(true);
    nextButton->setEnabled(true);
    setupGraphAction->setEnabled(true);
}

void GraphViewer::showWeightGroup()
{
    weightLineEdit->setText("");
    weightLineEdit->setFocus();
    weightGroupBox->setVisible(true);
}

void GraphViewer::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Gráf mentése"), QDir::homePath(), tr("Gráfok (*.graph)"));

    if (fileName != "") {
        QVector<QPointF> positions = scene->nodePositions();
        graph->saveGraph(fileName, positions);
    }
}

void GraphViewer::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Gráf betöltése"), QDir::currentPath(), tr("Gráfok (*.graph)"));

    if (fileName != "" && fileName != QString()) {
        QVector<std::tuple<int, QChar, QPointF>> nodes_data = graph->loadGraph(fileName);
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
    }
}

void GraphViewer::showWarningLabel()
{
    warningLabel->setHidden(false);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, this, [=]() {warningLabel->setHidden(true);});
    timer.start(2000);
}

void GraphViewer::resizeEvent(QResizeEvent *)
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

void GraphViewer::changeEvent(QEvent * event)
{

    if (event->type() == QEvent::WindowStateChange) {
        scene->updateEdges();
    }
}

