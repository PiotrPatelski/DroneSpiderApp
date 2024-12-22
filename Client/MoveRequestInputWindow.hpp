#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include "MoveRequestInputWindowUi.hpp"

class MoveRequestInputWindow : public QWidget {
    Q_OBJECT

public:
    MoveRequestInputWindow(QWidget* parent = nullptr);
    virtual ~MoveRequestInputWindow() = default;

signals:
    void positionSubmitted(const std::pair<int, int>& position);

private slots:
    void onSubmit();

private:
    MoveRequestInputWindowUi ui;
};