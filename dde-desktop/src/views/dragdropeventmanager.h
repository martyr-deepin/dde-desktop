#ifndef DRAGDROPEVENTMANAGER_H
#define DRAGDROPEVENTMANAGER_H

#include <QtCore>
#include "desktopfwd.h"


class DragDropEventManager : public QObject
{
    Q_OBJECT
public:
    explicit DragDropEventManager(QObject *parent = 0);
    ~DragDropEventManager();

signals:

public slots:
    void handleDragMoveEvent(const QList<DesktopItemPointer>& items, QDragMoveEvent* event);
    void handleDropEvent(const QList<DesktopItemPointer>& items, QDropEvent* event);

private:
    DesktopItemPointer m_hoverDesktopItem = DesktopItemPointer(); /*desktop item which hover on*/
    DesktopItemPointer m_destinationDesktopItem = DesktopItemPointer(); /*desktop item which will drop*/
};

#endif // DRAGDROPEVENTMANAGER_H
