#ifndef DESKTOPAPP_H
#define DESKTOPAPP_H

#include <QtCore>
#include "views/gridmanager.h"


class DesktopBox;
class AppController;
class DTaskDialog;

class DesktopApp : public QObject
{
    Q_OBJECT
public:
    explicit DesktopApp(QObject *parent = 0);
    ~DesktopApp();

    void initConnect();
    void registerDBusService();

signals:
    void closed();
    void shown();

public slots:
    void saveGridOn(bool mode);
    void saveSizeType(SizeType type);
    void saveSortFlag(QDir::SortFlags flag);

    void unRegisterDbusService();

    void show();
    void hide();
    void toggle();
    void exit();



private:
    DesktopBox* m_desktopBox;
    DTaskDialog* m_taskDialog;
};

#endif // DESKTOPAPP_H
