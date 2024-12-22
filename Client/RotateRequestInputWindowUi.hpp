#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class RotateRequestInputWindowUi
{
public:
    RotateRequestInputWindowUi(QWidget* parent);

    inline QPushButton* getSubmitButton() { return submitButton; }
    inline int getAngle() const {
        return angleInput->text().toInt();
    }
    inline bool isInputEmpty() const {
        return (angleInput->text().isEmpty());
    }
private:
    QLineEdit* angleInput{ nullptr };
    QPushButton* submitButton{ nullptr };
};