#include "global.h"

QString decodeUrl(QString url){
    return QUrl(url).toString();
}

bool isAppGroup(QString url){
    if (decodeUrl(url).contains(desktopLocation)){
        if (QDir(url).dirName().startsWith(RichDirPrefix)){
            return true;
        }
    }
    return false;
}
