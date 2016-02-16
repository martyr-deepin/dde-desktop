/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BACKGROUNDLABEL_H
#define BACKGROUNDLABEL_H

#include <QLabel>
class QResizeEvent;
class SystemBackground;
class BackgroundLabel : public QLabel
{
    Q_OBJECT

public:
    BackgroundLabel(bool isBlur=false, QWidget *parent = 0);
    ~BackgroundLabel();

protected:
    void resizeEvent(QResizeEvent* event);

protected:
    void setBlur(bool isBlur);

private:
    SystemBackground* m_systemBackground;
};

#endif // BACKGROUNDLABEL_H
