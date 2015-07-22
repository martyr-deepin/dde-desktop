#include "global.h"

QString decodeUrl(QString url){
    return QUrl(url).toString();
}

bool isAppGroup(QString url){
    url.replace("file://", "");
    if (decodeUrl(url).contains(desktopLocation)){
        if (QDir(url).dirName().startsWith(RichDirPrefix)){
            return true;
        }
    }
    return false;
}


bool isApp(QString url){
    if (url.endsWith(AppSuffix)){
        return true;
    }
    return false;
}

bool isComputer(QString url){
    if (url == ComputerUrl){
        return true;
    }
    return false;
}

bool isTrash(QString url){
    if (url == TrashUrl){
        return true;
    }
    return false;
}


bool isFolder(QString url){
    url.replace("file://", "");
    if (QDir(url).exists()){
        if (decodeUrl(url).contains(desktopLocation)){
            if (QDir(url).dirName().startsWith(RichDirPrefix)){
                return false;
            }
        }
        return true;
    }
    return false;
}
