#include "UserPanel.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"
#include "Message.hpp"
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QCoreApplication>

UserPanel::UserPanel(const unsigned short userId, std::unique_ptr<UserConnectionProxy> connection, Logger* logger)
	: QWidget(nullptr),
    ui(this),
    userId{ userId },
    connection( std::move(connection) ),
    logger{ logger }
{
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle("User control panel");

    connect(ui.getChangeModeButton(), &QPushButton::clicked, this, &UserPanel::switchMode);
    connect(ui.getMoveButton(), &QPushButton::clicked, this, &UserPanel::onMove);
    connect(ui.getRotateButton(), &QPushButton::clicked, this, &UserPanel::onRotate);
    connect(ui.getEchoButton(), &QPushButton::clicked, this, &UserPanel::onEcho);
    connect(ui.getShutdownButton(), &QPushButton::clicked, this, &UserPanel::onShutdown);
    connect(ui.getUpButton(), &QPushButton::clicked, this, &UserPanel::onMoveUp);
    connect(ui.getDownButton(), &QPushButton::clicked, this, &UserPanel::onMoveDown);
    connect(ui.getLeftButton(), &QPushButton::clicked, this, &UserPanel::onMoveLeft);
    connect(ui.getRightButton(), &QPushButton::clicked, this, &UserPanel::onMoveRight);

    switchToProceduralMode();

    setupLoggerParams();

    runStatusUpdateThread();
}

UserPanel::~UserPanel()
{
}

void UserPanel::runStatusUpdateThread()
{
    statusThread = std::thread(&UserPanel::updateStatusProcedure, this);
}


void UserPanel::updateStatusProcedure()
{
    std::this_thread::sleep_for(std::chrono::seconds(10));
    while (checkStatus)
    {
        logger->addLog("User sends STATUS_REQUEST!");
        connection->sendMessage(StatusRequest{ "STATUS_REQUEST", userId });
        const auto receivedMessage = connection->receiveMessage();
        if (receivedMessage)
        {
            std::visit([this](auto&& message)
                {
                    if (handleMessage(message) == MessageResult::RECV_SUCCESS)
                    {
                        logger->addLog("Received STATUS_IND!");
                    }
                }, *receivedMessage);
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

void UserPanel::ProceedMove(const int xPosition, const int yPosition, const std::vector<unsigned short>& selectedDroneIds)
{
    logger->addLog("Starting Move procedure...");
    logger->addLog(
        QString("User sends MOVE_REQUEST: \nuserId: %1,\ndronesAmount: %2,\ndroneId: %3,\nxPos: %4,\nyPos: %5")
        .arg(userId)
        .arg(selectedDroneIds.size())
        .arg(selectedDroneIds[0])
        .arg(xPosition)
        .arg(yPosition));
    connection->sendMessage(MoveRequest{ "MOVE_REQUEST", userId, selectedDroneIds, xPosition, yPosition });
    const auto receivedMessage = connection->receiveMessage();
    if (receivedMessage)
    {
        std::visit([this](auto&& message)
            {
                if (handleMessage(message) == MessageResult::RECV_SUCCESS)
                {
                    logger->addLog("Move procedure finished successfully!");
                }
            }, *receivedMessage);
    }
}

void UserPanel::ProceedRotate(const int angle, const std::vector<unsigned short>& selectedDroneIds)
{
    logger->addLog("Starting Rotate procedure...");
    connection->sendMessage(RotateRequest{ "ROTATE_REQUEST", userId, selectedDroneIds, angle });
    const auto receivedMessage = connection->receiveMessage();
    if (receivedMessage)
    {
        std::visit([this](auto&& message)
            {
                if (handleMessage(message) == MessageResult::RECV_SUCCESS)
                {
                    logger->addLog("Rotate procedure finished successfully!");
                }
            }, *receivedMessage);
    }
}

void UserPanel::onMove()
{
    const auto selectedDroneIds = ui.getSelectedDroneIds();
    if (selectedDroneIds.empty())
    {
        QMessageBox::warning(this, "Invalid Move Request", "No drone selected!");
        return;
    }
    auto inputWindow = new MoveRequestInputWindow(this);

    connect(inputWindow, &MoveRequestInputWindow::positionSubmitted, this, 
        [this, selectedDroneIds](const std::pair<int, int> position)
        { 
            const auto [xPosition, yPosition] = position;
            ProceedMove(xPosition, yPosition, selectedDroneIds); 
        });
    inputWindow->show();
}

void UserPanel::onRotate()
{
    const auto selectedDroneIds = ui.getSelectedDroneIds();
    if (selectedDroneIds.empty())
    {
        QMessageBox::warning(this, "Invalid Rotate Request", "No drone selected!");
        return;
    }
    auto inputWindow = new RotateRequestInputWindow(this);

    connect(inputWindow, &RotateRequestInputWindow::angleSubmitted, this,
        [this, selectedDroneIds](const int angle)
        { ProceedRotate(angle, selectedDroneIds); });
    inputWindow->show();
}

void UserPanel::onEcho()
{
    logger->addLog("Starting Echo procedure...");
    connection->sendMessage(EchoRequest{ "ECHO_REQUEST", userId });
    const auto receivedMessage = connection->receiveMessage();
    if (receivedMessage)
    {
        std::visit([this](auto&& message)
            {
                if (handleMessage(message) == MessageResult::RECV_SUCCESS)
                {
                    logger->addLog("Echo procedure finished successfully!");
                }
            }, *receivedMessage);
    }
}

void UserPanel::onShutdown()
{
    logger->addLog("Starting Shutdown procedure...");
    connection->sendMessage(ShutdownServerCommand{ "ADMIN_INITIATED_SHUTDOWN_COMMAND", userId });
    const auto receivedMessage = connection->receiveMessage();
    if (receivedMessage)
    {
        std::visit([this](auto&& message)
            {
                if (handleMessage(message) == MessageResult::RECV_SUCCESS)
                {
                    logger->addLog("Server answered shutdown, closing client!");
                    close();
                }
            }, *receivedMessage);
    }
}

void UserPanel::onMoveUp()
{
    const auto selectedDroneIds = ui.getSelectedDroneIds();
    if (selectedDroneIds.size() != 1)
    {
        QMessageBox::warning(this, "Invalid Move Up", "No drone selected or selected more than one!");
        return;
    }
    const auto& targetId = selectedDroneIds[0];
    logger->addLog("Moving Up directly...");
    connection->sendMessage(MoveUp{ "MOVE_UP", userId, targetId });
}

void UserPanel::onMoveDown()
{
    const auto selectedDroneIds = ui.getSelectedDroneIds();
    if (selectedDroneIds.size() != 1)
    {
        QMessageBox::warning(this, "Invalid Move Down", "No drone selected or selected more than one!");
        return;
    }
    const auto& targetId = selectedDroneIds[0];
    logger->addLog("Moving Down directly...");
    connection->sendMessage(MoveDown{ "MOVE_DOWN", userId, targetId });
}

void UserPanel::onMoveLeft()
{
    const auto selectedDroneIds = ui.getSelectedDroneIds();
    if (selectedDroneIds.size() != 1)
    {
        QMessageBox::warning(this, "Invalid Move Left", "No drone selected or selected more than one!");
        return;
    }
    const auto& targetId = selectedDroneIds[0];
    logger->addLog("Moving Left directly...");
    connection->sendMessage(MoveLeft{ "MOVE_LEFT", userId, targetId });
}

void UserPanel::onMoveRight()
{
    const auto selectedDroneIds = ui.getSelectedDroneIds();
    if (selectedDroneIds.size() != 1)
    {
        QMessageBox::warning(this, "Invalid Move Right", "No drone selected or selected more than one!");
        return;
    }
    const auto& targetId = selectedDroneIds[0];
    logger->addLog("Moving Right directly...");
    connection->sendMessage(MoveRight{ "MOVE_RIGHT", userId, targetId });
}

void UserPanel::switchMode()
{
    if (currentMode == Mode::PROCEDURAL)
    {
        switchToDirectMode();
    }
    else
    {
        switchToProceduralMode();
    }
}

void UserPanel::switchToProceduralMode()
{
    ui.switchToProceduralWidget();
    currentMode = Mode::PROCEDURAL;
}

void UserPanel::switchToDirectMode()
{
    ui.switchToDirectWidget();
    currentMode = Mode::DIRECT;
}

void UserPanel::setupLoggerParams()
{
    if (logger)
    {
        logger->setParent(this);
        logger->setWindowFlag(Qt::Window, true);
        logger->resize(400, 300);
        logger->show();
    }
}

MessageResult UserPanel::handleMessage(const EchoCfm& echoCfm)
{
    logger->addLog(QString("Handling received message: %1")
        .arg(QString::fromStdString(echoCfm.messageHeader)));
    return MessageResult::RECV_SUCCESS;
}

MessageResult UserPanel::handleMessage(const StatusInd& statusInd)
{
    logger->addLog(QString("Status received for connected drones amount: %1").arg(statusInd.drones.size()));
    updateStatusTable(statusInd.drones);
    return MessageResult::RECV_SUCCESS;
}

MessageResult UserPanel::handleMessage(const MoveDone& moveDone)
{
    logger->addLog(QString("Move procedure for drone id: %1, finished with cause: %2")
        .arg(moveDone.senderId)
        .arg(QString::fromStdString(resultToStr(moveDone.result))));
    return MessageResult::RECV_SUCCESS;
}

MessageResult UserPanel::handleMessage(const RotateDone& rotateDone)
{
    logger->addLog(QString("Rotate procedure for drone id: %1, finished with cause: %2")
        .arg(rotateDone.senderId)
        .arg(QString::fromStdString(resultToStr(rotateDone.result))));
    return MessageResult::RECV_SUCCESS;
}

MessageResult UserPanel::handleMessage(const ShutdownServerCfm& shutdownServerCfm)
{
    logger->addLog(QString("TcpServer shutdown success: %1")
        .arg(QString::fromStdString(shutdownServerCfm.messageHeader)));
    checkStatus = false;
    connection->close(shutdownServerCfm.messageHeader);

    return MessageResult::RECV_SUCCESS;
}

MessageResult UserPanel::handleMessage(const ConnectionLostInd& connectionLostInd)
{
    logger->addLog(QString("Closing connection due to : %1 received from server")
        .arg(QString::fromStdString(connectionLostInd.messageHeader)));
    checkStatus = false;
    connection->close(connectionLostInd.messageHeader);

    return MessageResult::RECV_ERROR;
}


std::unordered_set<unsigned short> UserPanel::getDroneIds(const std::vector<DroneContext>& drones) {
    std::unordered_set<unsigned short> droneIdsInUpdate;
    for (const auto& drone : drones) {
        droneIdsInUpdate.insert(drone.id);
    }
    return droneIdsInUpdate;
}

std::unordered_map<unsigned short, unsigned short> UserPanel::mapEntryIndicesToDroneIds() {
    std::unordered_map<unsigned short, unsigned short> entryIndexByDroneId;
    for (unsigned short entryIndex = 0; entryIndex < ui.getStatusTableSize(); ++entryIndex) {
        const auto tableDroneId = ui.getDroneIdAtStatusEntry(entryIndex);
        entryIndexByDroneId[tableDroneId] = entryIndex;
    }
    return entryIndexByDroneId;
}

void UserPanel::removeEntriesNotMatchingWithIncoming(const std::unordered_set<unsigned short>& incomingIds)
{
    for (short entryIndex = ui.getStatusTableSize() - 1; entryIndex >= 0; --entryIndex) {
        const auto entryDroneId = ui.getDroneIdAtStatusEntry(entryIndex);
        if (incomingIds.find(entryDroneId) == incomingIds.end()) {
            ui.removeStatusEntry(entryIndex);
        }
    }
}

void UserPanel::updateMissingAndExistingEntriesToMatchIncoming(const std::vector<DroneContext>& drones)
{
    logger->addLog(QString("UserPanel:: updateMissingAndExistingEntriesToMatchIncoming"));
    logger->addLog(QString("UserPanel:: drones are empty: %1").arg(drones.empty()));
    const auto entryIndexByDroneId = mapEntryIndicesToDroneIds();
    for (const auto& drone : drones) {
        logger->addLog(QString("UserPanel:: droneId: %1").arg(drone.id));
        const auto it = entryIndexByDroneId.find(drone.id);
        if (it != entryIndexByDroneId.end()) {
            logger->addLog(QString("UserPanel:: updating present droneId: %1").arg(drone.id));
            const auto entryIndex = it->second;
            ui.updateStatusEntry(entryIndex, drone);
        }
        else {
            logger->addLog(QString("UserPanel:: adding new droneId: %1").arg(drone.id));
            ui.addNewStatusEntry(drone);
        }
    }
}

void UserPanel::updateStatusTable(const std::vector<DroneContext>& drones) {
    const auto incomingDroneIds = getDroneIds(drones);
    removeEntriesNotMatchingWithIncoming(incomingDroneIds);

    updateMissingAndExistingEntriesToMatchIncoming(drones);
}

void UserPanel::close() {
    checkStatus = false;
    logger->addLog(QString("UserPanel:: closing"));
    if (statusThread.joinable())
    {
        statusThread.join();
    }
    logger->close();
}

void UserPanel::closeEvent(QCloseEvent* event)
{
    close();
    QWidget::closeEvent(event);
}