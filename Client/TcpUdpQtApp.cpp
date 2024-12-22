#include "TcpUdpQtApp.hpp"
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QDebug>


std::shared_ptr<std::map<QString, QString>> fetchIpAddressEntriesFromFile()
{
    auto entries = std::make_shared<std::map<QString, QString>>();
    QFile file("savedIpEntries.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList parts = line.split(':');
            if (parts.size() == 2)
            {
                entries->insert({ parts[0], parts[1] });
            }
        }
        file.close();
    }
    else
    {
        qDebug() << "Unable to open file for reading.";
    }
    return entries;
}

TcpUdpQtApp::TcpUdpQtApp(QWidget *parent)
    : QMainWindow(parent),
    ui(this),
    savedIpAddressEntryBase(fetchIpAddressEntriesFromFile())
{
    connect(ui.getConnectToServerAction(), &QAction::triggered, this, &TcpUdpQtApp::onConnectToServer);
}

TcpUdpQtApp::~TcpUdpQtApp()
{}

void TcpUdpQtApp::onConnectToServer()
{
    connectionWindow = new ConnectionWindow(this, savedIpAddressEntryBase);
    connect(connectionWindow, &ConnectionWindow::dataFromLastSession, this, &TcpUdpQtApp::updateSavedEntryBase);
    if (not connectionWindow->isVisible())
    {
        connectionWindow->show();
    }
    else 
    {
        connectionWindow->raise();
        connectionWindow->activateWindow();
    }
    
}

void TcpUdpQtApp::updateSavedEntryBase(const std::map<QString, QString>& newEntries)
{
    QFile file("savedIpEntries.txt");
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        for (const auto& [ipAddress, port] : newEntries)
        {
            QString line = ipAddress + ":" + port;
            out << line << "\n";
            savedIpAddressEntryBase->insert({ ipAddress , port });
        }
        file.close();
    }
    else
    {
        qDebug() << "Unable to open file for writing.";
    }
}

