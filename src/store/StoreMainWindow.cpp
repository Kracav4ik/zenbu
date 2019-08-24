#include "StoreMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>

StoreMainWindow::StoreMainWindow()
        : m_pWebSocketServer(new QWebSocketServer(
        QStringLiteral("Echo Server"),
        QWebSocketServer::NonSecureMode,
        this
)) {
    setupUi(this);

    if (m_pWebSocketServer->listen(QHostAddress::LocalHost, PORT)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &StoreMainWindow::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &StoreMainWindow::closed);
    }

    show();
}

void StoreMainWindow::onNewConnection() {
    QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();

    qDebug() << "Socked onNewConnection";

    connect(pSocket, &QWebSocket::textMessageReceived, this, &StoreMainWindow::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &StoreMainWindow::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &StoreMainWindow::socketDisconnected);
}

void StoreMainWindow::closed() {
    qDebug() << "Socked closed";
}

void StoreMainWindow::processBinaryMessage(QByteArray message) {
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void StoreMainWindow::processTextMessage(QString message) {
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    qDebug() << "Message received:" << message;

    if (pClient) {
        pClient->sendTextMessage(message);
    }
}

void StoreMainWindow::socketDisconnected() {
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        pClient->deleteLater();
    }
}

StoreMainWindow::~StoreMainWindow() = default;
