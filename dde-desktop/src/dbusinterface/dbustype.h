#ifndef DBUSTYPE_H
#define DBUSTYPE_H

#include <QtCore>
#include <QtDBus>

struct EntryInfoObj {
    QString DisplayName;
    QString BaseName;
    QString URI;
    QString MIME;
    QString Icon;
    qlonglong Size;
    ushort FileType;
    bool IsHidden;
    bool IsReadOnly;
    bool IsSymlink;
    bool CanDelete;
    bool CanExecute;
    bool CanRead;
    bool CanRename;
    bool CanTrash;
    bool CanWrite;

    QDBusArgument &operator<<(QDBusArgument &argument, const EntryInfoObj &obj)
    {
        argument.beginStructure();
        argument << obj.DisplayName << obj.BaseName;
        argument << obj.URI << obj.MIME;
        argument << obj.Icon << obj.Size;
        argument << obj.FileType << obj.IsHidden;
        argument << obj.IsReadOnly << obj.IsSymlink;
        argument << obj.CanDelete << obj.CanExecute;
        argument << obj.CanRead << obj.CanRename;
        argument << obj.CanTrash << obj.CanWrite;
        argument.endStructure();
        return argument;
    }


    const QDBusArgument &operator>>(const QDBusArgument &argument, EntryInfoObj &obj)
    {
        argument.beginStructure();
        argument >> obj.DisplayName >> obj.BaseName;
        argument >> obj.URI >> obj.MIME;
        argument >> obj.Icon >> obj.Size;
        argument >> obj.FileType >> obj.IsHidden;
        argument >> obj.IsReadOnly >> obj.IsSymlink;
        argument >> obj.CanDelete >> obj.CanExecute;
        argument >> obj.CanRead >> obj.CanRename;
        argument >> obj.CanTrash >> obj.CanWrite;
        argument.endStructure();
        return argument;
    }
};

typedef QList<EntryInfoObj> EntryInfoObjList;
Q_DECLARE_METATYPE(EntryInfoObjList)


#endif // DBUSTYPE_H

