#include "graphtextline.h"

GraphTextLine::GraphTextLine(QWidget *parent) : QWidget(parent)
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

    connect(deleteButton, &QPushButton::clicked, this, [=]() { emit deleteSignal(); });
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(weightLineEdit, SIGNAL(returnPressed()), this, SLOT(okClicked()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));

}

void GraphTextLine::setDisabled()
{
    okButton->setHidden(true);
    editButton->setHidden(false);
    fromComboBox->setEnabled(false);
    toComboBox->setEnabled(false);
    weightLineEdit->setEnabled(false);
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
