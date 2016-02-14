/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "filemonitorwoker.h"
#include "views/global.h"
#include "widgets/util.h"

FileMonitorWoker::FileMonitorWoker(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(monitorFolderChanged(QString)), this, SLOT(monitor(QString)));
    connect(&m_fileWatcher,SIGNAL(directoryChanged(QString)),this,SLOT(handleDirectoryChanged(QString)));
    connect(&m_fileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(handleFileChanged(QString)));
}

FileMonitorWoker::~FileMonitorWoker()
{

}


void FileMonitorWoker::monitor(const QString &path)
{
    m_init_event = false;
    m_currentFolder = path;
    addWatchFolder(path); //only desktop
    addWatchFile(path); // under desktop files
    addWatchGroup(path); // only under .deepin_rich_dir_* files

}

void FileMonitorWoker::addWatchFolder(const QString &path)
{
    m_desktopFolder = path;
    m_fileWatcher.addPath(path);
}

void FileMonitorWoker::delWatchFile(const QString &path)
{
    //if the file is not exists, fileWatcher can remove path automatic, when this function return false
    m_fileWatcher.removePath(path);
    qDebug()<<"remove file:" << path;
    handleInotifyEvent(IN_DELETE, path);
}

void FileMonitorWoker::delWatchFolder(const QString &path)
{
    delWatchFile(path);
}


void FileMonitorWoker::addWatchGroup(const QString &path){
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (isAppGroup(fileInfo.absoluteFilePath())){
            addSingleWatchFile( list.at(i).absoluteFilePath() );
            addWatchFile( list.at(i).absoluteFilePath() );
        }
    }

}

void FileMonitorWoker::addWatchDir(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);

        if (isAppGroup(fileInfo.absoluteFilePath())){
            addSingleWatchFile( list.at(i).absoluteFilePath() );
            addWatchFile( list.at(i).absoluteFilePath() );
        }else{
            addSingleWatchFile( list.at(i).absoluteFilePath() );
        }
    }

}




void FileMonitorWoker::addWatchFile(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i) {
        addSingleWatchFile( list.at(i).absoluteFilePath() );
    }

}
void FileMonitorWoker::addSingleWatchFile(const QString &path)
{
    if(m_fileWatcher.addPath(path))
    {
        handleInotifyEvent(IN_CREATE, path);
    }

}



void FileMonitorWoker::handleFileChanged(const QString & path)
{
    m_init_event = true;
    if(QFile(path).exists())
    {
        addSingleWatchFile(path);
    }else{
         delWatchFile(path);
    }

}

void FileMonitorWoker::handleDirectoryChanged(const QString & path)
{
    m_init_event = true;

    if(QFile(path).exists())
    {
        checkingForChanges(path);
    }else{
        delWatchFile(path);
    }

}

void FileMonitorWoker::checkingFiles(const QString & path)
{

    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::Hidden |QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();

    while(!list.isEmpty())
    {
        QString filepath = list.takeLast().absoluteFilePath();

        if(QFile(filepath).exists())
        {
            addSingleWatchFile(filepath);
            continue;
        }else{
            delWatchFile(filepath);
        }
    }
}

void FileMonitorWoker::checkingDirs(const QString & path)
{

    QDir dir(m_desktopFolder);
    dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    int dirSize = list.size() + 1;

    while(!list.isEmpty())
    {
        QString filepath = list.takeLast().absoluteFilePath();
        if(QFile(filepath).exists())
        {
            if (isAppGroup(filepath)){
                addSingleWatchFile( filepath );
                addWatchFile( filepath );
                continue;
            }else{
                addSingleWatchFile( filepath );
            }

        }else{
           // qDebug() <<"!! move dir event";
            delWatchFolder(filepath);
        }
    }

}

void FileMonitorWoker::checkingForChanges(const QString & path)
{

    if(path == m_desktopFolder || isAppGroup(path) )
    {
        checkingFiles(path);
    }
    checkingDirs(path);

}

void FileMonitorWoker::handleInotifyEvent(int event, QString path)
{
    if(!m_init_event)
        return;
    switch(event)
    {
    case IN_CREATE:
        emit fileCreated(event, path);
        break;
    case IN_MOVED_FROM:
        emit fileMovedFrom(event, path);
        break;
    case IN_MOVED_TO:
        emit fileMovedTo(event, path);
        break;
    case IN_DELETE:
        emit fileDeleted(event, path);
        break;
    case IN_ATTRIB:
        emit metaDataChanged(event, path);
        break;
    }
}


