#include "views/desktopframe.h"
#include <QApplication>

void test() {
    for (int i = 0; i < 10000; i ++) {
        QPixmap p("1.png");
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DesktopFrame* w = new DesktopFrame;
    w->show();

    return a.exec();
}
