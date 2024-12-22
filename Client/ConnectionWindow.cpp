#include "ConnectionWindow.hpp"
#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QShortcut>
#include "UserPanel.hpp"
#include "Message.hpp"
#include "Serializer.hpp"
#include "Deserializer.hpp"

static bool isIpAddressValid(const QString& ipAddressValue)
{
    return QRegularExpression("^([0-9]{1,3}\\.){3}[0-9]{1,3}$").match(ipAddressValue).hasMatch();
}

static bool isPortValid(const QString& portValue)
{
    bool portHasDigitsOnly;
    const auto portNumber = portValue.toInt(&portHasDigitsOnly);
    return portHasDigitsOnly and portNumber >= 1 and portNumber <= 65535 and portValue.length() <= 5;
}

static Logger* createLogger(QWidget* parent)
{
    auto* logger = new Logger(parent);
    logger->resize(400, 300);
    logger->show();
    return logger;
}

ConnectionWindow::ConnectionWindow(QWidget *parent, std::shared_ptr<const std::map<QString, QString>> ipAddressEntryBase)
	: QWidget(parent, Qt::Window),
    ui(this),
    ipAddressEntryBase{ ipAddressEntryBase },
    tcpEndpoint{ std::make_unique<TcpClient>() }
{
    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle("Connection Window");

    connect(ui.getIpAddressTable(), &QTableWidget::itemSelectionChanged, this, &ConnectionWindow::onRowSelected);

    connect(ui.getSubmitButton(), &QPushButton::clicked, this, &ConnectionWindow::onSubmit);
    auto* enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(enterShortcut, &QShortcut::activated, this, &ConnectionWindow::onSubmit);

    fillIpAddressTableWithPredefinedBase(*ipAddressEntryBase);
}

ConnectionWindow::~ConnectionWindow()
{
}

void ConnectionWindow::closeEvent(QCloseEvent* event)
{
    emit dataFromLastSession(ipAddressEntriesSavedDuringThisSession);
    QWidget::closeEvent(event);
}

void ConnectionWindow::onRowSelected()
{
    const auto selectedEntry = ui.findSelectedEntry();
    if (selectedEntry)
    {
        ui.setInput(*selectedEntry);
    }
}

void ConnectionWindow::startConnectedService(const std::string& ipAddress, const unsigned short port, const unsigned short userId)
{
    if (loginPanel)
    {
        loginPanel->close();
    }
    auto udpEndpoint = std::make_unique<UdpClient>();
    logger->addLog(QString("Adding UDP endpoint for new User %1:%2").arg(QString::fromStdString(ipAddress)).arg(port + 1));
    udpEndpoint->addService(AF_INET, ipAddress, port + 1);
    auto* userPanel = new UserPanel(
        userId, 
        std::make_unique<UserConnectionProxy>(std::move(tcpEndpoint), std::move(udpEndpoint)), 
        std::move(logger));
    userPanel->resize(600, 400);
    userPanel->show();
}

void ConnectionWindow::handleLoginInput(const LoginData& loginData)
{
    tcpEndpoint->sendMessage(serialize(ServerPasswordInd{ "SERVER_PASSWORD_IND", 0, loginData.password}));
    const auto receivedMessage = tcpEndpoint->receiveMessage();

    const auto messageHeader = getMessageHeader(receivedMessage);

    if (messageHeader == "ADMIN_LOGIN_CFM")
    {
        auto deserializedMsg = deserializeAdminLoginCfm(receivedMessage);
        startConnectedService(loginData.ipAddress, loginData.port, deserializedMsg.userId);
    }
    else if(messageHeader == "ADMIN_PASSWORD_INVALID")
    {
        QMessageBox::warning(this, "Invalid Password", "Please enter a valid server password!");
    }
}

void ConnectionWindow::onSubmit()
{
    const auto ipAddressValue = ui.getIpAddress();
    const auto portValue = ui.getPort();
    if (not isUserInputValid(ipAddressValue, portValue))
    {
        return;
    }
    if (isUnique(ipAddressValue, portValue))
    {
        ui.addIpAddressEntry({ ipAddressValue, portValue });
        ipAddressEntriesSavedDuringThisSession[ipAddressValue] = portValue;
    }

    ui.clearInput();

    if (logger) {
        logger = nullptr;
    }
    logger = createLogger(this);
    logger->addLog("Attempting to connect to " + ipAddressValue + ":" + portValue);

    attemptToConnect(ipAddressValue, portValue);
}

void ConnectionWindow::runLoginPanel(const std::string& ipAddress, const unsigned short port)
{
    loginPanel = new LoginPanel(this, ipAddress, port);
    connect(loginPanel, &LoginPanel::loginAttempt, this, &ConnectionWindow::handleLoginInput);
    loginPanel->show();
}

void ConnectionWindow::loginToConnectedServer(const std::string& ipAddress, const unsigned short port)
{
    tcpEndpoint->sendMessage("ADMIN_LOGIN_REQUEST");
    const auto receivedMessage = tcpEndpoint->receiveMessage();

    const auto messageHeader = getMessageHeader(receivedMessage);

    if (messageHeader == "ADMIN_PASSWORD_REQUEST")
    {
        logger->addLog(QString("Server: %1:%2 requires password!").arg(QString::fromStdString(ipAddress)).arg(port));
        runLoginPanel(ipAddress, port);
    }
    else if (messageHeader == "ADMIN_LOGIN_CFM")
    {
        auto deserializedMsg = deserializeAdminLoginCfm(receivedMessage);
        startConnectedService(ipAddress, port, deserializedMsg.userId);
    }
}

void ConnectionWindow::attemptToConnect(const QString& ipAddressValue, const QString& portValue)
{
    const auto ipAddress = ipAddressValue.toStdString();
    const auto port = static_cast<unsigned short>(portValue.toInt());

    tcpEndpoint->addService(AF_INET);
    if (tcpEndpoint->connectToServer(ipAddress, port))
    {
        logger->addLog(QString("Connection with ip: %1, port: %2 - established successfully!").arg(ipAddressValue).arg(portValue));
        loginToConnectedServer(ipAddress, port);
    }
    else
    {
        logger->addLog(QString("Cannot connect to server with given ip: %1, port: %2").arg(ipAddressValue).arg(portValue));
    }
}

void ConnectionWindow::fillIpAddressTableWithPredefinedBase(const std::map<QString, QString>& ipAddressEntryBase)
{
    for (const auto& [ipAddress, port] : ipAddressEntryBase)
    {
        ui.addIpAddressEntry({ ipAddress, port });
    }
}

bool ConnectionWindow::isUserInputValid(const QString& ipAddressValue, const QString& portValue)
{
    if (ipAddressValue.isEmpty() or portValue.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill both IP address and port.");
        return false;
    }
    if (not isIpAddressValid(ipAddressValue)) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid IP address (e.g., 192.168.0.1).");
        return false;
    }
    if (not isPortValid(portValue)) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid port number (1-65535) with no more than 5 digits.");
        return false;
    }

    return true;
}

bool ConnectionWindow::isUnique(const QString& ipAddressValue, const QString& portValue)
{
    auto foundInCurrent = ipAddressEntriesSavedDuringThisSession.find(ipAddressValue);
    if (foundInCurrent != ipAddressEntriesSavedDuringThisSession.end() and foundInCurrent->second == portValue)
    {
        return false;
    }
    auto foundInBase = ipAddressEntryBase->find(ipAddressValue);
    if (foundInBase != ipAddressEntryBase->end() and foundInBase->second == portValue)
    {
        return false;
    }
    return true;
}
