/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MENUCONTROLLER_H
#define MENUCONTROLLER_H


#include <QtCore>
#include "views/desktopfwd.h"

class MenumanagerInterface;
class MenuInterface;

#define MenuManager_service "com.deepin.menu"
#define MenuManager_path "/com/deepin/menu"

class MenuController : public QObject
{
    Q_OBJECT

public:

    explicit MenuController(QObject *parent = 0);
    ~MenuController();

    void initConnect();

    QJsonObject createMenuItem(int itemId, QString itemText);
    QJsonObject createSeperator();

    QString registerMenu();
    QString JsonToQString(QPoint pos, QString menucontent);
    QString createTextEditMenuContentByContent(QString fullText, QString selectText);
    void showTexitEditMenu(QString menuDBusObjectPath, QString menuContent);

    QString createMenuContent(QStringList createmenupath);
    void showMenu(const QString path, QString content);
signals:

public slots:
    void showMenuByDesktopItem(QString url, QPoint pos);
    void showMenuByDesktopItem(DesktopItemPointer pItem, QPoint pos);
    void showMenuByUrls(QStringList urls, QPoint pos);
    void showMenuByDesktopItem(const QList<DesktopItemPointer>& pCheckItems, const DesktopItemPointer& pItem, QPoint pos);
    void menuItemInvoked(QString itemId, bool flag);

    void showTexitEditMenuBySelectContent(QString url, QString fullText, QString selectText, QPoint pos);
    void textEditMenuInvoked(QString itemId, bool flag);

    void handleMenuUnregistered();

private:
    MenumanagerInterface* m_menuManagerInterface;
    MenuInterface* m_menuInterface;
    QString m_url;
};

#endif // MENUCONTROLLER_H
