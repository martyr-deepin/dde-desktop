#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#include <QtCore>
#include "views/gridmanager.h"

class DesktopBox;
class AppController;

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
    DesktopBox* m_desktopBox;
    AppController* m_appController;
};

#endif // DESKTOPAPP_H
