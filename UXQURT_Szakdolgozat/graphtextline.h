#ifndef GRAPHTEXTLINE_H
#define GRAPHTEXTLINE_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>


class GraphTextLine : public QWidget
{
    Q_OBJECT
public:
    explicit GraphTextLine(QWidget *parent = nullptr);

    QComboBox* fromComboBox;
    QComboBox* toComboBox;
    QLineEdit* weightLineEdit;
    QPushButton* okButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QHBoxLayout* edgeLayout;

    void setDisabled();


signals:
    void deleteSignal();
    void readyToSetWeight();

public slots:
    void okClicked();
    void editClicked();

};

#endif // GRAPHTEXTLINE_H
