#pragma once

#include <QtGui/QAction>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>

class TcpUdpQtAppUi
{
public:
    TcpUdpQtAppUi(QMainWindow* parrent)
        : menuBar{new QMenuBar(parrent) },
        menuStart{ new QMenu("Start", menuBar) },
        menuAbout{ new QMenu("About", menuBar) },
        connectToServer{ new QAction(parrent) }
    {
        parrent->setWindowTitle("TcpUdpQt Application");
        parrent->resize(600, 400);

        auto* centralWidget = new QWidget(parrent);
        parrent->setCentralWidget(centralWidget);

        menuBar->setEnabled(true);
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menuBar->setDefaultUp(false);

        parrent->setMenuBar(menuBar);

        connectToServer->setText("Connect to server");

        menuBar->addAction(menuStart->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuStart->addAction(connectToServer);
    }

    inline QAction* getConnectToServerAction() { return connectToServer; }

private:
    QMenuBar* menuBar;
    QMenu* menuStart;
    QMenu* menuAbout;
    QAction* connectToServer;
};

