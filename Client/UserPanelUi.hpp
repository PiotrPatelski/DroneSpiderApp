#pragma once

#include <QAction>
#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QWidget>
#include <QPushButton>
#include "UnitContext.hpp"

class UserPanelUi
{
public:
    UserPanelUi(QWidget* parrent);
    void switchToProceduralWidget();
    void switchToDirectWidget();
    std::vector<unsigned short> getSelectedDroneIds() const;
    inline unsigned short getStatusTableSize() const { return static_cast<unsigned short>(statusTable->rowCount()); }
    unsigned short getDroneIdAtStatusEntry(const int entryIndex) const;
    void updateStatusEntry(const int entryIndex, const DroneContext& drone);
    void addNewStatusEntry(const DroneContext& drone);
    void removeStatusEntry(const int entryIndex);

    inline QPushButton* getChangeModeButton(){ return changeModeButton; }
    inline QPushButton* getMoveButton() { return moveButton; }
    inline QPushButton* getRotateButton() { return rotateButton; }
    inline QPushButton* getEchoButton() { return echoButton; }
    inline QPushButton* getShutdownButton() { return shutdownButton; }
    inline QPushButton* getUpButton() { return upButton; }
    inline QPushButton* getDownButton() { return downButton; }
    inline QPushButton* getLeftButton() { return leftButton; }
    inline QPushButton* getRightButton() { return rightButton; }

private:
    QVBoxLayout* createUi(QWidget* parent);
    QTableWidget* createStatusTable(QWidget* parent);
    QWidget* createProceduralModeWidget(QWidget* parent);
    QWidget* createDirectModeWidget(QWidget* parent);
    void setupProceduralModeButtons(QWidget* parent);
    void setupDirectModeButtons(QWidget* parent);

    QVBoxLayout* mainLayout{ nullptr };
    QTableWidget* statusTable{ nullptr };
    QWidget* proceduralModeWidget{ nullptr };
    QWidget* directModeWidget{ nullptr };
    QPushButton* changeModeButton{ nullptr };
    QPushButton* moveButton{ nullptr };
    QPushButton* rotateButton{ nullptr };
    QPushButton* echoButton{ nullptr };
    QPushButton* shutdownButton{ nullptr };
    QPushButton* upButton{ nullptr };
    QPushButton* downButton{ nullptr };
    QPushButton* leftButton{ nullptr };
    QPushButton* rightButton{ nullptr };
};

