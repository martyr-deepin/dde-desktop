#ifndef BASETABLEWIDGET_H
#define BASETABLEWIDGET_H

#include <QTableWidget>

class BaseTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit BaseTableWidget(QWidget *parent = 0);
    ~BaseTableWidget();

signals:

public slots:
};

#endif // BASETABLEWIDGET_H
