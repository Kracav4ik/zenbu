#include "StoreMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>
#include <QMessageBox>
#include <QFileDialog>

const QString pathsFileName("paths.json");
const QString rootsFileName("roots.json");

StoreMainWindow::StoreMainWindow()
        : filesList(new QJsonArray())
        , rootsList(new QJsonArray())
        , m_pWebSocketServer(new QWebSocketServer(
        "Server",
        QWebSocketServer::NonSecureMode,
        this
)) {
    setupUi(this);

    if (m_pWebSocketServer->listen(QHostAddress::LocalHost, PORT)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &StoreMainWindow::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &StoreMainWindow::closed);
    }

    connect(searchButton, &QPushButton::clicked, this, &StoreMainWindow::updateMask);

    {
        QFile pathsLoadFile(pathsFileName);
        QFile rootsLoadFile(rootsFileName);
        if (pathsLoadFile.exists() && rootsLoadFile.exists()) {
            if (pathsLoadFile.open(QIODevice::ReadOnly) && rootsLoadFile.open(QIODevice::ReadOnly)) {
                loaded = true;
                QJsonDocument pathsDoc(QJsonDocument::fromJson(pathsLoadFile.readAll()));
                const QJsonArray& pathArr = pathsDoc.array();
                for (const QJsonValue& val : pathArr) {
                    filesList->append(val);
                    paths->addItem(val["path"].toString());
                }
                QJsonDocument rootsDoc(QJsonDocument::fromJson(rootsLoadFile.readAll()));
                const QJsonArray& rootsArr = rootsDoc.array();
                for (const QJsonValue& val : rootsArr) {
                    rootsList->append(val);
                    paths->addItem(val["path"].toString());
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
    clients[client->origin()] = client;
}

void StoreMainWindow::closed() {
    log("Socked closed");
}

void StoreMainWindow::processBinaryMessage(const QByteArray& message) {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    log(QString("Binary Message received: %1").arg(QString::fromUtf8(message)));

    QJsonParseError er;
    QJsonDocument doc = QJsonDocument::fromJson(message, &er);
    if (er.error != QJsonParseError::NoError) {
        log("JSON PARSE ERROR!!:");
        log(QString::fromUtf8(message));
        return;
    }

    QJsonObject inputObj = doc.object();
    QString command = inputObj["command"].toString();
    if (command == TAKE_UNTOUCHED_DIRS && client->origin() == LISTER) {
        takeUntouchedDirs(inputObj["data"].toArray());
    } else if (command == TAKE_ALL_DATA) {
        takeAllData(client, inputObj["data"].toString());
    } else if (command == ADD_ROOT) {
        addRoot(inputObj["data"].toString());
    }
}

void StoreMainWindow::takeUntouchedDirs(const QJsonArray& arr) {
    for (const auto& val : arr) {
        QJsonObject dirObj = val.toObject();
        addChildren(dirObj["path"].toString(), dirObj["children"].toArray());
        for (auto obj : dirObj["objects"].toArray()) {
            filesList->append(obj);
        }
    }
    saveJSON();
    QJsonArray sendArr;
    paths->clear();
    for (const auto& val : *filesList) {
        if (val.toObject()["isDir"].toBool() && val.toObject()["children"].toArray().empty()) {
            sendArr.append(val.toObject());
        }
        paths->addItem(val.toObject()["path"].toString());
    }
    if (!sendArr.isEmpty()) {
        QJsonObject sendObj;
        sendObj["command"] = TAKE_UNTOUCHED_DIRS;
        sendObj["data"] = sendArr;

        send(clients[LISTER], sendObj);
    }
}

void StoreMainWindow::addRoot(const QString& path) {
    QDir rootDir(path);
    QJsonObject rootObj;
    rootObj["name"] = rootDir.dirName();
    rootObj["path"] = rootDir.path();
    rootObj["isDir"] = true;
    rootObj["children"] = QJsonArray();
    filesList->append(rootObj);
    rootsList->append(rootObj);
    takeUntouchedDirs({});
}

void StoreMainWindow::takeAllData(QWebSocket* client, const QString& name) const {
    QJsonObject paths;
    paths["command"] = TAKE_ALL_DATA;
    paths["data"] = selectName(name);
    send(client, paths);

    QJsonObject roots;
    roots["command"] = ADD_ROOT;
    roots["data"] = *rootsList;
    send(client, roots);
}

void StoreMainWindow::send(QWebSocket* client, const QJsonObject& paths) const {
    client->sendBinaryMessage(QJsonDocument(paths).toJson());
}

void StoreMainWindow::saveJSON() {
    QJsonDocument pathsDoc(*filesList);
    QJsonDocument rootsDoc(*rootsList);
    QFile pathsSaveFile(pathsFileName);
    QFile rootsSaveFile(rootsFileName);

    if (!pathsSaveFile.open(QIODevice::WriteOnly) || !rootsSaveFile.open(QIODevice::WriteOnly)) {
        log("Couldn't open save file.");
        return;
    }

    rootsSaveFile.write(rootsDoc.toJson());
    pathsSaveFile.write(pathsDoc.toJson());
}

void StoreMainWindow::processTextMessage(const QString& message) {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    log(QString("Text Message received: %1").arg(message));
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

void StoreMainWindow::updateMask() {
    paths->clear();
    for (auto val : selectName(search->text())) {
        paths->addItem(val.toObject()["path"].toString());
    }
}

QJsonArray StoreMainWindow::selectName(const QString& name) const {
    QJsonArray arr;
    for (auto val : *filesList) {
        if (val.toObject()["name"].toString().contains(name)) {
            QJsonObject obj;
            obj["path"] = val.toObject()["path"];
            arr.append(obj);
        }
    }
    return arr;
}

StoreMainWindow::~StoreMainWindow() = default;
