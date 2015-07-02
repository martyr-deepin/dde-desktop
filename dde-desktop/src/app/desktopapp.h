#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#include <QtCore>
#include "views/desktopframe.h"
#include "controllers/appcontroller.h"

class DesktopApp : public QObject
{
    Q_OBJECT
public:
    explicit DesktopApp(QObject *parent = 0);
    ~DesktopApp();

    void initConnect();


signals:

public slots:
    void loadSettings();
    void saveSettings();

    void saveGridOn(bool mode);
    void saveSizeType(SizeType type);
    void saveSortFlag(QDir::SortFlag flag);

    void show();

private:
    DesktopFrame* m_desktopFrame;
    AppController* m_appController;
};

#endif // DESKTOPAPP_H
