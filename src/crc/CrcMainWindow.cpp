#include "CrcMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>

CrcMainWindow::CrcMainWindow()
: m_webSocket(new QWebSocket(CRC))
{
    setupUi(this);

    connect(m_webSocket, &QWebSocket::connected, this, &CrcMainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &CrcMainWindow::closed);
    m_webSocket->open(QString("ws://localhost:%1").arg(PORT));

    show();
}

void CrcMainWindow::onConnected() {
    log("WebSocket connected");
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &CrcMainWindow::onTextMessageReceived);
}

void CrcMainWindow::closed() {
    log("WebSocket connection closed");
}

void CrcMainWindow::onTextMessageReceived(const QString& message) {
    log(QString("Message received: %1").arg(message));
}

CrcMainWindow::~CrcMainWindow() = default;
