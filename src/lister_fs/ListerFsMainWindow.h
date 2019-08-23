#pragma once

#include <QMainWindow>

#include "ui_listerFs.h"

class ListerFsMainWindow : public QMainWindow, private Ui::ListerFsMainWindow {
Q_OBJECT
public:
    ListerFsMainWindow();
    ~ListerFsMainWindow();
};
