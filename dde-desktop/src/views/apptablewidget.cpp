#include "apptablewidget.h"
#include "global.h"
#include "widgets/util.h"

AppTableWidget::AppTableWidget(QWidget *parent) : QTableWidget(parent)
{
    init();
}

AppTableWidget::AppTableWidget(int rows, int columns, QWidget *parent):
    QTableWidget(rows, columns, parent)
{
    init();
}

void AppTableWidget::init(){
    setAttribute(Qt::WA_TranslucentBackground);
    horizontalHeader()->hide();
    verticalHeader()->hide();
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setShowGrid(false);

    setStyleSheet(getQssFromFile(":/skin/qss/Tablewidget.qss"));
}

void AppTableWidget::addItems(QList<DesktopItemInfo> itemInfos){

    if (itemInfos.count() > 1){
        int width = gridManager->getItemWidth();
        int height = gridManager->getItemHeight();

        int count = itemInfos.count();
        QList<int> ret = getColumnRowByCount(count);
        int columns = ret.at(0);
        int rows = ret.at(1);

        setColumnCount(columns);
        setRowCount(rows);

        setFixedSize(width * columns + 5, rows * height + 5);

        for (int i=0; i < count; i++){
            int _row = i / columns;
            int _column = i % columns;

            DesktopItemInfo fileInfo = itemInfos.at(i);
            QString displayName = fileInfo.DisplayName;
            QString uri = fileInfo.URI;
            QString url = decodeUrl(uri);
            QString icon = fileInfo.Icon;

            DesktopItem*  pDesktopItem = new DesktopItem(defaut_icon, displayName, this);
            pDesktopItem->setUrl(url);
            pDesktopItem->setDesktopIcon(icon);
            pDesktopItem->setDesktopItemInfo(fileInfo);
            pDesktopItem->resize(width, height);
            pDesktopItem->setIsInAppGroup(true);

            setCellWidget(_row, _column, pDesktopItem);
            setRowHeight(_row, height);
            setColumnWidth(_column, width);
        }
    }
}

QList<int> AppTableWidget::getColumnRowByCount(int count){
//    int width = gridManager->getItemWidth();
//    int height = gridManager->getItemHeight();
//    int desktopWidth = qApp->desktop()->availableGeometry().width();
//    int desktopHeight = qApp->desktop()->availableGeometry().height();
    int i = qFloor(qSqrt((double)count));
    int j = 0;
    if (count % i > 0){
        j = count / i + 1;
    }else{
        j = count / i;
    }
    QList<int> ret;
    int column = qMax(i, j);
    int row = qMin(i, j);

    if (column > 6){
        column = 6;
        if (count % column == 0){
            row = count / column;
        }else{
            row = count / column + 1;
        }
    }
    ret.append(column);
    ret.append(row);

    return ret;
}

AppTableWidget::~AppTableWidget()
{

}

