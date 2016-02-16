/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FILECONFLICTDIALOG_H
#define FILECONFLICTDIALOG_H

#include "dbasedialog.h"


class QHBoxLayout;
class DesktopItemInfo;

class FileConflictDialog : public DBaseDialog
{
    Q_OBJECT
public:
    explicit FileConflictDialog(QWidget *parent = 0);
    ~FileConflictDialog();

    QHBoxLayout* createFileLayout(QString icon, QString size, QString time, QString fileTip);

signals:

public slots:
    void addConflictFiles(QString newFile, QString oldFile);
};

#endif // FILECONFLICTDIALOG_H
