/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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

