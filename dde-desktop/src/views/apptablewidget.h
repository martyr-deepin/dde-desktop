#ifndef APPTABLEWIDGET_H
#define APPTABLEWIDGET_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "dbusinterface/dbustype.h"

class AppTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit AppTableWidget(QWidget *parent = 0);
    AppTableWidget(int rows, int columns, QWidget *parent = 0);
    ~AppTableWidget();

    void init();

    QList<int> getColumnRowByCount(int count);

signals:

public slots:
    void addItems(QList<DesktopItemInfo> itemInfos);
};

#endif // APPTABLEWIDGET_H
