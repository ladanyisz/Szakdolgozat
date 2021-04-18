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
#include <QTableWidget>

#include "../Model/graph.h"
#include "../Model/algorithm.h"
#include "graphscene.h"
#include "graphtexteditor.h"
#include "algorithminfos.h"
#include "algorithmhelp.h"
#include <tuple>


class GraphViewer : public QMainWindow
{
    Q_OBJECT


public:
    GraphViewer(QWidget *parent = nullptr);

public slots:
    void nodesFull();                                       // betelt a maximálisan lehelyezhető csúcsok száma
    void edgesFull();                                       // nincs több lehetséges él
    void deleteGraph();                                     // egész gráf törlése
    void algorithmSelected(QString selectedAlgorithm);      // változik a kiválasztott algoritmus
    void nodeSelected(int);                                 // változik a kiválasztott startcsúcs

    void algorithmStopped();                                // stop gomb megnyomása
    void algorithmStarted();                                // start gomb megnyomása
    void algorithmPaused();                                 // szünet gomb megnyomása
    void nextPressed();                                     // következő gomb megnyomása
    void previousPressed();                                 // előző gomb megnyomása

    void showGraphTextEditor();                             // "szöveges" szerkesztő megnyitása
    void hideGraphTextEditor();                             // "szöveges" szerkesztő elrejtése
    void showWeightGroup();                                 // súlyszerkesztő megjelenítése

    void saveFile();                                        // mentés
    void openFile();                                        // betöltés

    void enableAlgorithms();                                // algoritmus kiválasztó és kezelő engedélyezése
    void disableAlgorithms();                               // algoritmus kiválasztó és kezelő letiltása
    void enableSelectors();                                 // algoritmus kiválasztó engedélyezése
    void disableSelectors();                                // algoritmus kiválasztó letiltása
    void enableEdit();                                      // szerkesztő gombok engedélyezése
    void disableEdit();                                     // szerkesztő gombok letiltása

    void needWeights();                                     // figyelmeztetés: az algortimus csak élsúlyozott gráfon futttatható
    void noWeights();                                       // figyelmeztetés: az algorimtus csak élsúlyozatlan gráfon futtatható
    void needOnlyNonnegativeEdges();                        // figyelmeztetés: az algoritmus csak nemnegatív élsúlyokkal futtatható
    void needsToBeConnected();                              // figyelmeztetés: az algoritmus csak összefüggő gráfon futtatható
    void needToBeDirected();                                // figyelmeztetés: az algoritmus csak irányított gráfon futtatható
    void needsToBeUndirected();                             // figyelmeztetés: az algoritmus csak irányítatlan gráfon futtatható

    void algoInitReady(int);                                // algoritmus inicializálás kész
    void parentChanged(int, QChar);                         // változott egy csúcs szülője
    void distChanged(int, int);                             // változott egy csúcs távolsága a kezdőcsúcstól
    void queueChanged(QString);                             // változott a sor
    void discoveryFinishChanged(int, int, int);             // változott az elérési / befejezési idő
    void clearColorsInAlgTable();                           // színezések leszedhetőek

private:

    QHBoxLayout* layout;
    QVBoxLayout* v_layout;
    QTimer timer;

    // MENÜSÁV
    QMenu* fileMenu;
    QAction* saveFileAction;
    QAction* openFileAction;

    QMenu* editMenu;
    QMenu* graphMenu;
    QAction* deleteGraphAction;
    QAction* resetLayout;
    QAction* setupGraphAction;

    QAction* szelessegiHelpAction;
    QAction* melysegiHelpAction;
    QAction* primHelpAction;
    QAction* dijkstraHelpAction;

    QAction* aboutMenu;


    // ESZKÖZSÁV
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

    // SÚLYSZERKESZTŐ
    QGroupBox* weightGroupBox;
    QHBoxLayout* weightLayout;
    QLabel* weightLabel;
    QLineEdit* weightLineEdit;

    // GRÁF ELEMEINEK "TÁROLÓJA"
    GraphScene *scene;
    QGraphicsView* view;

    GraphTextEditor* graphTextEditor;

    QString path;

    // Algoritmus által számított értékek
    QGridLayout* algoValues;
    QFont tableFont;
    QFont tableDataFont;
    QLabel* queue;

    // KIRAJZOLT STUKI + SZÍNKÓDOK
    AlgorithmInfos* algoInfos;


    Graph* graph;
    Algorithm* algo;

    void algoConnections();         // algoritmus eseménykezelő beállítások
    void setStyles();               // stílusbeállítások
    void initMenu();                // menüsáv beállítása
    void initEditToolbar();         // algoritmuskezelő-eszközsáv beállítása
    void initAlgorithmToolbar();    // szerkesztő-eszközsáv beállítása
    void initViews();               // felületi szerkesztő beállítása
    void initAlgoViews();           // algorimtus futása közbeni adatok megjelenítésének beállítása
    void initWeightGroup();         // súlyszerkesztő beállítása
    void initWarningLabel();        // figyelmeztető felirat beállítása
    void updateNodeSelector();      // startcsúcs legördülő menüjének beállítása
    void showWarningLabel();        // figyelmeztető felirat mutatása
    void updateSceneRect();         // felületi gráf-megjelenítő frissítése




protected:
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *);
};
#endif // GRAPHVIEWER_H
