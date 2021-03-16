#include "graphtexteditor.h"
#include <QDebug>

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
    nodeSpinBox->setMaximum(graph->getMaxNodeNum());
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
    buttonLayout->addWidget(newEdgeButton);
    buttonLayout->addWidget(okButton);


    graphTextLayout->addLayout(nodeLayout);
    graphTextLayout->addWidget(edgeLabel);
    graphTextLayout->addLayout(edgeLayout);
    graphTextLayout->addLayout(buttonLayout);


    connect(newEdgeButton, SIGNAL(clicked()), this, SLOT(addNewEdge()));
    connect(nodeSpinBox, SIGNAL(editingFinished()), this, SLOT(refreshGraphSize()));
    connect(okButton, &QPushButton::clicked, this, [=](){emit graphReady();});
}

void GraphTextEditor::initEditor()
{
    graph->serializeGraph();
    nodeSpinBox->setValue(graph->getSize());
    updateNames();

    foreach(GraphTextLine* line, edgeLines) {
        delete line;
    }
    edgeLines.clear();

    for(int i=0; i<graph->getSize(); i++) {
        int id = graph->getId(i);
        for(int j=0; j<graph->getAdjNum(i); j++) {
            GraphTextLine* line = addNewLine();
            updateNames();
            line->fromComboBox->setCurrentText(graph->getName(id));
            line->toComboBox->setCurrentText(graph->getAdjName(i,j));
            line->weightLineEdit->setText(QString::number(graph->getAdjWeight(i,j)));
            line->setDisabled();
        }
    }
}

void GraphTextEditor::addNewEdge()
{
    if (edgeLines.size() != 0) {
        GraphTextLine* lastLine = edgeLines.at(edgeLines.size()-1);
        if (!lastLine->fromComboBox->isEnabled() && !lastLine->toComboBox->isEnabled()) {
            addNewLine();
        }
    } else {
        addNewLine();
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
    graph->serializeGraph();
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
    QStringList names2 = names;
    QComboBox* sender_cb = qobject_cast<QComboBox*>(sender());
    GraphTextLine* line = qobject_cast<GraphTextLine*>(sender_cb->parent());
    if (fromText == "") line->toComboBox->clear();
    else {
        names2.removeAll(fromText);
        foreach(GraphTextLine* line2, edgeLines) {
            if (line2 != line && line2->fromComboBox->currentText() == fromText) {
                names2.removeAll(line2->toComboBox->currentText());
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
    GraphTextLine* line = new GraphTextLine();
    edgeLines.append(line);
    edgeLayout->addWidget(line);
    QValidator *validator = new QIntValidator(this);
    line->weightLineEdit->setValidator(validator);
    line->weightLineEdit->setText("1");
    if (names.size() > 1) line->fromComboBox->addItems(names);

    connect(line, SIGNAL(deleteSignal()), this, SLOT(deleteEdgeLine()));
    connect(line, SIGNAL(readyToSetWeight()), this, SLOT(setEdge()));
    connect(line->fromComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(fromNodeChanged(QString)));

    return line;

}

void GraphTextEditor::updateNames()
{
    names.clear();
    names = graph->getNames();
    names.prepend(QString());
    foreach(GraphTextLine* line, edgeLines) {
        QString fromText = line->fromComboBox->currentText();
        QString toText = line->toComboBox->currentText();
        line->fromComboBox->clear();
        line->toComboBox->clear();
        if (names.size() > 1) {
            line->fromComboBox->addItems(names);
            line->fromComboBox->setCurrentText(fromText);
            if (toText != "") {
                line->toComboBox->setCurrentText(toText);
            }
        }

    }
}
