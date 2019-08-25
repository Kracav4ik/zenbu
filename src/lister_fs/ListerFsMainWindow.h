#pragma once

#include <QMainWindow>

#include "ui_listerFs.h"

class QWebSocket;

class ListerFsMainWindow : public QMainWindow, private Ui::ListerFsMainWindow {
Q_OBJECT
public:
    ListerFsMainWindow();
    ~ListerFsMainWindow();

private slots:
    void slotAddRoot();
    void slotSend(const QJsonDocument& doc);
    void slotUpdatePathsList(const QJsonArray& arr);
    void slotUpdateRootsList(const QJsonArray& arr);
    void takeAllData();

signals:
    void sendMainThread(const QJsonDocument&);
    void updatePathsList(const QJsonArray&);
    void updateRootsList(const QJsonArray&);

private:
    QWebSocket* m_webSocket;

    void send(const QJsonObject& obj) const;
    void closed();
    void onConnected();
    void onBinaryMessageReceived(const QByteArray& message);
    void updateArrayToList(QListWidget* list, const QJsonArray& arr) const;
};
