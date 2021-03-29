#ifndef GRAPHVIEWER_H
#define GRAPHVIEWER_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QToolButton>
#include <QToolBar>
#include <QButtonGroup>
#include <QComboBox>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QGraphicsView>
#include <QLineEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>

#include "../Model/graph.h"
#include "../Model/algorithm.h"
#include "graphscene.h"
#include "graphtexteditor.h"
#include <tuple>


class GraphViewer : public QMainWindow
{
    Q_OBJECT


public:
    GraphViewer(QWidget *parent = nullptr);

public slots:
    void nodesFull();
    void edgesFull();
    void deleteGraph();
    void algorithmSelected(QString selectedAlgorithm);
    void nodeSelected(QString selectedNode);
    void algorithmStopped();
    void algorithmStarted();
    void algorithmPaused();
    void nextPressed();
    void previousPressed();
    void showGraphTextEditor();
    void hideGraphTextEditor();
    void showWeightGroup();
    void saveFile();
    void openFile();
    void enableAlgorithms();
    void disableAlgorithms();
    void enableSelectors();
    void disableSelectors();
    void enableEdit();
    void disableEdit();
    void needWeights();
    void needOnlyNonnegativeEdges();

private:

    QHBoxLayout* layout;
    QTimer timer;

    QMenu* fileMenu;
    QAction* saveFileAction;
    QAction* openFileAction;

    QMenu* editMenu;
    QMenu* graphMenu;
    QAction* deleteGraphAction;
    QAction* resetLayout;
    QAction* setupGraphAction;


    QToolBar* editToolbar;
    QButtonGroup* editButtonGroup;
    QToolButton* pointerButton;
    QToolButton* nodeButton;
    QToolButton* edgeButton;
    QToolButton* weightButton;
    QToolButton* deleteButton;

    QCheckBox* directedCheckBox;
    QCheckBox* weightedCheckBox;

    QComboBox* nodeSelector;
    QComboBox* algorithmSelector;
    QButtonGroup* algorithmButtonGroup;
    QButtonGroup* stepButtonGroup;
    QToolButton* playButton;
    QToolButton* pauseButton;
    QToolButton* previousButton;
    QToolButton* nextButton;
    QToolButton* stopButton;
    QToolBar* algorithmToolbar;

    QLabel* warningLabel;

    QGroupBox* weightGroupBox;
    QHBoxLayout* weightLayout;
    QLabel* weightLabel;
    QLineEdit* weightLineEdit;

    GraphScene *scene;
    QGraphicsView* view;

    GraphTextEditor* graphTextEditor;


    Graph* graph;
    Algorithm* algo;

    void setStyles();
    void initMenu();
    void initEditToolbar();
    void initAlgorithmToolbar();
    void initViews();
    void initWeightGroup();
    void initWarningLabel();
    void showWarningLabel();
    void updateSceneRect();

    QWidget * QMainWindow;



protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *);
};
#endif // GRAPHVIEWER_H
