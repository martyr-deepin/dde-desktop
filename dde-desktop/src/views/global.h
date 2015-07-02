#ifndef GLOBAL_H
#define GLOBAL_H


#include "gridmanager.h"
#include "signalmanager.h"

#define gridManager  GridManager::instance()
#define signalManager  SignalManager::instance()
#define desktopLocation QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DesktopLocation).at(0)
#define computer "Computer://"
#define trash "Trash://"
#define RichDirPrefix ".deepin_rich_dir_"

#endif // GLOBAL_H

