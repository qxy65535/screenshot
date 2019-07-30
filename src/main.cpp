#include <QApplication>
#include <QDesktopWidget>

#include "screenshot.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ScreenShot *screenshot = new ScreenShot();
    screenshot->show();
    return app.exec();
}