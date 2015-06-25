#include "views/griditem.h"

GridItem::GridItem(int row, int column, QRect rect, QObject *parent) : QObject(parent)
{
    _row = row;
    _column = column;
    _rect = rect;
}

int GridItem::getRow(){
    return _row;
}

void GridItem::setRow(const int row){
    _row = row;
}

int GridItem::getColumn(){
    return _column;
}

void GridItem::setColumn(const int column){
    _column = column;
}

QRect GridItem::getRect(){
    return _rect;
}

void GridItem::setRect(const QRect rect){
    _rect = rect;
}

GridItem::~GridItem()
{

}

