#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class GridItem : public QObject
{
    Q_OBJECT
public:
    explicit GridItem(int row, int column, QRect rect, QObject *parent = 0);
    ~GridItem();

    int getColumn();
    void setColumn(const int column);
    int getRow();
    void setRow(const int row);
    QRect getRect();
    void setRect(const QRect rect);

signals:

public slots:

private:
    int _column;
    int _row;
    QRect _rect;
};

#endif // GRIDITEM_H
