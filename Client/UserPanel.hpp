#pragma once

#include <QWidget>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "UserPanelUi.hpp"
#include "UserConnectionProxy.hpp"
#include "MoveRequestInputWindow.hpp"
#include "RotateRequestInputWindow.hpp"
#include "Logger.hpp"
#include "UnitContext.hpp"

enum Mode
{
	PROCEDURAL,
	DIRECT
};

enum MessageResult
{
	RECV_SUCCESS,
	RECV_ERROR,
	RECV_FAIL
};

class UserPanel : public QWidget
{
	Q_OBJECT

public:
	UserPanel(const unsigned short userId, std::unique_ptr<UserConnectionProxy> connection, Logger* logger);
	~UserPanel();
	void ProceedMove(const int xPosition, const int yPosition, const std::vector<unsigned short>& selectedDroneIds);
	void ProceedRotate(const int angle, const std::vector<unsigned short>& selectedDroneIds);

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void onMove();
    void onRotate();
    void onEcho();
    void onShutdown();
    void onMoveUp();
    void onMoveDown();
    void onMoveLeft();
    void onMoveRight();
	void switchMode();

private:
	void switchToProceduralMode();
	void switchToDirectMode();
	void setupLoggerParams();
	void runStatusUpdateThread();
	void handleUdpMessages();
	void updateStatusProcedure();
	void updateStatusTable(const std::vector<DroneContext>& drones);
	MessageResult handleMessage(const EchoCfm& echoCfm);
	MessageResult handleMessage(const StatusInd& statusInd);
	MessageResult handleMessage(const MoveDone& moveDone);
    MessageResult handleMessage(const RotateDone& rotateDone);
    MessageResult handleMessage(const ShutdownServerCfm& shutdownServerCfm);
    MessageResult handleMessage(const ConnectionLostInd& connectionLostInd);
	std::unordered_set<unsigned short> getDroneIds(const std::vector<DroneContext>& drones);
	std::unordered_map<unsigned short, unsigned short> mapEntryIndicesToDroneIds();
	void removeEntriesNotMatchingWithIncoming(const std::unordered_set<unsigned short>& incomingIds);
	void updateMissingAndExistingEntriesToMatchIncoming(const std::vector<DroneContext>& drones);
	void close();

	UserPanelUi ui;
	Mode currentMode{};
	unsigned short userId{ 0 };
	bool checkStatus{ true };

	Logger* logger{ nullptr };
	std::unique_ptr<UserConnectionProxy> connection{ nullptr };

	std::thread statusThread;
};