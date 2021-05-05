#include "graphtexteditor.h"

GraphTextEditor::GraphTextEditor(Graph* graph, QWidget *parent) : QWidget(parent)
{

    this->graph = graph;

    setMaximumWidth(200);

    updateNames();

    graphTextLayout = new QVBoxLayout(this);
    this->setLayout(graphTextLayout);
    graphTextLayout->setAlignment(Qt::AlignTop);

    nodeLabel = new QLabel("Csúcsok száma: ");
    nodeSpinBox = new QSpinBox();
    nodeSpinBox->setValue(0);
    nodeSpinBox->setMinimum(0);
    nodeSpinBox->setMaximum(Graph::getMaxNodeNum());
    nodeSpinBox->setMaximumWidth(60);
    nodeSpinBox->setSingleStep(1);
    nodeLayout = new QHBoxLayout();
    nodeLayout->addWidget(nodeLabel);
    nodeLayout->addWidget(nodeSpinBox);

    edgeLabel = new QLabel("Élek: ");
    edgeLayout = new QVBoxLayout();

    buttonLayout = new QHBoxLayout();
    newEdgeButton = new QPushButton(tr("Új él hozzáadása"));
    okButton = new QPushButton(tr("Bezár"));
    okButton->setStyleSheet("background-color: rgb(192, 65, 65);");
    buttonLayout->addWidget(newEdgeButton);
    buttonLayout->addWidget(okButton);


    graphTextLayout->addLayout(nodeLayout);
    graphTextLayout->addWidget(edgeLabel);
    graphTextLayout->addLayout(edgeLayout);
    graphTextLayout->addLayout(buttonLayout);


    connect(newEdgeButton, &QAbstractButton::clicked, this, &GraphTextEditor::addNewEdge);
    connect(nodeSpinBox, &QAbstractSpinBox::editingFinished, this, &GraphTextEditor::refreshGraphSize);
    connect(okButton, &QPushButton::clicked, this, [=](){emit graphReady();});
    connect(graph, &Graph::directedChanged, this, &GraphTextEditor::initEditor);
    connect(graph, &Graph::graphDeleted, this, &GraphTextEditor::initEditor);
}

void GraphTextEditor::initEditor()
{
    nodeSpinBox->setValue(graph->getSize());

    foreach(GraphTextLine* line, edgeLines) {
        delete line;
    }
    edgeLines.clear();

    for(int i=0; i<graph->getSize(); i++) {
        int id = graph->getId(i);
        for(int j=0; j<graph->getAdjNum(i); j++) {
            if ((!graph->getDirected() && (graph->getName(id) < graph->getAdjName(i,j))) ||  graph->getDirected()) {
                GraphTextLine* line = addNewLine();
                QString fromName = graph->getName(id);
                QString toName = graph->getAdjName(i,j);
                line->fromComboBox->addItem(fromName);
                line->toComboBox->addItem(toName);
                line->fromComboBox->setCurrentText(fromName);
                line->toComboBox->setCurrentText(toName);
                line->weightLineEdit->setText(QString::number(graph->getAdjWeight(i,j)));
                line->setDisabled();
            }
        }
    }
}

void GraphTextEditor::addNewEdge()
{
    bool availableEdges = false;
    int i = 0;
    int nodes_num = graph->getSize();
    while (i < nodes_num && !availableEdges) {
        availableEdges = graph->getAdjNum(i) < nodes_num - 1;
        i++;
    }
    updateNames();
    if (availableEdges) {
        if (edgeLines.size() != 0) {
            GraphTextLine* lastLine = edgeLines.at(edgeLines.size()-1);
            if (!lastLine->fromComboBox->isEnabled() && !lastLine->toComboBox->isEnabled()) {
                addNewLine();
            }
        } else {
            addNewLine();
        }
    } else {
        emit edgesFull();
    }
}

void GraphTextEditor::refreshGraphSize()
{
    int newSize = nodeSpinBox->value();
    if (graph->getSize() < newSize) {
        graph->addNodes(newSize - graph->getSize());
        emit nodesChanged();
    } else if (graph->getSize() > newSize){
        graph->deleteNodes(graph->getSize()-newSize);
        emit nodesChanged();
        initEditor();
    }

    updateNames();
}

void GraphTextEditor::deleteEdgeLine()
{
    GraphTextLine* line = qobject_cast<GraphTextLine*>(sender());
    if (!line->fromComboBox->isEnabled() && !line->toComboBox->isEnabled()) {
        graph->deleteEdge(line->fromComboBox->currentText(), line->toComboBox->currentText());
        emit edgeDeleted(line->fromComboBox->currentText(), line->toComboBox->currentText());
    }
    edgeLines.removeAll(line);
    delete line;
}

void GraphTextEditor::setEdge()
{
    GraphTextLine* line = qobject_cast<GraphTextLine*>(sender());

    if (graph->setEdge(line->fromComboBox->currentText(), line->toComboBox->currentText(), line->weightLineEdit->text().toInt())) {
        emit newEdge(line->fromComboBox->currentText(), line->toComboBox->currentText(), line->weightLineEdit->text().toInt());
    } else {
        emit edgeSet(line->fromComboBox->currentText(), line->toComboBox->currentText(), line->weightLineEdit->text().toInt());
    }
}

void GraphTextEditor::fromNodeChanged(QString fromText)
{
    QStringList names2 = graph->getNames();
    names2.prepend("");
    QComboBox* sender_cb = qobject_cast<QComboBox*>(sender());
    GraphTextLine* line = qobject_cast<GraphTextLine*>(sender_cb->parent());
    if (fromText == "") line->toComboBox->clear();
    else {
        names2.removeAll(fromText);
        foreach(GraphTextLine* line2, edgeLines) {
            if (line2 != line && line2->fromComboBox->currentText() == fromText) {      // vegye ki azokat a csúcsokat, amikbe már megy innen él
                names2.removeAll(line2->toComboBox->currentText());
            }
            if(line2 != line && !graph->getDirected() && line2->toComboBox->currentText() == fromText) { // ha irányítatlan, vegye ki azokat a csúcsokat, amikből megy ebbe él
                names2.removeAll(line2->fromComboBox->currentText());
            }
        }

        QString toText = line->toComboBox->currentText();
        line->toComboBox->clear();
        line->toComboBox->addItems(names2);
        if (toText != "" && names2.contains(toText)) {
            line->toComboBox->setCurrentText(toText);
        }
    }
}

GraphTextLine* GraphTextEditor::addNewLine()
{
    GraphTextLine* line = new GraphTextLine(graph->getWeighted());
    edgeLines.append(line);
    edgeLayout->addWidget(line);
    QValidator *validator = new QIntValidator(this);
    line->weightLineEdit->setValidator(validator);
    line->weightLineEdit->setText("1");
    if (names.size() > 1) line->fromComboBox->addItems(names);

    connect(line, &GraphTextLine::deleteSignal, this, &GraphTextEditor::deleteEdgeLine);
    connect(line, &GraphTextLine::readyToSetWeight, this, &GraphTextEditor::setEdge);
    connect(line->fromComboBox, &QComboBox::currentTextChanged, this, &GraphTextEditor::fromNodeChanged);
    connect(graph, &Graph::weightedChanged, line, &GraphTextLine::setWeighted);

    return line;

}

void GraphTextEditor::updateNames()
{
    names.clear();
    names = graph->getNames();
    names.prepend(QString());
    int nodes_num = graph->getSize();
    for(int i=0; i<nodes_num; i++) {
        if (graph->getAdjNum(i) == nodes_num-1) {
            names.removeAll(graph->getName(graph->getId(i)));
        }
    }

    if (edgeLines.size() != 0) {                                                // ha módosult a gráf mérete miközben egy él még nem lett "leokézva", a neveket is frissíti a legördülőben
        GraphTextLine* lastLine = edgeLines.last();
        if (lastLine->fromComboBox->isEnabled() || lastLine->toComboBox->isEnabled()) {
            QString fromText = lastLine->fromComboBox->currentText();
            QString toText = lastLine->toComboBox->currentText();
            lastLine->fromComboBox->clear();
            lastLine->toComboBox->clear();
            if (names.size() > 1) {
                lastLine->fromComboBox->addItems(names);
                lastLine->fromComboBox->setCurrentText(fromText);
                if (toText != "") {
                    lastLine->toComboBox->setCurrentText(toText);
                }
            }
        }
    }
}
