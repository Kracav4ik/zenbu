#include <QApplication>
#include "StoreMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    StoreMainWindow* window = new StoreMainWindow();
    return app.exec();
}
