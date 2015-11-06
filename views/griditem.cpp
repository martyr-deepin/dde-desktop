#include "views/griditem.h"

GridItem::GridItem(int row, int column, QRect rect, QObject *parent) : QObject(parent)
{
    m_row = row;
    m_column = column;
    m_rect = rect;
    m_pos = QPoint(rect.x(), rect.y());
    m_isDesktopItemIn = false;
}

int GridItem::getRow(){
    return m_row;
}

void GridItem::setRow(const int row){
    m_row = row;
}

int GridItem::getColumn(){
    return m_column;
}

void GridItem::setColumn(const int column){
    m_column = column;
}

QRect GridItem::getRect(){
    return m_rect;
}

void GridItem::setRect(const QRect rect){
    m_rect = rect;
    m_pos = QPoint(rect.x(), rect.y());
}

QPoint GridItem::getPos(){
    return m_pos;
}

bool GridItem::hasDesktopItem(){
    if (m_isMultiDesktopItemsIn){
        return false;
    }else{
        return m_isDesktopItemIn;
    }
}

void GridItem::setDesktopItem(bool flag){
    if (m_isMultiDesktopItemsIn){
        m_isDesktopItemIn = false;
    }else{
        m_isDesktopItemIn = flag;
    }
}

bool GridItem::isMultiDesktopItemsIn(){
    return m_isMultiDesktopItemsIn;
}

void GridItem::setMultiDesktopItemsIn(bool flag){
    m_isMultiDesktopItemsIn = flag;
}

QString GridItem::key(){
    return QString("%1-%2").arg(QString::number(m_pos.x()), QString::number(m_pos.y()));
}

GridItem::~GridItem()
{

}

