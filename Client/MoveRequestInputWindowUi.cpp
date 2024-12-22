#include "MoveRequestInputWindowUi.hpp"
#include <QVBoxLayout>
#include <QLabel>

MoveRequestInputWindowUi::MoveRequestInputWindowUi(QWidget* parent) {
    QVBoxLayout* layout = new QVBoxLayout(parent);

    QLabel* descriptionLabel = new QLabel("Set the target position", parent);
    QLabel* xLabel = new QLabel("X Position:", parent);
    xInput = new QLineEdit(parent);
    QLabel* yLabel = new QLabel("Y Position:", parent);
    yInput = new QLineEdit(parent);

    submitButton = new QPushButton("Submit", parent);

    layout->addWidget(descriptionLabel);
    layout->addWidget(xLabel);
    layout->addWidget(xInput);
    layout->addWidget(yLabel);
    layout->addWidget(yInput);
    layout->addWidget(submitButton);

    parent->setLayout(layout);
    parent->setWindowTitle("Move request");
    parent->setFixedSize(200, 150);
}