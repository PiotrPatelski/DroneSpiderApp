#pragma once

#include <QAction>
#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QWidget>
#include <QPushButton>
#include <QString>
#include <optional>

struct Entry
{
    QString ipAddress{};
    QString port{};
};

class ConnectionWindowUi
{
public:
    ConnectionWindowUi(QWidget* parrent);
    void addIpAddressEntry(const Entry& entry);
    void setInput(const Entry& entry);
    void clearInput();

    std::optional<Entry> findSelectedEntry();
    inline QTableWidget* getIpAddressTable() { return ipAddressTable; }
    inline QPushButton* getSubmitButton() { return submitButton; }
    inline const QString getIpAddress() const { return ipAddressInput->text(); }
    inline const QString getPort() const { return portInput->text(); }

private:
    QTableWidget* createIpAddressTable(QWidget* parent);

    QTableWidget* ipAddressTable{ nullptr };
    QLineEdit* ipAddressInput{ nullptr };
    QLineEdit* portInput{ nullptr };
    QPushButton* submitButton{ nullptr };
};

