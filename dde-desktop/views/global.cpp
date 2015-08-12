#include "global.h"

QString decodeUrl(QString url){
    return QUrl(url).toString();
}

QString deleteFilePrefix(QString path){
    QString ret = path.replace(FilePrefix, "");
    return decodeUrl(ret);
}

bool isDesktop(QString url){
    if (url.startsWith(FilePrefix)){
        url.replace(FilePrefix, "");
    }
    if (url == desktopLocation){
        return true;
    }
    return false;
}

bool isAppGroup(QString url){
    if (url.startsWith(FilePrefix)){
        url.replace(FilePrefix, "");
    }
    QFileInfo f(url);
    if (decodeUrl(f.path()) == desktopLocation){
        if (f.fileName().startsWith(RichDirPrefix)){
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

bool isAllApp(QStringList urls){
    if (urls.length() > 0){
        bool ret = true;
        foreach (QString url, urls) {
            ret = ret && isApp(url);
            if (!ret){
                return false;
            }
        }
        return ret;
    }else{
        return false;
    }
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
    url.replace(FilePrefix, "");
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

bool isInDesktop(QString url){
    if (decodeUrl(QFileInfo(url).path()) == desktopLocation){
        return true;
    }
    return false;
}


bool isDesktopAppFile(QString url){
    if (QFileInfo(url).fileName().endsWith(AppSuffix)){
        return true;
    }
    return false;
}


QString formatURl(QString url){
    QString key = decodeUrl(url);
    if (!key.startsWith(FilePrefix)){
        key = FilePrefix + key;
    }
    return key;
}
