
#include "dbustype.h"

EntryInfoObj::EntryInfoObj(){

}

EntryInfoObj::~EntryInfoObj(){

}


void EntryInfoObj::registerMetaType()
{
    qRegisterMetaType<EntryInfoObj>("EntryInfoObj");
    qDBusRegisterMetaType<EntryInfoObj>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const EntryInfoObj &obj)
{
    argument.beginStructure();
    argument << obj.DisplayName << obj.BaseName;
    argument << obj.URI << obj.MIME;
    argument << obj.Size;
    argument << obj.FileType << obj.IsBackup << obj.IsHidden;
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
    argument >> obj.Size;
    argument >> obj.FileType >> obj.IsBackup >> obj.IsHidden;
    argument >> obj.IsReadOnly >> obj.IsSymlink;
    argument >> obj.CanDelete >> obj.CanExecute;
    argument >> obj.CanRead >> obj.CanRename;
    argument >> obj.CanTrash >> obj.CanWrite;
    argument.endStructure();
    return argument;
}
