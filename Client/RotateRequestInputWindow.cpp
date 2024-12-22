#include "RotateRequestInputWindow.hpp"
#include <QMessageBox>

RotateRequestInputWindow::RotateRequestInputWindow(QWidget* parent)
    : QWidget(parent, Qt::Window),
    ui(this) 
{
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui.getSubmitButton(), &QPushButton::clicked, this, &RotateRequestInputWindow::onSubmit);
}

void RotateRequestInputWindow::onSubmit()
{
    if (not ui.isInputEmpty()) {
        emit angleSubmitted(ui.getAngle());
        close();
    }
    else {
        QMessageBox::warning(this, "Invalid Input", "Please enter values for both X and Y positions.");
    }
}
