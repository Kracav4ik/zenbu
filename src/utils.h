#pragma once

#include <QTime>
#include <QDebug>

const int PORT = 2228;
const QString LISTER = "Lister";
const QString CRC = "CRC";

void log(QString text) {
    qDebug() << QString("%1.%2: %3").arg(QTime::currentTime().toString()).arg(QTime::currentTime().msec()).arg(text);
}
