#pragma once

#include <QMainWindow>

#include "ui_crc.h"

class QWebSocket;

class CrcMainWindow : public QMainWindow, private Ui::CrcMainWindow {
Q_OBJECT
public:
    CrcMainWindow();
    ~CrcMainWindow();

private:
    QWebSocket* m_webSocket;

    void closed();
    void onConnected();
    void onTextMessageReceived(const QString& message);
};
