#pragma once

#include <QMainWindow>

#include "ui_store.h"

class StoreMainWindow : public QMainWindow, private Ui::StoreMainWindow {
Q_OBJECT
public:
    StoreMainWindow();
    ~StoreMainWindow();
};
