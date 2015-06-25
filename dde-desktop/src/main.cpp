#include "views/desktopframe.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DesktopFrame* w = new DesktopFrame;
    w->show();

    return a.exec();
}
