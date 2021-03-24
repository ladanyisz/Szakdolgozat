#include "graphtextline.h"

GraphTextLine::GraphTextLine(bool weighted, QWidget *parent) : QWidget(parent)
{

    fromComboBox = new QComboBox(this);
    toComboBox = new QComboBox(this);
    fromComboBox->setMaximumWidth(40);
    toComboBox->setMaximumWidth(40);

    weightLineEdit = new QLineEdit();
    weightLineEdit->setMaximumWidth(30);

    okButton = new QPushButton();
    okButton->setStyleSheet("border-image:url(:/img/tick.png);");
    okButton->setCursor(QCursor(Qt::PointingHandCursor));
    okButton->setMinimumHeight(22);

    editButton = new QPushButton();
    editButton->setStyleSheet("border-image:url(:/img/blue_edit.png)");
    editButton->setCursor(QCursor(Qt::PointingHandCursor));
    editButton->setMinimumHeight(22);
    editButton->setHidden(true);

    deleteButton = new QPushButton(/*QIcon(":/img/red_trash.png"), ""*/);
    deleteButton->setStyleSheet("border-image:url(:/img/red_trash.png);");
    deleteButton->setCursor(QCursor(Qt::PointingHandCursor));
    deleteButton->setMinimumHeight(22);

    edgeLayout = new QHBoxLayout();
    edgeLayout->addWidget(fromComboBox);
    edgeLayout->addWidget(toComboBox);
    edgeLayout->addWidget(weightLineEdit);
    edgeLayout->addWidget(okButton);
    edgeLayout->addWidget(editButton);
    edgeLayout->addWidget(deleteButton);
    setLayout(edgeLayout);
    edgeLayout->setAlignment(Qt::AlignJustify);
    edgeLayout->setContentsMargins(0,3,0,3);

    setWeighted(weighted);

    connect(deleteButton, &QPushButton::clicked, this, [=]() { emit deleteSignal(); });
    connect(okButton, &QAbstractButton::clicked, this, &GraphTextLine::okClicked);
    connect(weightLineEdit, &QLineEdit::returnPressed, this, &GraphTextLine::okClicked);
    connect(editButton, &QAbstractButton::clicked, this, &GraphTextLine::editClicked);

}

void GraphTextLine::setDisabled()
{
    okButton->setHidden(true);
    if (weight) editButton->setHidden(false);
    fromComboBox->setEnabled(false);
    toComboBox->setEnabled(false);
    weightLineEdit->setEnabled(false);
}

void GraphTextLine::setWeighted(bool w) {
    weight = w;
    if (!w) {
        weightLineEdit->setEnabled(false);
        weightLineEdit->setVisible(false);
        editButton->setVisible(false);
        okButton->setVisible(false);
        if(fromComboBox->currentText() == "" || toComboBox->currentText() == "") {
            okButton->setVisible(true);
        }
    } else {
        if(fromComboBox->currentText() == "" || toComboBox->currentText() == "") {
            okButton->setVisible(true);
        } else editButton->setVisible(true);
        weightLineEdit->setVisible(true);
    }
}


void GraphTextLine::okClicked()
{
    if(fromComboBox->currentText() != "" && toComboBox->currentText() != "" && weightLineEdit->text() != "") {
        setDisabled();
        emit readyToSetWeight();
    }
}

void GraphTextLine::editClicked()
{
    okButton->setHidden(false);
    editButton->setHidden(true);
    weightLineEdit->setEnabled(true);
}
