/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DTASKDIALOG_H
#define DTASKDIALOG_H

#include "dmovabledialog.h"
#include "widgets/dcircleprogress.h"
#include <QLabel>
#include <QListWidget>
#include <QResizeEvent>
#include <QListWidgetItem>
#include <QButtonGroup>
#include <QCheckBox>

class MoveCopyTaskWidget : public QFrame
{
    Q_OBJECT
public:
    explicit MoveCopyTaskWidget(const QMap<QString, QString>& jobDetail, QWidget *parent = 0);
    void initUI();
    void initConnect();
    QString getTargetObj();
    QString getDestinationObj();
    int getProgress();
    float getSpeed();
    int getTimeLeft();
    QString getMessage();
    QString getTipMessage();

    void initButtonFrame();

signals:
    void closed(const QMap<QString, QString>& jobDetail);
    void conflictResponseConfirmed(const QMap<QString, QString>& jobDetail, const QMap<QString, QVariant>& response);
    void heightChanged();
    void conflictShowed(const QMap<QString, QString>& jobDetail);
    void conflictHided(const QMap<QString, QString>& jobDetail);

public slots:
    void setTargetObj(QString targetObj);
    void setDestinationObj(QString destinationObj);
    void setProgress(int value);
    void setProgress(QString value);
    void setSpeed(float speed);
    void setTimeLeft(int time);
    void setMessage(QString message);
    void setTipMessage(QString tipMessage);
    void handleClose();
    void handleResponse();
    void updateMessage(const QMap<QString, QString>& data);
    void updateTipMessage();

    void showConflict();
    void hideConflict();
protected:

private:
    int m_progress = 0;
    float m_speed = 0.0;
    int m_timeLeft;
    QString m_targetObj;
    QString m_destinationObj;
    QString m_message;
    QString m_tipMessage;
    DCircleProgress* m_cirleWidget=NULL;
    QLabel* m_messageLabel;
    QLabel* m_tipMessageLabel;
    QPushButton* m_closeButton;
    QMap<QString, QString> m_jobDetail;
    QMap<QString, QVariant> m_response;
    QButtonGroup* m_buttonGroup;
    QFrame* m_buttonFrame=NULL;
    QCheckBox* m_checkBox=NULL;
    QPushButton* m_enterButton=NULL;
};


class DTaskDialog : public DMovabelDialog
{
    Q_OBJECT
public:
    explicit DTaskDialog(QWidget *parent = 0);
    void initUI();
    void initConnect();
signals:
    void abortCopyTask(const QMap<QString, QString>& jobDetail);
    void abortMoveTask(const QMap<QString, QString>& jobDetail);
    void abortDeleteTask(const QMap<QString, QString>& jobDetail);
    void conflictRepsonseConfirmed(const QMap<QString, QString>& jobDetail, const QMap<QString, QVariant>& response);
    void conflictShowed(const QMap<QString, QString>& jobDetail);
    void conflictHided(const QMap<QString, QString>& jobDetail);
public slots:
    void setTitle(QString title);
    void setTitle(int taskCount);
    void addCopyMoveTask(const QMap<QString, QString>& jobDetail);
    void addConflictTask(const QMap<QString, QString>& jobDetail);
    void handleTaskClose(const QMap<QString, QString>& jobDetail);
    void removeTaskWidget(const QMap<QString, QString>& jobDetail);
    void removeTaskWidget(QString jobPath);
    void handleUpdateTaskWidget(const QMap<QString, QString>& jobDetail,
                                const QMap<QString, QString>& data);
    void adjustSize();

    void showConflictDiloagByJob(const QMap<QString, QString>& jobDetail);
    void handleConflictResponse(const QMap<QString, QString>& jobDetail, const QMap<QString, QVariant>& response);

    void delayShow();
    void quickHide();

protected:
    void closeEvent(QCloseEvent* event);


private:
    int m_defaultWidth = 440;
    int m_defaultHeight = 120;
    QLabel* m_titleLabel=NULL;
    QPushButton* m_titleBarMinimizeButton;
    QPushButton* m_titleBarCloseButton;
    QListWidget* m_taskListWidget=NULL;
    QMap<QString, QListWidgetItem*> m_jobPathItems;

    QTimer * m_delayShowTimer = NULL;
};

#endif // DTASKDIALOG_H
