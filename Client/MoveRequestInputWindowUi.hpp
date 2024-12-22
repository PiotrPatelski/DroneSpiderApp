#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class MoveRequestInputWindowUi
{
public:
    MoveRequestInputWindowUi(QWidget* parent);

    inline QPushButton* getSubmitButton() { return submitButton; }
    inline std::pair<int, int> getPosition() const {
        return std::make_pair(xInput->text().toInt(), yInput->text().toInt());
    }
    inline bool isInputEmpty() const {
        return (xInput->text().isEmpty() or yInput->text().isEmpty());
    }
private:
    QLineEdit* xInput{ nullptr };
    QLineEdit* yInput{ nullptr };
    QPushButton* submitButton{ nullptr };
};