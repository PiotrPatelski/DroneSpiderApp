#include "TcpUdpQtApp.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpUdpQtApp w;
    w.show();
    return a.exec();
}
