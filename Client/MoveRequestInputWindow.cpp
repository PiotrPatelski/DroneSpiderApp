#include "MoveRequestInputWindow.hpp"
#include <QMessageBox>

MoveRequestInputWindow::MoveRequestInputWindow(QWidget* parent) 
    : QWidget(parent, Qt::Window),
    ui(this) 
{
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui.getSubmitButton(), &QPushButton::clicked, this, &MoveRequestInputWindow::onSubmit);
}

void MoveRequestInputWindow::onSubmit()
{
    if (not ui.isInputEmpty()) {
        emit positionSubmitted(ui.getPosition());
        close();
    }
    else {
        QMessageBox::warning(this, "Invalid Input", "Please enter values for both X and Y positions.");
    }
}
