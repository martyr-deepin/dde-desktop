#include "views/launcherframe.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LauncherFrame* launcher = new LauncherFrame;
    launcher->show();

    return a.exec();
}
