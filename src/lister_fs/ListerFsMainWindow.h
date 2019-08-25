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
    void slotSend(const QByteArray& arr);

private:
    QWebSocket* m_webSocket;

    void closed();
    void onConnected();
    void onBinaryMessageReceived(const QByteArray& message);

signals:
    void send(const QByteArray&);
};
