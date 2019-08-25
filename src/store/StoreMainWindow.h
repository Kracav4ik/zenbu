#pragma once

#include <QMainWindow>

#include "ui_store.h"

class QWebSocketServer;
class QWebSocket;

class StoreMainWindow : public QMainWindow, private Ui::StoreMainWindow {
Q_OBJECT
public:
    StoreMainWindow();
    ~StoreMainWindow();

private:
    QJsonArray* filesList;
    QWebSocketServer* m_pWebSocketServer;
    QList<QWebSocket*> clients;
    bool loaded = false;

    void onNewConnection();
    void closed();
    void saveJSON();
    void addChildren(const QString& path, const QJsonArray& children);

    void processTextMessage(const QString& message);
    void processBinaryMessage(const QByteArray& message);

    void socketDisconnected();
};
