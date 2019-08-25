#include "StoreMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>
#include <QMessageBox>
#include <QFileDialog>

const QString fileName("db.json");

StoreMainWindow::StoreMainWindow()
        : filesList(new QJsonArray())
        , m_pWebSocketServer(new QWebSocketServer(
        "Server",
        QWebSocketServer::NonSecureMode,
        this
)) {
    setupUi(this);

    if (m_pWebSocketServer->listen(QHostAddress::LocalHost, PORT)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &StoreMainWindow::onNewConnection, Qt::DirectConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &StoreMainWindow::closed);
    }

    {
        QFile loadFile(fileName);
        if (loadFile.exists()) {
            if (loadFile.open(QIODevice::ReadOnly)) {
                loaded = true;
                QJsonDocument doc(QJsonDocument::fromJson(loadFile.readAll()));
                const QJsonArray& arr = doc.array();
                for (const QJsonValue& val : arr) {
                    filesList->append(val);
                }
            } else {
                log("Couldn't open load file.");
            }
        }
    }

    saveJSON();

    show();
}

void StoreMainWindow::onNewConnection() {
    QWebSocket* client = m_pWebSocketServer->nextPendingConnection();

    log(QString("Socked onNewConnection %1").arg(client->origin()));

    connect(client, &QWebSocket::textMessageReceived, this, &StoreMainWindow::processTextMessage);
    connect(client, &QWebSocket::binaryMessageReceived, this, &StoreMainWindow::processBinaryMessage);
    connect(client, &QWebSocket::disconnected, this, &StoreMainWindow::socketDisconnected);
    clients.append(client);

    QJsonArray arr;
    if (filesList->empty() && client->origin() == LISTER) {
        QDir rootDir = QFileDialog::getExistingDirectory(this, "Select root dir");
        QJsonObject rootObj;
        rootObj["name"] = rootDir.dirName();
        rootObj["path"] = rootDir.path();
        rootObj["isDir"] = true;
        rootObj["children"] = QJsonArray();
        filesList->append(rootObj);
        arr = *filesList;
    } else {
        for (const QJsonValueRef& val : *filesList) {
            if (val.toObject()["isDir"].toBool() && val.toObject()["children"].toArray().empty()) {
                arr.append(val.toObject());
            }
        }
    }

    client->sendBinaryMessage(QJsonDocument(arr).toJson());
    saveJSON();
}

void StoreMainWindow::closed() {
    log("Socked closed");
}

void StoreMainWindow::processBinaryMessage(const QByteArray& message) {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    log(QString("Binary Message received: %1").arg(QString::fromUtf8(message)));

    QJsonArray arr = QJsonDocument::fromJson(message).array();
    QJsonArray sendArr;
    for (auto val : arr) {
        QJsonObject dirObj = val.toObject();
        addChildren(dirObj["path"].toString(), dirObj["children"].toArray());
        for (auto obj : dirObj["objects"].toArray()) {
            filesList->append(obj);
            if (obj.toObject()["isDir"].toBool()) {
                sendArr.append(obj);
            }
        }
    }
    if (!sendArr.empty()) {
        client->sendBinaryMessage(QJsonDocument(sendArr).toJson());
    }
    saveJSON();
}

void StoreMainWindow::saveJSON() {
    QJsonDocument doc(*filesList);
    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        log("Couldn't open save file.");
        return;
    }

    saveFile.write(doc.toJson());
}

void StoreMainWindow::processTextMessage(const QString& message) {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    log(QString("Binary Message received: %1").arg(message));
}

void StoreMainWindow::socketDisconnected() {
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    log(QString("socketDisconnected: %1").arg(pClient->origin()));
    if (pClient) {
        pClient->deleteLater();
    }
}

void StoreMainWindow::addChildren(const QString& path, const QJsonArray& children) {
    for (auto val : *filesList) {
        if (val.toObject()["path"].toString() == path) {
            QJsonObject obj = val.toObject();
            obj["children"] = children;
            val = obj;
        }
    }
}

StoreMainWindow::~StoreMainWindow() = default;
