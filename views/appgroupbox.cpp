#include "appgroupbox.h"
#include "apptablewidget.h"
#include "global.h"
#include "app/xcb_misc.h"

AppGroupBox::AppGroupBox(QWidget *parent) :
    DArrowRectangle(DArrowRectangle::ArrowTop, parent)
{
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose);
}


void AppGroupBox::showDetailByDesktopItem(DesktopItemPointer &pItem){
    activateWindow();
    if (!pItem.isNull()){
        if (pItem->getAppGroupItems().count() < 2){
            return;
        }
        setProperty("url", pItem->getUrl());

        AppTableWidget* tableWidget = new AppTableWidget();
        tableWidget->addItems(pItem->getAppGroupItems().values());

        int width = gridManager->getItemWidth();
        int height = gridManager->getItemHeight();
        int desktopWidth = qApp->desktop()->availableGeometry().width();
        int desktopHeight = qApp->desktop()->availableGeometry().height();

        int topHeight = pItem->pos().y() - margin() * 2 - arrowHeight() ;
        int bottomHeight = desktopHeight - pItem->pos().y() - height - margin() * 2 - arrowHeight();
        int groupHeight = margin() * 2 + arrowHeight()  + tableWidget->height();

        if (bottomHeight > groupHeight){
            setContent(tableWidget);
            QPoint point = mapToGlobal(QPoint(pItem->pos().x() + width / 2,
                                              pItem->pos().y() + height - margin() - shadowBlurRadius()));
            setArrowDirection(ArrowTop);
            show(point.x() % desktopWidth, point.y());
        }else{
            if (groupHeight < topHeight){
                setContent(tableWidget);
                QPoint point = mapToGlobal(QPoint(pItem->pos().x() + width / 2,
                                                  pItem->pos().y() + shadowBlurRadius()));
                setArrowDirection(ArrowBottom);
                show(point.x() % desktopWidth, point.y());
            }else{
                if (topHeight / height * height > bottomHeight / height * height){
                    tableWidget->setFixedHeight(topHeight / height * height);

                    if (tableWidget->width() < desktopWidth){
                        tableWidget->setFixedWidth(tableWidget->width() + 5);
                    }else{
                        tableWidget->setFixedWidth(desktopWidth / width * width + 5);
                    }

                    setContent(tableWidget);
                    QPoint point = mapToGlobal(QPoint(pItem->pos().x() + width / 2,
                                                      pItem->pos().y() + shadowBlurRadius()));
                    setArrowDirection(ArrowBottom);
                    show(point.x() % desktopWidth, point.y());
                }else{
                    tableWidget->setFixedHeight(bottomHeight / height * height);
                    if (tableWidget->width() < desktopWidth){
                        tableWidget->setFixedWidth(tableWidget->width() + 5);
                    }else{
                        tableWidget->setFixedWidth(desktopWidth / width * width + 5);
                    }
                    setContent(tableWidget);
                    QPoint point = mapToGlobal(QPoint(pItem->pos().x() + width / 2,
                                                      pItem->pos().y() + height - shadowBlurRadius()));
                    setArrowDirection(ArrowTop);
                    show(point.x() % desktopWidth, point.y());
                }
            }
        }
    }
}

AppGroupBox::~AppGroupBox()
{

}

