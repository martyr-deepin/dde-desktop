#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "desktopitem.h"
#include <QtCore>


class SignalManager: public QObject
{
    Q_OBJECT
public:
    static SignalManager* instance();

signals:
    void girdModeChanged(bool mode); /*true: grid on; false: grid off*/
    void contextMenuShowed(DesktopItemType type, QPoint pos);

private:
    SignalManager();
    ~SignalManager();
    static SignalManager* m_instance;
    Q_DISABLE_COPY(SignalManager)
};

#endif // SIGNALMANAGER_H
