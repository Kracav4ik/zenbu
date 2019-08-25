#pragma once

#include <QTime>
#include <QDebug>

const int PORT = 2228;
const QString ADD_ROOT = "Add root";
const QString TAKE_ALL_DATA = "Take all data";
const QString TAKE_UNTOUCHED_DIRS = "Take untouched dirs";
const QString LISTER = "Lister";
const QString CRC = "CRC";

void log(QString text) {
    qDebug() << QString("%1.%2: %3").arg(QTime::currentTime().toString()).arg(QTime::currentTime().msec()).arg(text);
}
