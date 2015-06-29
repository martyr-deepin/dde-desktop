#include "app/desktopapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DesktopApp* w = new DesktopApp;
    w->show();

    return a.exec();
}
