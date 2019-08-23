#pragma once

#include <QMainWindow>

#include "ui_crc.h"

class CrcMainWindow : public QMainWindow, private Ui::CrcMainWindow {
Q_OBJECT
public:
    CrcMainWindow();
    ~CrcMainWindow();
};
