/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GROWINGTEXTEDIT_H
#define GROWINGTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class GrowingTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit GrowingTextEdit(QWidget *parent = 0);
    ~GrowingTextEdit();

    QString getUrl();

signals:
    void escaped(QString url);
    void editedFinished();

public slots:
    void sizeChange();
    void setUrl(QString url);

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    int m_minHeight = 0;
    int m_maxHeight = 6000;
    QString m_url;
};

#endif // GROWINGTEXTEDIT_H
