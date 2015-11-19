
#include "dbustype.h"

DesktopItemInfo::DesktopItemInfo(){

}

DesktopItemInfo::~DesktopItemInfo(){

}

void DesktopItemInfo::registerMetaType()
{
    qRegisterMetaType<DesktopItemInfo>("DesktoItemInfo");
    qDBusRegisterMetaType<DesktopItemInfo>();
}


QDBusArgument &operator<<(QDBusArgument &argument, const DesktopItemInfo &obj)
{
    argument.beginStructure();
    argument << obj.DisplayName << obj.BaseName;
    argument << obj.URI << obj.MIME;
    argument << obj.Icon <<  obj.IconName << obj.thumbnail << obj.Size;
    argument << obj.FileType << obj.IsBackup << obj.IsHidden;
    argument << obj.IsReadOnly << obj.IsSymlink;
    argument << obj.CanDelete << obj.CanExecute;
    argument << obj.CanRead << obj.CanRename;
    argument << obj.CanTrash << obj.CanWrite;
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument, DesktopItemInfo &obj)
{
    argument.beginStructure();
    argument >> obj.DisplayName >> obj.BaseName;
    argument >> obj.URI >> obj.MIME;
    argument >> obj.Icon >> obj.IconName >> obj.thumbnail >>obj.Size;
    argument >> obj.FileType >> obj.IsBackup >> obj.IsHidden;
    argument >> obj.IsReadOnly >> obj.IsSymlink;
    argument >> obj.CanDelete >> obj.CanExecute;
    argument >> obj.CanRead >> obj.CanRename;
    argument >> obj.CanTrash >> obj.CanWrite;
    argument.endStructure();
    return argument;
}


ConflictInfo::ConflictInfo(){

}

ConflictInfo::~ConflictInfo(){

}

void ConflictInfo::registerMetaType()
{
    qRegisterMetaType<ConflictInfo>("ConflictInfo");
    qDBusRegisterMetaType<ConflictInfo>();
}


QDBusArgument &operator<<(QDBusArgument &argument, const ConflictInfo &obj)
{
    argument.beginStructure();
    argument << obj.code << obj.applyToAll;
    argument << obj.userData;
    argument.endStructure();
    return argument;
}


const QDBusArgument &operator>>(const QDBusArgument &argument, ConflictInfo &obj)
{
    argument.beginStructure();
    argument >> obj.code >> obj.applyToAll;
    argument >> obj.userData;
    argument.endStructure();
    return argument;
}
