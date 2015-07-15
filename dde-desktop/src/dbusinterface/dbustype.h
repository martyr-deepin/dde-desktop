#ifndef DBUSTYPE_H
#define DBUSTYPE_H

#include <QtCore>
#include <QtDBus>

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


typedef QMap<QString, DesktopItemInfo> DesktopItemInfoMap;
Q_DECLARE_METATYPE(DesktopItemInfo)
Q_DECLARE_METATYPE(DesktopItemInfoMap)

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)

#endif // DBUSTYPE_H

