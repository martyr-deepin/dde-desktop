#ifndef GLOBAL_H
#define GLOBAL_H


#include <QtCore>
#include "widgets/singleton.h"
#include "gridmanager.h"
#include "signalmanager.h"
#include "controllers/dbuscontroller.h"

#define gridManager  Singleton<GridManager>::instance()
#define signalManager  Singleton<SignalManager>::instance()
#define dbusController Singleton<DBusController>::instance()
#define desktopLocation QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0)

#define defaut_icon ":/images/skin/images/default.png"
#define defaut_computerIcon ":/images/skin/images/computer.png"
#define defaut_trashIcon ":/images/skin/images/user-trash-full.png"
#define RichDirPrefix ".deepin_rich_dir_"
#define AppSuffix ".desktop"

#define ComputerUrl "computer://"
#define TrashUrl "trash://"
#define FilePrefix "file://"

const QSet<QString> SupportMimeTypes{"image/png" ,"image/jpeg" , "image/gif"
                                      , "image/bmp", "image/tiff" , "image/svg+xml"
                                      , "application/pdf"
                                    };
QString getThumbnailsPath();
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
bool isInDesktopFolder(QString url);
bool isDesktopAppFile(QString url);

bool isRequestThumbnail(QString url);
QString getMimeTypeGenericIconName(QString url);
QString getMimeTypeIconName(QString url);
QString getMimeTypeName(QString url);
#endif // GLOBAL_H

