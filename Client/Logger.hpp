#pragma once

#include <QWidget>
#include <QTextEdit>

class Logger : public QWidget
{
	Q_OBJECT

public:
	Logger(QWidget *parent = nullptr);
	~Logger();

	void addLog(const QString& logEntry);
private:
	QTextEdit* logDisplay;
};
