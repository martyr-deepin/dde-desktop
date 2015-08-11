#include "apptablewidget.h"
#include "desktopitem.h"
#include "global.h"
#include "widgets/util.h"
#include <typeinfo>

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
    setEditTriggers(NoEditTriggers);
    setDragDropMode(DragOnly);
    setAcceptDrops(true);
    setDragEnabled(true);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setShowGrid(false);

    setStyleSheet(getQssFromFile(":/skin/qss/Tablewidget.qss"));

    connect(this, SIGNAL(cellClicked(int,int)), this, SLOT(handleCellClicked(int,int)));
    connect(this, SIGNAL(cellPressed(int,int)), this, SLOT(handleCellPressed(int,int)));
    LOG_INFO() << "App Group init finished";
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
            pDesktopItem->setFocusPolicy(Qt::NoFocus);
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

void AppTableWidget::handleCellClicked(int row, int column){
    qDebug() << row << column << "handleCellClicked";
    DesktopItem* pItem = dynamic_cast<DesktopItem*>(cellWidget(row, column));
    if (pItem == NULL){
        LOG_INFO() << "handleCellClicked no desktop item";
    }else{
        emit signalManager->openFile(pItem->getDesktopItemInfo());
    }
}


void AppTableWidget::handleCellPressed(int row, int column){
    qDebug() << row << column << "handleCellPressed";
    DesktopItem* pItem = reinterpret_cast<DesktopItem*>(cellWidget(row, column));
    if (pItem == NULL){
        LOG_INFO() << "handleCellPressed no desktop item";
    }else{
        LOG_INFO() << "handleCellPressed drag desktop item" << pItem;
        m_dragItem = pItem;
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

void AppTableWidget::startDrag(const DesktopItemInfo &info){
//    QByteArray itemData;
//    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    QList<QUrl> urls;
    qDebug() << info.URI;
    urls.append(QUrl(info.URI));
    QMimeData* mimeData = new QMimeData;
//    mimeData->setData("application/x-dnditemdata", itemData);
    mimeData->setUrls(urls);

    QPixmap dragPixmap = getDragPixmap();
    QDrag* pDrag = new QDrag(this);
    pDrag->setMimeData(mimeData);
    pDrag->setPixmap(dragPixmap);
    pDrag->setHotSpot(QCursor::pos());
    Qt::DropAction action = pDrag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
     if (action == Qt::MoveAction){
        LOG_INFO() << "app group item drag move======";
     }else{
        LOG_INFO() << "app group item drag copy======";
     }
}

QPixmap AppTableWidget::getDragPixmap(){
    QFrame* F = new QFrame(this);
    F->setAttribute(Qt::WA_DeleteOnClose);
    F->setObjectName("DesktopFrame");
    F->setGeometry(qApp->desktop()->availableGeometry());

    DesktopItem* item = new DesktopItem(m_dragItem->getDesktopItemInfo().Icon,
                                       m_dragItem->getDesktopName(), F);
    item->resize(m_dragItem->size());
    item->move(mapToGlobal(m_dragItem->pos()));
    item->setObjectName("DragChecked");

    F->setStyleSheet(qApp->styleSheet());
    QPixmap ret = F->grab();
    F->close();
    return ret;
}


void AppTableWidget::mouseMoveEvent(QMouseEvent *event){
    if (m_dragItem){
        qDebug() << "mouseMoveEvent getDesktopItemInfo" << m_dragItem->getDesktopItemInfo().BaseName;
        startDrag(m_dragItem->getDesktopItemInfo());
    }
}

void AppTableWidget::dragEnterEvent(QDragEnterEvent *event){
    LOG_INFO() << "app group enter" << event->pos();
//    m_dragLeave = false;
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }else{
            LOG_INFO() << event->mimeData();
            event->ignore();
        }
    }else{
        event->ignore();
    }
}


void AppTableWidget::dragMoveEvent(QDragMoveEvent *event){
    if (event->mimeData()->hasFormat("application/x-dnditemdata")){
        if (event->source() == this){
             event->setDropAction(Qt::MoveAction);
             event->accept();
        }else{
            event->ignore();;//eptProposedAction();
        }
    }else{
        event->ignore();
    }
}

void AppTableWidget::dragLeaveEvent(QDragLeaveEvent *event){
    //    m_dragLeave = true;
    LOG_INFO() << "app group leave" << event;
    event->ignore();
}

AppTableWidget::~AppTableWidget()
{

}

