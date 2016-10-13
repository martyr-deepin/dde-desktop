/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "filewidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

FileWidget::FileWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    QLabel *icon = new QLabel();
    icon->setPixmap(QPixmap("/usr/share/icons/hicolor/512x512/apps/owncloud.png").scaled(64, 64));
    icon->setFixedSize(64, 64);

    QTextEdit *edit = new QTextEdit;
    edit->setFixedSize(90, 32);

    mainLayout->addWidget(icon, 0, Qt::AlignCenter);
    mainLayout->addWidget(edit, 0, Qt::AlignCenter);
}
