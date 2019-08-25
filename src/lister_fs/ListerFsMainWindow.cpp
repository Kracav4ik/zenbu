#include "ListerFsMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>
#include <QFileDialog>

ListerFsMainWindow::ListerFsMainWindow()
: m_webSocket(new QWebSocket(LISTER))
{
    setupUi(this);

    connect(m_webSocket, &QWebSocket::connected, this, &ListerFsMainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &ListerFsMainWindow::closed);
    connect(addRoot, &QPushButton::clicked, this, &ListerFsMainWindow::slotAddRoot);
    connect(this, &ListerFsMainWindow::sendMainThread, this, &ListerFsMainWindow::slotSend);
    connect(this, &ListerFsMainWindow::updatePathsList, this, &ListerFsMainWindow::slotUpdatePathsList);
    connect(this, &ListerFsMainWindow::updateRootsList, this, &ListerFsMainWindow::slotUpdateRootsList);
    connect(searchButton, &QPushButton::clicked, this, &ListerFsMainWindow::takeAllData);

    m_webSocket->open(QString("ws://localhost:%1").arg(PORT));

    show();
}

void ListerFsMainWindow::onConnected() {
    log("WebSocket connected");
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &ListerFsMainWindow::onBinaryMessageReceived);
    takeAllData();

    QJsonObject obj;
    obj["command"] = TAKE_UNTOUCHED_DIRS;
    obj["data"] = {};
    send(obj);
}

void ListerFsMainWindow::closed() {
    log("WebSocket connection closed");
}

void ListerFsMainWindow::onBinaryMessageReceived(const QByteArray& message) {
    log(QString("Message received: %1").arg(QString::fromUtf8(message)));

    struct Worker : public QRunnable {
        QByteArray message;
        ListerFsMainWindow& parent;
        Worker(QByteArray message, ListerFsMainWindow& parent): message(std::move(message)), parent(parent) {}

        void run() override {
            QJsonParseError er;

            QJsonDocument doc = QJsonDocument::fromJson(message, &er);
            if (er.error == QJsonParseError::NoError) {
                const QString& command = doc.object()["command"].toString();
                const QJsonArray& data = doc.object()["data"].toArray();

                if (command == TAKE_UNTOUCHED_DIRS) {
                    process(data);
                } else if (command == TAKE_ALL_DATA) {
                    emit parent.updatePathsList(data);
                } else if (command == ADD_ROOT) {
                    emit parent.updateRootsList(data);
                }
            } else {
                log("JSON PARSE ERROR!!:");
                log(QString::fromUtf8(message));
            }
        }

        void process(const QJsonArray& arr) {
            QJsonArray sendArr;
            for (const auto& val : arr) {
                QDir dir(val.toObject()["path"].toString());
                QJsonArray childrenArr;
                QJsonArray objArr;
                QString path = dir.path();
                for (const QString& entry : dir.entryList()) {
                    if (entry == "." || entry == "..") {
                        continue;
                    }
                    QJsonObject entryObj;
                    QString pathToFile = path + '/' + entry;
                    QFileInfo f(pathToFile);
                    entryObj["name"] = entry;
                    entryObj["path"] = pathToFile;
                    entryObj["isDir"] = f.isDir();
                    if (f.isDir()) {
                        entryObj["children"] = QJsonArray();
                    } else {
                        entryObj["size"] = f.size();
                    }
                    objArr.append(entryObj);
                    QJsonObject childObj;
                    childObj["path"] = pathToFile;
                    childrenArr.append(childObj);
                }
                QJsonObject dirObj;
                dirObj["path"] = path;
                dirObj["objects"] = objArr;
                dirObj["children"] = childrenArr;
                sendArr.append(dirObj);
            }
            QJsonObject obj;
            obj["command"] = TAKE_UNTOUCHED_DIRS;
            obj["data"] = sendArr;
            send(QJsonDocument(obj));
        }
        void send(const QJsonDocument& doc) {
            emit parent.sendMainThread(doc);
        }
    };

    QThreadPool::globalInstance()->start(new Worker(message, *this));
}

void ListerFsMainWindow::slotSend(const QJsonDocument& doc) {
    send(doc.object());
    takeAllData();
}

void ListerFsMainWindow::takeAllData() {
    QJsonObject obj;
    obj["command"] = TAKE_ALL_DATA;
    obj["data"] = search->text();
    send(obj);
}

void ListerFsMainWindow::slotUpdatePathsList(const QJsonArray& arr) {
    updateArrayToList(paths, arr);
}

void ListerFsMainWindow::slotUpdateRootsList(const QJsonArray& arr) {
    updateArrayToList(roots, arr);
}

void ListerFsMainWindow::updateArrayToList(QListWidget* list, const QJsonArray& arr) const {
    list->clear();
    for (const auto& item : arr) {
        list->addItem(item.toObject()["path"].toString());
    }
}

void ListerFsMainWindow::slotAddRoot() {
    QString path = QFileDialog::getExistingDirectory(this, "Select root dir");
    if (!path.isEmpty()) {
        QJsonObject obj;
        obj["command"] = ADD_ROOT;
        obj["data"] = path;
        roots->addItem(path);

        send(obj);
    }
}

void ListerFsMainWindow::send(const QJsonObject& obj) const {
    m_webSocket->sendBinaryMessage(QJsonDocument(obj).toJson());
}

ListerFsMainWindow::~ListerFsMainWindow() = default;
