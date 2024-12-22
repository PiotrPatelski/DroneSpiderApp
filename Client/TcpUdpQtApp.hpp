#pragma once

#include <QtWidgets/QMainWindow>
#include <memory>
#include "TcpUdpQtAppUi.hpp"
#include "ConnectionWindow.hpp"

class TcpUdpQtApp : public QMainWindow
{
    Q_OBJECT

public:
    TcpUdpQtApp(QWidget *parent = nullptr);
    ~TcpUdpQtApp();

private slots:
    void onConnectToServer();

private:
    void updateSavedEntryBase(const std::map<QString, QString>& newEntries);

    TcpUdpQtAppUi ui;
    std::shared_ptr<std::map<QString, QString>> savedIpAddressEntryBase{ nullptr };
    ConnectionWindow* connectionWindow{ nullptr };
};
