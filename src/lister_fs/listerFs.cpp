#include <QApplication>
#include "ListerFsMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    ListerFsMainWindow* window = new ListerFsMainWindow();
    return app.exec();
}
