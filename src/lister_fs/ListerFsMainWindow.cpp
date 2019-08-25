#include "ListerFsMainWindow.h"
#include "utils.h"
#include <QtWebSockets/QtWebSockets>

ListerFsMainWindow::ListerFsMainWindow()
: m_webSocket(new QWebSocket(LISTER))
{
    setupUi(this);

    connect(m_webSocket, &QWebSocket::connected, this, &ListerFsMainWindow::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, this, &ListerFsMainWindow::closed);
    connect(this, &ListerFsMainWindow::send, this, &ListerFsMainWindow::slotSend);
    m_webSocket->open(QString("ws://localhost:%1").arg(PORT));

    show();
}

void ListerFsMainWindow::onConnected() {
    log("WebSocket connected");
    connect(m_webSocket, &QWebSocket::binaryMessageReceived, this, &ListerFsMainWindow::onBinaryMessageReceived);
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
                process(doc);
            } else {
                log("JSON PARSE ERROR!!:");
                log(QString::fromUtf8(message));
            }
        }

        void process(const QJsonDocument& doc) {
            QJsonArray arr = doc.array();
            QJsonArray sendArr;
            for (const QJsonValueRef& val : arr) {
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
            send(QJsonDocument(sendArr));
        }
        void send(const QJsonDocument& doc) {
            emit parent.send(doc.toJson());
        }
    };

    QThreadPool::globalInstance()->start(new Worker(message, *this));
}

void ListerFsMainWindow::slotSend(const QByteArray& arr) {
    m_webSocket->sendBinaryMessage(arr);
}

ListerFsMainWindow::~ListerFsMainWindow() = default;
