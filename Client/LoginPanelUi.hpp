#pragma once

#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QString>
#include <QLabel>

class LoginPanelUi
{
public:
    LoginPanelUi(QWidget* parrent);
    inline void clearInput() { passwordInput->clear(); }

    inline QPushButton* getSubmitButton() { return submitButton; }
    inline QPushButton* getCancelButton() { return cancelButton; }
    inline const QString getPassword() const { return passwordInput->text(); }

private:

    QLineEdit* passwordInput{ nullptr };
    QPushButton* submitButton{ nullptr };
    QPushButton* cancelButton{ nullptr };
    QLabel* descriptionLabel{ nullptr };
};

