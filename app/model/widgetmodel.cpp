/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "widgetmodel.h"

WidgetModel::WidgetModel(QObject *parent) : QAbstractItemModel(parent)
{

}

QModelIndex WidgetModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex WidgetModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int WidgetModel::rowCount(const QModelIndex &parent) const
{
    return -1;
}

int WidgetModel::columnCount(const QModelIndex &parent) const
{
    return -1;
}

QVariant WidgetModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}
