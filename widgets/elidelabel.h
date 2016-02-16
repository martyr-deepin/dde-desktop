/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ELIDELABEL_H
#define ELIDELABEL_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class ElidedLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ElidedLabel(QWidget *parent=0, Qt::WindowFlags f=0);
    explicit ElidedLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);

    bool isSimpleWrapMode();
    void setFullText(const QString &text);
    void setTextLimitShrink(const QString &text, int width);
    void setTextLimitExpand(const QString &text);
    QString fullText() const;
    QString getFullWrapText();
    QString getSimpWrapText();
    QStringList getTexts();

public:
    void elideText();
    void showSimpleWrapText();
    void showFullWrapText();
    void setSimpleMode(bool flag);
protected:
    void resizeEvent(QResizeEvent * event);

private:
    QString m_fullText;
    QString m_fullWrapText;
    QString m_simpleWrapText;
    QStringList m_texts;
    bool m_simpleWrapMode = true;
};

#endif // ELIDELABEL_H
