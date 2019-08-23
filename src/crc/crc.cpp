#include <QApplication>
#include "CrcMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CrcMainWindow* window = new CrcMainWindow();
    return app.exec();
}
