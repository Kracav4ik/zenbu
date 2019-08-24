#include "ListerFsMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>

ListerFsMainWindow::ListerFsMainWindow()
: m_webSocket(new QWebSocket("Lister"))
{
    setupUi(this);

    connect(m_webSocket, &QWebSocket::connected, this, &ListerFsMainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &ListerFsMainWindow::closed);
    m_webSocket->open(QString("ws://localhost:%1").arg(PORT));

    show();
}

void ListerFsMainWindow::onConnected() {
    qDebug() << "WebSocket connected";
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &ListerFsMainWindow::onTextMessageReceived);
    m_webSocket->sendTextMessage("Hello, world!");
}

void ListerFsMainWindow::closed() {
    qDebug() << "WebSocket connection closed";
}

void ListerFsMainWindow::onTextMessageReceived(const QString& message) {
    qDebug() << "Message received:" << message;
}

ListerFsMainWindow::~ListerFsMainWindow() = default;
