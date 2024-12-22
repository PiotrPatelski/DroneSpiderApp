#include "LoginPanelUi.hpp"
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

LoginPanelUi::LoginPanelUi(QWidget* parent)
    : passwordInput(new QLineEdit(parent)),
    submitButton(new QPushButton("Submit", parent)),
    cancelButton(new QPushButton("Cancel", parent)),
    descriptionLabel(new QLabel("Please type password requested by desired server:", parent))
{
    passwordInput->setEchoMode(QLineEdit::Password);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(descriptionLabel);
    layout->addWidget(passwordInput);
    layout->addWidget(submitButton);
    layout->addWidget(cancelButton);
    parent->setLayout(layout);
}

