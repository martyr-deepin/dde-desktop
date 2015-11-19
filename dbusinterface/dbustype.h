#ifndef DBUSTYPE_H
#define DBUSTYPE_H

#include <QtCore>
#include <QtDBus>
#include "iosfwd"

class  DesktopItemInfo
{

public:
    QString DisplayName;
    QString BaseName;
    QString URI;
    QString MIME;
    QString Icon;
    QString IconName;
    QString thumbnail;
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


class ConflictInfo{


public:

    enum {
        ResponseCancel  = 1 << 0,
        ResponseSkip = 1 << 1,
        ResponseRetry = 1 << 2,
        ResponseDelete = 1 << 3,
        ResponseOverwrite = 1 << 4,
        ResponseAutoRename = 1 << 5, // auto rename the conflict file/directory
        ResponseYes = 1 << 6
    };

    int32_t code;
    bool applyToAll;
    QString userData;

    ConflictInfo();
    ~ConflictInfo();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const ConflictInfo &obj);

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ConflictInfo &obj);

    static void registerMetaType();
};


typedef QMap<QString, DesktopItemInfo> DesktopItemInfoMap;
Q_DECLARE_METATYPE(DesktopItemInfo)
Q_DECLARE_METATYPE(DesktopItemInfoMap)

Q_DECLARE_METATYPE(ConflictInfo)

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)

#endif // DBUSTYPE_H

