#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <memory>
#include "ConnectionWindowUi.hpp"
#include "LoginPanel.hpp"
#include "Logger.hpp"
#include "TcpClient.hpp"

class Logger;

class ConnectionWindow : public QWidget
{
	Q_OBJECT

public:
	ConnectionWindow(QWidget *parent, std::shared_ptr<const std::map<QString, QString>> ipAddressEntryBase);
	~ConnectionWindow();

signals:
	void dataFromLastSession(const std::map<QString, QString>& savedEntries);

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void onSubmit();
	void onRowSelected();

private:
	void attemptToConnect(const QString& ipAddressValue, const QString& portValue);
	void loginToConnectedServer(const std::string& ipAddress, const unsigned short port);
	void runLoginPanel(const std::string& ipAddress, const unsigned short port);
	void handleLoginInput(const LoginData& loginData);
	void startConnectedService(const std::string& ipAddress, const unsigned short port, const unsigned short userId);
	void fillIpAddressTableWithPredefinedBase(const std::map<QString, QString>& ipAddressEntryBase);
	bool isUserInputValid(const QString& ipAddressValue, const QString& portValue);
	bool isUnique(const QString& ipAddressValue, const QString& portValue);

	ConnectionWindowUi ui;
	Logger* logger{ nullptr };
	LoginPanel* loginPanel{ nullptr };

	std::map<QString, QString> ipAddressEntriesSavedDuringThisSession{};
	std::shared_ptr<const std::map<QString, QString>> ipAddressEntryBase{ nullptr };
	std::unique_ptr<TcpClient> tcpEndpoint{ nullptr };
};
