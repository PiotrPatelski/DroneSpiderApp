#include "ConnectionWindowUi.hpp"
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

enum Id
{
    IP_ADDRESS_COLUMN = 0,
    PORT_COLUMN = 1,
};

ConnectionWindowUi::ConnectionWindowUi(QWidget* parent)    
    : ipAddressTable{ createIpAddressTable(parent) },
    ipAddressInput{ new QLineEdit(parent) },
    portInput{ new QLineEdit(parent) }
{
    parent->setAttribute(Qt::WA_DeleteOnClose);
    parent->setWindowTitle("Connection Window");

    auto* descriptionLabel = new QLabel("Type IP address and port to connect to desired unit.", parent);

    ipAddressInput->setPlaceholderText("Enter IP Address");
    portInput->setPlaceholderText("Enter Port");

    auto* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(ipAddressInput);
    inputLayout->addWidget(portInput);

    submitButton = new QPushButton("Submit", parent);

    auto* layout = new QVBoxLayout(parent);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(20);
    layout->addWidget(descriptionLabel);
    layout->addWidget(ipAddressTable);
    layout->addLayout(inputLayout);
    layout->addWidget(submitButton);
    parent->setLayout(layout);
}

std::optional<Entry> ConnectionWindowUi::findSelectedEntry()
{
    QList<QTableWidgetSelectionRange> selectedRanges = ipAddressTable->selectedRanges();

    if (!selectedRanges.isEmpty()) {
        const auto selectedRow = selectedRanges.first().topRow();

        const auto ipAddress = ipAddressTable->item(selectedRow, 0)->text();
        const auto port = ipAddressTable->item(selectedRow, 1)->text();
        return Entry{ ipAddress, port };
    }
    return std::nullopt;
}


void ConnectionWindowUi::addIpAddressEntry(const Entry& entry)
{
    auto* ipAddressEntry = new QTableWidgetItem(entry.ipAddress);
    auto* portEntry = new QTableWidgetItem(entry.port);

    ipAddressEntry->setFlags(ipAddressEntry->flags() & ~Qt::ItemIsEditable);
    portEntry->setFlags(portEntry->flags() & ~Qt::ItemIsEditable);

    const auto row = ipAddressTable->rowCount();
    ipAddressTable->insertRow(row);
    ipAddressTable->setItem(row, Id::IP_ADDRESS_COLUMN, ipAddressEntry);
    ipAddressTable->setItem(row, Id::PORT_COLUMN, portEntry);
}

void ConnectionWindowUi::setInput(const Entry& entry)
{
    ipAddressInput->setText(entry.ipAddress);
    portInput->setText(entry.port);
}

void ConnectionWindowUi::clearInput()
{
    ipAddressInput->clear();
    portInput->clear();
}

QTableWidget* ConnectionWindowUi::createIpAddressTable(QWidget* parent)
{
    auto* table = new QTableWidget(parent);
    table->setObjectName("ipAddressTable");
    table->setColumnCount(2);

    auto* ipAddressTableSection = new QTableWidgetItem("IP");
    auto* portTableSection = new QTableWidgetItem("Port");

    table->setHorizontalHeaderItem(Id::IP_ADDRESS_COLUMN, ipAddressTableSection);
    table->setHorizontalHeaderItem(Id::PORT_COLUMN, portTableSection);
    table->setSelectionBehavior(QTableWidget::SelectRows);
    table->verticalHeader()->setVisible(false);

    QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(1);
    sizePolicy.setHeightForWidth(table->sizePolicy().hasHeightForWidth());
    table->setSizePolicy(sizePolicy);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    return table;
}

