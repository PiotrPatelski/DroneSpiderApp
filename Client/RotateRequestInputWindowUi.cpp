#include "RotateRequestInputWindowUi.hpp"
#include <QVBoxLayout>
#include <QLabel>

RotateRequestInputWindowUi::RotateRequestInputWindowUi(QWidget* parent) {
    QVBoxLayout* layout = new QVBoxLayout(parent);

    QLabel* descriptionLabel = new QLabel("Set the target angle", parent);
    QLabel* angleLabel = new QLabel("Angle:", parent);

    angleInput = new QLineEdit(parent);
    submitButton = new QPushButton("Submit", parent);

    layout->addWidget(descriptionLabel);
    layout->addWidget(angleLabel);
    layout->addWidget(angleInput);
    layout->addWidget(submitButton);

    parent->setLayout(layout);
    parent->setWindowTitle("Move request");
    parent->setFixedSize(200, 150);
}