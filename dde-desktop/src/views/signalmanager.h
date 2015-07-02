#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "desktopitem.h"
#include "gridmanager.h"
#include <QtCore>


class SignalManager: public QObject
{
    Q_OBJECT
public:
    static SignalManager* instance();

signals:
    void moveActionFinished();
    void gridModeChanged(bool mode); /*true: grid on; false: grid off*/
    void gridOnResorted(); /*sort desktop item when grid on */

    void gridSizeTypeChanged(SizeType type);
    void sortedModeChanged(QDir::SortFlag flag);

    void contextMenuShowed(DesktopItemPointer pItem, QPoint pos);

private:
    SignalManager();
    ~SignalManager();
    static SignalManager* m_instance;
    Q_DISABLE_COPY(SignalManager)
};

#endif // SIGNALMANAGER_H
