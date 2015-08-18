#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QtCore>

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
    QPoint getPos();
    QString key();

    bool hasDesktopItem();

signals:

public slots:
    void setDesktopItem(bool flag);

private:
    int m_column;
    int m_row;
    QRect m_rect;
    QPoint m_pos;
    bool m_isDesktopItemIn;
};

#endif // GRIDITEM_H
