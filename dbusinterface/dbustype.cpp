
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
    argument << obj.Icon <<  obj.thumbnail << obj.Size;
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
    argument >> obj.Icon >> obj.thumbnail >>obj.Size;
    argument >> obj.FileType >> obj.IsBackup >> obj.IsHidden;
    argument >> obj.IsReadOnly >> obj.IsSymlink;
    argument >> obj.CanDelete >> obj.CanExecute;
    argument >> obj.CanRead >> obj.CanRename;
    argument >> obj.CanTrash >> obj.CanWrite;
    argument.endStructure();
    return argument;
}
