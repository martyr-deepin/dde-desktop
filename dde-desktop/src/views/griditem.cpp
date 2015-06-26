#include "views/griditem.h"

GridItem::GridItem(int row, int column, QRect rect, QObject *parent) : QObject(parent)
{
    m_row = row;
    m_column = column;
    m_rect = rect;
    m_pos = QPoint(rect.x(), rect.y());
    m_desktopItem = DesktopItemPointer();
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
    return m_isDesktopItemIn;
}

DesktopItemPointer GridItem::getDesktopItem(){
    return m_desktopItem;
}

void GridItem::setDesktopItem(DesktopItemPointer desktopItem){
//    clearDesktopItem();
    m_desktopItem = desktopItem;
    m_isDesktopItemIn = true;
}

void GridItem::clearDesktopItem(){
    m_desktopItem = DesktopItemPointer();
    m_isDesktopItemIn = false;
}

GridItem::~GridItem()
{

}

