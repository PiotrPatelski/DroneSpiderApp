#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include "RotateRequestInputWindowUi.hpp"

class RotateRequestInputWindow : public QWidget {
    Q_OBJECT

public:
    RotateRequestInputWindow(QWidget* parent = nullptr);

signals:
    void angleSubmitted(const int angle);

private slots:
    void onSubmit();

private:
    RotateRequestInputWindowUi ui;
};