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

private slots:
    void updateMask();

private:
    QJsonArray* filesList;
    QJsonArray* rootsList;
    QWebSocketServer* m_pWebSocketServer;
    QMap<QString, QWebSocket*> clients;
    bool loaded = false;

    void onNewConnection();
    void closed();
    QJsonArray selectName(const QString& name) const;

    void saveJSON();
    void addChildren(const QString& path, const QJsonArray& children);

    void processTextMessage(const QString& message);
    void processBinaryMessage(const QByteArray& message);

    void socketDisconnected();
    void takeAllData(QWebSocket* client, const QString& name) const;
    void addRoot(const QString& path);
    void takeUntouchedDirs(const QJsonArray& arr);
    void send(QWebSocket* client, const QJsonObject& paths) const;
};
