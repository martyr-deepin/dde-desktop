#include "views/griditem.h"

GridItem::GridItem(int row, int column, QRect rect, QObject *parent) : QObject(parent)
{
    m_row = row;
    m_column = column;
    m_rect = rect;
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
}

GridItem::~GridItem()
{

}

