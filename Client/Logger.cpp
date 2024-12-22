#include "Logger.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTime>

Logger::Logger(QWidget* parent) : QWidget(parent, Qt::Window)
{
    setWindowTitle("Logger");

    logDisplay = new QTextEdit(this);
    logDisplay->setReadOnly(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(logDisplay);

    setLayout(layout);
}

Logger::~Logger()
{
}

void Logger::addLog(const QString& logEntry)
{
    QString timestamp = QTime::currentTime().toString("HH:mm:ss");
    QString logWithTimestamp = timestamp + " - " + logEntry;
    logDisplay->append(logWithTimestamp);
    logDisplay->moveCursor(QTextCursor::End);
}
