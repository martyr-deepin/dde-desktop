/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONFIRMDELETEDIALOG_H
#define CONFIRMDELETEDIALOG_H

#include "dbasedialog.h"

class ConfirmDeleteDialog : public DBaseDialog
{
    Q_OBJECT
public:
    explicit ConfirmDeleteDialog(QWidget *parent = 0);
    ~ConfirmDeleteDialog();

signals:

public slots:
    void handleKeyEnter();
};

#endif // CONFIRMDELETEDIALOG_H
