#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore>
#include "gridmanager.h"
#include "signalmanager.h"
#include "controllers/dbuscontroller.h"

#define gridManager  GridManager::instance()
#define signalManager  SignalManager::instance()
#define dbusController DBusController::instance()
#define desktopLocation QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DesktopLocation).at(0)

#define defaut_icon ":/skin/images/QFramer.png"
#define RichDirPrefix ".deepin_rich_dir_"
#define AppSuffix ".desktop"

#define ComputerUrl "computer://"
#define TrashUrl "trash://"

QString decodeUrl(QString url);
bool isAppGroup(QString url);
bool isApp(QString url);

#endif // GLOBAL_H

