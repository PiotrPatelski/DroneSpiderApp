#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include "LoginPanelUi.hpp"


struct LoginData
{
	std::string password{ "" };
	std::string ipAddress{ "" };
	unsigned short port{0};
};

class LoginPanel : public QWidget
{
	Q_OBJECT

public:
	LoginPanel(QWidget *parent, const std::string& ipAddress, const unsigned short port);
	virtual ~LoginPanel();

signals:
	void loginAttempt(const LoginData& loginData);

private slots:
	void onSubmit();

private:
	LoginPanelUi ui;
	std::string ipAddress{""};
	unsigned short port;
};
