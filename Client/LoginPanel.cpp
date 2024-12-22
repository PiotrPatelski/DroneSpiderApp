#include "LoginPanel.hpp"

LoginPanel::LoginPanel(QWidget *parent, const std::string& ipAddress, const unsigned short port)
	: QWidget(parent, Qt::Window),
    ui(this),
    ipAddress{ipAddress},
    port{port}
{
    setWindowTitle("Login");
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui.getSubmitButton(), &QPushButton::clicked, this, &LoginPanel::onSubmit);
    connect(ui.getCancelButton(), &QPushButton::clicked, this, &QWidget::close);
}

LoginPanel::~LoginPanel()
{
}

void LoginPanel::onSubmit()
{
    emit loginAttempt(LoginData{ ui.getPassword().toStdString(), ipAddress, port});
}

