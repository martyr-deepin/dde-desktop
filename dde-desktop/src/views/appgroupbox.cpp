#include "appgroupbox.h"
#include "apptablewidget.h"

AppGroupBox::AppGroupBox(QWidget *parent) : ArrowRectangle(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setMargin(10);
}


void AppGroupBox::showDetailByDesktopItem(DesktopItemPointer &pItem){
    setProperty("url", pItem->getUrl());

    AppTableWidget* tableWidget = new AppTableWidget();
    tableWidget->addItems(pItem->getAppGroupItems().values());

    int width = gridManager->getItemWidth();
    int height = gridManager->getItemHeight();
    int desktopWidth = qApp->desktop()->availableGeometry().width();
    int desktopHeight = qApp->desktop()->availableGeometry().height();

    int topHeight = pItem->pos().y() - getMargin() * 2 - getArrowHeight() ;
    int bottomHeight = desktopHeight - pItem->pos().y() - height - getMargin() * 2 - getArrowHeight();
    int groupHeight = getMargin() * 2 + getArrowHeight()  + tableWidget->height();

    if (bottomHeight > groupHeight){
        setArrorDirection(ArrowRectangle::ArrowTop);
        setContent(tableWidget);
        showAtTop(pItem->pos().x() + width / 2, pItem->pos().y() + height);
    }else{
        if (groupHeight < topHeight){
            setArrorDirection(ArrowRectangle::ArrowBottom);
            setContent(tableWidget);
            showAtBottom(pItem->pos().x() + width / 2, pItem->pos().y());
        }else{
            if (topHeight / height * height > bottomHeight / height * height){
                setArrorDirection(ArrowRectangle::ArrowBottom);
                tableWidget->setFixedHeight(topHeight / height * height);

                if (tableWidget->width() < desktopWidth){
                    tableWidget->setFixedWidth(tableWidget->width() + 5);
                }else{
                    tableWidget->setFixedWidth(desktopWidth / width * width + 5);
                }

                setContent(tableWidget);
                showAtBottom(pItem->pos().x() + width / 2, pItem->pos().y());
            }else{
                setArrorDirection(ArrowRectangle::ArrowTop);
                tableWidget->setFixedHeight(bottomHeight / height * height);
                if (tableWidget->width() < desktopWidth){
                    tableWidget->setFixedWidth(tableWidget->width() + 5);
                }else{
                    tableWidget->setFixedWidth(desktopWidth / width * width + 5);
                }
                setContent(tableWidget);
                showAtTop(pItem->pos().x() + width / 2, pItem->pos().y() + height);
            }
        }
    }
}

AppGroupBox::~AppGroupBox()
{
    qDebug() << "appGroup delete";
}

