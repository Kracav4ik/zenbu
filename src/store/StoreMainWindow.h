#pragma once

#include <QMainWindow>

#include "ui_store.h"

class QWebSocketServer;

class StoreMainWindow : public QMainWindow, private Ui::StoreMainWindow {
Q_OBJECT
public:
    StoreMainWindow();
    ~StoreMainWindow();

private:
    QWebSocketServer* m_pWebSocketServer;

    void onNewConnection();
    void closed();

    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);

    void socketDisconnected();
};
