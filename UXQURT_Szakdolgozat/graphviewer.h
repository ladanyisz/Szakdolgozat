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

#include "graph.h"
#include "graphscene.h"
#include "graphtexteditor.h"


class GraphViewer : public QMainWindow
{
    Q_OBJECT

public:
    GraphViewer(QWidget *parent = nullptr);

public slots:
    void nodesFull();
    void deleteGraph();
    void showGraphTextEditor();
    void hideGraphTextEditor();
    void showWeightGroup();

private:

    QHBoxLayout* layout;
    QTimer timer;

    QMenu* fileMenu;
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

    QComboBox* nodeSelector;
    QComboBox* algorithmSelector;
    QButtonGroup* algorithmButtonGroup;
    QButtonGroup* stepButtonGroup;
    QToolButton* playButton;
    QToolButton* pauseButton;
    QToolButton* previousButton;
    QToolButton* nextButton;
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

    void setStyles();
    void initMenu();
    void initEditToolbar();
    void initAlgorithmToolbar();
    void initViews();
    void initWeightGroup();
    void initWarningLabel();
    void showWarningLabel();

    QWidget * QMainWindow;



protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *);
};
#endif // GRAPHVIEWER_H
