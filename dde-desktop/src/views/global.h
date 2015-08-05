#ifndef GLOBAL_H
#define GLOBAL_H


#include <QtCore>
#include <Logger.h>
#include "widgets/singleton.h"
#include "gridmanager.h"
#include "signalmanager.h"
#include "controllers/dbuscontroller.h"

#define gridManager  Singleton<GridManager>::instance()
#define signalManager  Singleton<SignalManager>::instance()
#define dbusController Singleton<DBusController>::instance()
#define desktopLocation QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0)

#define defaut_icon ":/skin/images/QFramer.png"
#define RichDirPrefix ".deepin_rich_dir_"
#define AppSuffix ".desktop"

#define ComputerUrl "computer://"
#define TrashUrl "trash://"
#define FilePrefix "file://"


QString decodeUrl(QString url);
QString deleteFilePrefix(QString path);
bool isDesktop(QString url);
bool isAppGroup(QString url);
bool isApp(QString url);
bool isAllApp(QStringList urls);
bool isComputer(QString url);
bool isTrash(QString url);
bool isFolder(QString url);
bool isInDesktop(QString url);
bool isDesktopAppFile(QString url);
QString formatURl(QString url);
#endif // GLOBAL_H

