#ifndef APPTABLEWIDGET_H
#define APPTABLEWIDGET_H

#include "dbusinterface/dbustype.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopItem;

class AppTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit AppTableWidget(QWidget *parent = 0);
    AppTableWidget(int rows, int columns, QWidget *parent = 0);
    ~AppTableWidget();

    void init();
    void startDrag(const DesktopItemInfo& info);
    QPixmap getDragPixmap();
    QList<int> getColumnRowByCount(int count);
    void setItemUnChecked();

signals:

public slots:
    void addItems(QList<DesktopItemInfo> itemInfos);
    void handleCellClicked(int row, int column);
    void handleCellPressed(int row, int column);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);

private:
    DesktopItem* m_dragItem;
};

#endif // APPTABLEWIDGET_H
