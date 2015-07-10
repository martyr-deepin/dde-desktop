#ifndef DBUSTYPE_H
#define DBUSTYPE_H

#include <QtCore>
#include <QtDBus>

class  EntryInfoObj
{

public:
    QString DisplayName;
    QString BaseName;
    QString URI;
    QString MIME;
    qlonglong Size;
    ushort FileType;
    bool IsBackup;
    bool IsHidden;
    bool IsReadOnly;
    bool IsSymlink;
    bool CanDelete;
    bool CanExecute;
    bool CanRead;
    bool CanRename;
    bool CanTrash;
    bool CanWrite;

    EntryInfoObj();
    ~EntryInfoObj();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const EntryInfoObj &obj);

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, EntryInfoObj &obj);

    static void registerMetaType();
};


class  DesktopItemInfo
{

public:
    QString DisplayName;
    QString BaseName;
    QString URI;
    QString MIME;
    QString Icon;
    qlonglong Size;
    ushort FileType;
    bool IsBackup;
    bool IsHidden;
    bool IsReadOnly;
    bool IsSymlink;
    bool CanDelete;
    bool CanExecute;
    bool CanRead;
    bool CanRename;
    bool CanTrash;
    bool CanWrite;

    DesktopItemInfo();
    ~DesktopItemInfo();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const DesktopItemInfo &obj);

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, DesktopItemInfo &obj);

    static void registerMetaType();
};



typedef QList<EntryInfoObj> EntryInfoObjList;
Q_DECLARE_METATYPE(EntryInfoObj)
Q_DECLARE_METATYPE(EntryInfoObjList)

typedef QList<DesktopItemInfo> DesktopItemInfoList;
Q_DECLARE_METATYPE(DesktopItemInfo)
Q_DECLARE_METATYPE(DesktopItemInfoList)
#endif // DBUSTYPE_H

