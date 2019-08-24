#include "CrcMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>

CrcMainWindow::CrcMainWindow()
        : m_webSocket(new QWebSocket("CRC"))
{
    setupUi(this);

    connect(m_webSocket, &QWebSocket::connected, this, &CrcMainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &CrcMainWindow::closed);
    m_webSocket->open(QString("ws://localhost:%1").arg(PORT));

    show();
}

void CrcMainWindow::onConnected() {
    qDebug() << "WebSocket connected";
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &CrcMainWindow::onTextMessageReceived);
    m_webSocket->sendTextMessage("Hello, world!");
}

void CrcMainWindow::closed() {
    qDebug() << "WebSocket connection closed";
}

void CrcMainWindow::onTextMessageReceived(const QString& message) {
    qDebug() << "Message received:" << message;
}

CrcMainWindow::~CrcMainWindow() = default;
