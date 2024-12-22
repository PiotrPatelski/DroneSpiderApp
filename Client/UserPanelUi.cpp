#include "UserPanelUi.hpp"
#include <cassert>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

static constexpr QSizePolicy buttonSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
static constexpr int buttonWidth{ 100 };

enum LayoutId
{
    DESCRIPTION_LABEL = 0,
    STATUS_TABLE = 1,
    CONTROL_LABEL = 2,
    CHANGE_MODE_BUTTON = 3,
    CONTROL_GRID = 4
};

enum StatusTableColumn
{
    ID = 0,
    STATUS = 1,
    POSITION = 2,
    ORIENTATION = 3
};

UserPanelUi::UserPanelUi(QWidget* parent)
{
    mainLayout = createUi(parent);
}

void UserPanelUi::switchToProceduralWidget()
{
    directModeWidget->hide();
    mainLayout->removeWidget(directModeWidget);
    mainLayout->addWidget(proceduralModeWidget);
    proceduralModeWidget->show();
}

void UserPanelUi::switchToDirectWidget()
{
    proceduralModeWidget->hide();
    mainLayout->removeWidget(proceduralModeWidget);
    mainLayout->addWidget(directModeWidget);
    directModeWidget->show();
}

std::vector<unsigned short> UserPanelUi::getSelectedDroneIds() const {
    const auto selectedRows = statusTable->selectionModel()->selectedRows();
    std::vector<unsigned short> selectedDroneIds;
    for (const auto& rowIndex : selectedRows) {
        const auto* idField = statusTable->item(rowIndex.row(), StatusTableColumn::ID);
        assert(idField && "idField should not be nullptr");
        const auto droneId = idField->text().toUShort();
        selectedDroneIds.push_back(droneId);
    }

    return selectedDroneIds;
}

QVBoxLayout* UserPanelUi::createUi(QWidget* parent)
{
    auto* descriptionLabel = new QLabel("Select unit from table below, you wish to command.", parent);
    descriptionLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    statusTable = createStatusTable(parent);

    auto* controlDescription = new QLabel("Use procedures below to send specific commands or change mode for direct control", parent);

    changeModeButton = new QPushButton("Change Mode", parent);
    changeModeButton->setSizePolicy(buttonSizePolicy);
    changeModeButton->setFixedWidth(buttonWidth);

    setupProceduralModeButtons(parent);
    setupDirectModeButtons(parent);

    proceduralModeWidget = createProceduralModeWidget(parent);
    directModeWidget = createDirectModeWidget(parent);

    auto* mainLayout = new QVBoxLayout(parent);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(statusTable);
    mainLayout->addWidget(controlDescription, Qt::AlignLeft);
    mainLayout->addWidget(changeModeButton, Qt::AlignRight);
    mainLayout->addWidget(proceduralModeWidget);

    mainLayout->setStretch(LayoutId::DESCRIPTION_LABEL, 0);
    mainLayout->setStretch(LayoutId::STATUS_TABLE, 1);
    mainLayout->setStretch(LayoutId::CONTROL_LABEL, 1);
    mainLayout->setStretch(LayoutId::CHANGE_MODE_BUTTON, 1);
    mainLayout->setStretch(LayoutId::CONTROL_GRID, 1);
    mainLayout->setSpacing(10);

    return mainLayout;
}

QTableWidget* UserPanelUi::createStatusTable(QWidget* parent)
{
    auto* statusTable = new QTableWidget(0, 4, parent);
    statusTable->setHorizontalHeaderLabels({ "ID", "Status", "Position", "Orientation"});
    statusTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    statusTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    statusTable->verticalHeader()->setVisible(false);
    statusTable->setSelectionBehavior(QTableWidget::SelectRows);
    statusTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    return statusTable;
}

QWidget* UserPanelUi::createProceduralModeWidget(QWidget* parent)
{
    auto* proceduralModeLayout = new QGridLayout(parent);
    proceduralModeLayout->addWidget(moveButton, 2, 1, 1, 1);
    proceduralModeLayout->addWidget(rotateButton, 2, 2, 1, 1);
    proceduralModeLayout->addWidget(echoButton, 2, 3, 1, 1);
    proceduralModeLayout->addWidget(shutdownButton, 2, 4, 1, 1);

    auto* proceduralModeWidget = new QWidget(parent);
    proceduralModeWidget->setLayout(proceduralModeLayout);
    return proceduralModeWidget;
}

QWidget* UserPanelUi::createDirectModeWidget(QWidget* parent)
{
    auto* directModeLayout = new QGridLayout(parent);
    directModeLayout->addWidget(upButton, 0, 1, 1, 1);
    directModeLayout->addWidget(leftButton, 1, 0, 1, 1);
    directModeLayout->addWidget(downButton, 1, 1, 1, 1);
    directModeLayout->addWidget(rightButton, 1, 2, 1, 1);

    auto* directModeWidget = new QWidget(parent);
    directModeWidget->setLayout(directModeLayout);
    return directModeWidget;
}

void UserPanelUi::setupProceduralModeButtons(QWidget* parent)
{
    moveButton = new QPushButton("Move", parent);
    rotateButton = new QPushButton("Rotate", parent);
    echoButton = new QPushButton("Echo", parent);
    shutdownButton = new QPushButton("Shutdown", parent);

    moveButton->setSizePolicy(buttonSizePolicy);
    rotateButton->setSizePolicy(buttonSizePolicy);
    echoButton->setSizePolicy(buttonSizePolicy);
    shutdownButton->setSizePolicy(buttonSizePolicy);

    moveButton->setFixedWidth(buttonWidth);
    rotateButton->setFixedWidth(buttonWidth);
    echoButton->setFixedWidth(buttonWidth);
    shutdownButton->setFixedWidth(buttonWidth);
}

void UserPanelUi::setupDirectModeButtons(QWidget* parent)
{
    upButton = new QPushButton("Up", parent);
    downButton = new QPushButton("Down", parent);
    leftButton = new QPushButton("Left", parent);
    rightButton = new QPushButton("Right", parent);

    upButton->setSizePolicy(buttonSizePolicy);
    downButton->setSizePolicy(buttonSizePolicy);
    leftButton->setSizePolicy(buttonSizePolicy);
    rightButton->setSizePolicy(buttonSizePolicy);

    upButton->setFixedWidth(buttonWidth);
    downButton->setFixedWidth(buttonWidth);
    leftButton->setFixedWidth(buttonWidth);
    rightButton->setFixedWidth(buttonWidth);
}

unsigned short UserPanelUi::getDroneIdAtStatusEntry(const int entryIndex) const {
    return statusTable->item(entryIndex, 0)->text().toUShort();
}

void UserPanelUi::updateStatusEntry(const int row, const DroneContext& drone) {
    statusTable->setItem(row, 0, new QTableWidgetItem(QString::number(drone.id))); // ID
    statusTable->setItem(row, 1, new QTableWidgetItem(QString::number(static_cast<int>(drone.status)))); // Status
    statusTable->setItem(row, 2, new QTableWidgetItem(QString("(%1, %2)").arg(drone.position.x).arg(drone.position.y))); // Position
    statusTable->setItem(row, 3, new QTableWidgetItem(QString::number(drone.orientation))); // Orientation
}

void UserPanelUi::addNewStatusEntry(const DroneContext& drone) {
    const auto newRow = statusTable->rowCount();
    statusTable->insertRow(newRow);
    updateStatusEntry(newRow, drone);
}

void UserPanelUi::removeStatusEntry(const int entryIndex) {
    statusTable->removeRow(entryIndex);
}


