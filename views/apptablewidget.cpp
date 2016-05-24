/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "apptablewidget.h"
#include "desktopitem.h"
#include "global.h"
#include "widgets/util.h"
#include <typeinfo>
#include <QTableWidgetItem>

AppTableWidget::AppTableWidget(QWidget *parent) : QTableWidget(parent)
{
    init();
}

AppTableWidget::AppTableWidget(int rows, int columns, QWidget *parent):
    QTableWidget(rows, columns, parent)
{
    init();
}

void AppTableWidget::init(){
    setAttribute(Qt::WA_TranslucentBackground);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setEditTriggers(NoEditTriggers);
    setDragDropMode(DragOnly);
    setAcceptDrops(true);
    setDragEnabled(true);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setShowGrid(false);

    setStyleSheet(getQssFromFile(":/qss/skin/qss/Tablewidget.qss"));

    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(handleCellClicked(int,int)));
    connect(this, SIGNAL(cellPressed(int,int)), this, SLOT(handleCellPressed(int,int)));
    qDebug() << "App Group init finished";
}

void AppTableWidget::addItems(QList<DesktopItemInfo> itemInfos){

    if (itemInfos.count() > 1){
        int width = gridManager->getItemWidth();
        int height = gridManager->getItemHeight();

        int count = itemInfos.count();
        QList<int> ret = getColumnRowByCount(count);
        int columns = ret.at(0);
        int rows = ret.at(1);

        setColumnCount(columns);
        setRowCount(rows);

        setFixedSize(width * columns + 5, rows * height + 5);

        for (int i=0; i < count; i++){
            int _row = i / columns;
            int _column = i % columns;

            DesktopItemInfo fileInfo = itemInfos.at(i);
            QString displayName = fileInfo.DisplayName;
            QString uri = fileInfo.URI;
            QString url = decodeUrl(uri);
            QString icon = fileInfo.Icon;

            DesktopItem*  pDesktopItem = new DesktopItem(this);
            pDesktopItem->setFocusPolicy(Qt::NoFocus);
            pDesktopItem->setDesktopName(displayName);
            pDesktopItem->setUrl(url);
            pDesktopItem->setDesktopIcon(icon);
            pDesktopItem->setDesktopItemInfo(fileInfo);
            pDesktopItem->resize(width, height);
            pDesktopItem->setIsInAppGroup(true);
            pDesktopItem->disabledTextShadow();

            setCellWidget(_row, _column, pDesktopItem);
            setRowHeight(_row, height);
            setColumnWidth(_column, width);
        }
    }

}

void AppTableWidget::handleCellClicked(int row, int column){
    qDebug() << row << column << "handleCellClicked";
    DesktopItem* pItem = dynamic_cast<DesktopItem*>(cellWidget(row, column));
    if (pItem == NULL){
        qDebug() << "handleCellClicked no desktop item";
    }else{
        emit signalManager->openFile(pItem->getDesktopItemInfo());
    }
}


void AppTableWidget::handleCellPressed(int row, int column){
    qDebug() << row << column << "handleCellPressed";
    emit signalManager->appGroupItemRightClicked(true);
    m_dragItem = reinterpret_cast<DesktopItem*>(cellWidget(row, column));
    qDebug() << "handleCellPressed drag desktop item" << m_dragItem;
}


QList<int> AppTableWidget::getColumnRowByCount(int count){
//    int width = gridManager->getItemWidth();
//    int height = gridManager->getItemHeight();
//    int desktopWidth = qApp->desktop()->availableGeometry().width();
//    int desktopHeight = qApp->desktop()->availableGeometry().height();
    int i = qFloor(qSqrt((double)count));
    int j = 0;
    if (count % i > 0){
        j = count / i + 1;
    }else{
        j = count / i;
    }
    QList<int> ret;
    int column = qMax(i, j);
    int row = qMin(i, j);

    if (column > 6){
        column = 6;
        if (count % column == 0){
            row = count / column;
        }else{
            row = count / column + 1;
        }
    }
    ret.append(column);
    ret.append(row);

    return ret;
}

void AppTableWidget::startDrag(const DesktopItemInfo &info){
//    QByteArray itemData;
//    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QList<QUrl> urls;
    qDebug() << info.URI;
    urls.append(QUrl(info.URI));
    QMimeData* mimeData = new QMimeData;
    mimeData->setData("source", "AppTableWidget");
    mimeData->setUrls(urls);

    QPixmap dragPixmap = getDragPixmap();
    QDrag* pDrag = new QDrag(this);
    pDrag->setMimeData(mimeData);
    pDrag->setPixmap(dragPixmap);
    pDrag->setHotSpot(-mapToGlobal(m_dragItem->pos()) + QCursor::pos());
    Qt::DropAction action = pDrag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
     if (action == Qt::MoveAction){
        qDebug() << "app group item drag move======";
     }else{
        qDebug() << "app group item drag copy======";
     }
}

QPixmap AppTableWidget::getDragPixmap(){
    QFrame* F = new QFrame(this);
    F->setAttribute(Qt::WA_DeleteOnClose);
    F->setObjectName("DesktopFrame");

    DesktopItem* item = new DesktopItem(m_dragItem->getDesktopItemInfo().Icon,
                                       m_dragItem->getDesktopName(), F);
    item->resize(m_dragItem->size());
    item->setObjectName("DragChecked");
    item->setDesktopIcon(m_dragItem->getDesktopItemInfo().Icon);

    F->setStyleSheet(qApp->styleSheet());
    F->resize(100, 100);
    QPixmap ret = F->grab();
    F->close();
    return ret;
}


void AppTableWidget::mouseMoveEvent(QMouseEvent *event){
    Q_UNUSED(event);
    if (m_dragItem){
        qDebug() << "mouseMoveEvent getDesktopItemInfo" << m_dragItem->getDesktopItemInfo().BaseName;
        startDrag(m_dragItem->getDesktopItemInfo());
    }
}

void AppTableWidget::dragEnterEvent(QDragEnterEvent *event){
    qDebug() << "app group enter" << event->pos();
//    m_dragLeave = false;
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }else{
            qDebug() << event->mimeData();
            event->ignore();
        }
    }else{
        event->ignore();
    }
}


void AppTableWidget::dragMoveEvent(QDragMoveEvent *event){
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){
             event->setDropAction(Qt::MoveAction);
             event->accept();
        }else{
            event->ignore();;//eptProposedAction();
        }
    }else{
        event->ignore();
    }
}

void AppTableWidget::dragLeaveEvent(QDragLeaveEvent *event){
    //    m_dragLeave = true;
    qDebug() << "app group leave" << event;
    event->ignore();
}

void AppTableWidget::mousePressEvent(QMouseEvent *event)
{
    int i = event->pos().x() / gridManager->getItemWidth();
    int j = event->pos().y() / gridManager->getItemHeight();
    if (!cellWidget(j, i)){
        return;
    }
    QTableWidget::mousePressEvent(event);
}

void AppTableWidget::setItemUnChecked(){
    for(int i = 0; i< rowCount(); i++){
        for (int j=0; j < columnCount(); j++){
            if (item(i, j)){
                item(i, j)->setFlags(Qt::NoItemFlags);
            }else{
                QTableWidgetItem* item = new QTableWidgetItem();
                item->setFlags(Qt::NoItemFlags);
                setItem(i, j, item);
            }
        }
    }
}

AppTableWidget::~AppTableWidget()
{

}

