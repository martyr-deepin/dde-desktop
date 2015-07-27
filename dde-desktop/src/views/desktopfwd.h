#ifndef DESKTOPFWD
#define DESKTOPFWD

#include <QtCore>
#include "griditem.h"
#include "desktopitem.h"

typedef QSharedPointer<GridItem> GridItemPointer;
typedef QList<GridItemPointer> GridItemPointerList;
typedef QSharedPointer<GridItemPointerList> GridListPointer;
typedef QList<GridListPointer> DoubleGridItemPointerList;

typedef QSharedPointer<DesktopItem> DesktopItemPointer;
#endif // DESKTOPFWD

