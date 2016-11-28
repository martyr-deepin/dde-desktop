/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "canvasviewhelper.h"

#include <dfileinfo.h>
#include <dfilesystemmodel.h>
#include <diconitemdelegate.h>

#include "canvasgridview.h"

CanvasViewHelper::CanvasViewHelper(CanvasGridView *parent): DFileViewHelper(parent)
{

}

CanvasGridView *CanvasViewHelper::parent() const
{
    return qobject_cast<CanvasGridView *>(DFileViewHelper::parent());
}

const DAbstractFileInfoPointer CanvasViewHelper::fileInfo(const QModelIndex &index) const
{
    return parent()->model()->fileInfo(index);
}

DStyledItemDelegate *CanvasViewHelper::itemDelegate() const
{
    return parent()->itemDelegate();
}

DFileSystemModel *CanvasViewHelper::model() const
{
    return parent()->model();
}

const DUrlList CanvasViewHelper::selectedUrls() const
{
    qDebug() << parent()->selectedUrls();
    return parent()->selectedUrls();
}
