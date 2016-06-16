/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <gio/gio.h>

#include "util.h"

QString getQssFromFile(QString filename)
{
    QFile f(filename);
    QString qss = "";
    if (f.open(QFile::ReadOnly))
    {
        qss = QLatin1String(f.readAll());
        f.close();
    }
    return qss;
}

QString joinPath(const QString& path, const QString& fileName){
    QString separator(QDir::separator());
    return QString("%1%2%3").arg(path, separator, fileName);
}

bool IsExecutableScript(const DesktopItemInfo &info) {
    auto isFileExecutable = [](QString &path) {
        QFile file(path);
        return (file.permissions() & QFile::ReadUser) && (file.permissions() & QFile::ExeUser);
    };

    QString mimetype = info.MIME;
    QString path = QUrl(info.URI).toLocalFile();

    if (mimetype == "inode/symlink") {
        path = QFile(path).symLinkTarget();
        mimetype = GetFileMimetype(path);
    }

    if (mimetype.startsWith("text/") || (mimetype == "application/x-shellscript")) {
        return isFileExecutable(path);
    }

    return false;
}

QString GetFileMimetype(const QString &path)
{
    GFile *file;
    GFileInfo *info;
    QString result;

    file = g_file_new_for_path(path.toUtf8());
    info = g_file_query_info(file, "standard::content-type", G_FILE_QUERY_INFO_NONE, NULL, NULL);
    result = g_file_info_get_content_type(info);

    g_object_unref(file);

    return result;
}
