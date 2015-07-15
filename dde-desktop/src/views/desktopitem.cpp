#include "desktopitem.h"
#include "widgets/elidelabel.h"
#include "widgets/util.h"
#include "global.h"
#include "controllers/dbuscontroller.h"


DesktopItem::DesktopItem(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_desktopIcon = QPixmap();
    m_desktopName = "";
    m_checked = false;
    m_hover = false;
    setObjectName("DesktopItem");
    initUI();
    initConnect();
    emit desktopIconChanged(m_desktopIcon);
    emit desktopNameChanged(m_desktopName);
}

DesktopItem::DesktopItem(QString icon, QString name, QWidget *parent):
    QFrame(parent)
{
    m_desktopIcon = QPixmap(icon);
    m_desktopName = name;
    m_checked = false;
    m_hover = false;
    setObjectName("DesktopItem");
    initUI();
    initConnect();

    emit desktopIconChanged(m_desktopIcon);
    emit desktopNameChanged(m_desktopName);
}

DesktopItem::DesktopItem(QPixmap icon, QString name, QWidget *parent):
    QFrame(parent)
{
    m_desktopIcon = icon;
    m_desktopName = name;
    m_checked = false;
    m_hover = false;
    setObjectName("DesktopItem");
    initUI();
    initConnect();

    emit desktopIconChanged(m_desktopIcon);
    emit desktopNameChanged(m_desktopName);
}

DesktopItem::DesktopItem(QString url, QString icon, QString name, QWidget *parent):
    QFrame(parent)
{
    m_url = url;
    m_desktopIcon = QPixmap(icon);
    m_desktopName = name;
    m_checked = false;
    m_hover = false;
    setObjectName("DesktopItem");
    initUI();
    initConnect();
    emit desktopIconChanged(m_desktopIcon);
    emit desktopNameChanged(m_desktopName);
}


void DesktopItem::initUI(){
    iconLabel = new QLabel;
    iconLabel->setObjectName("Icon");
    nameLabel = new ElidedLabel;
    nameLabel->setObjectName("Name");

    iconLabel->resize(48, 48);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}

void DesktopItem::initConnect(){
    connect(this, SIGNAL(desktopIconChanged(QString)), this,  SLOT(setDesktopIcon(QString)));
    connect(this, SIGNAL(desktopIconChanged(QPixmap&)), this,  SLOT(setDesktopIcon(QPixmap&)));
    connect(this, SIGNAL(desktopNameChanged(QString)), this, SLOT(setDesktopName(QString)));
    connect(this, SIGNAL(hoverChanged(bool)), this, SLOT(setHover(bool)));
    connect(this, SIGNAL(checkedChanged(bool)), this, SLOT(setChecked(bool)));
}


DesktopItemInfo DesktopItem::getDesktopItemInfo(){
    return m_desktopItemInfo;
}


void DesktopItem::setDesktopItemInfo(DesktopItemInfo desktopItemInfo){
    m_desktopItemInfo = desktopItemInfo;
}


QString DesktopItem::getDesktopName(){
    return m_desktopName;
}

void DesktopItem::setDesktopName(QString name){
    m_desktopName = name;
    nameLabel->setFullText(name);
}

QPixmap DesktopItem::getDesktopIcon(){
    return m_desktopIcon;
}

void DesktopItem::setDesktopIcon(QString icon){
     m_desktopIcon = QPixmap(icon);
     iconLabel->setPixmap(m_desktopIcon.scaled(iconLabel->size()));
}

void DesktopItem::setDesktopIcon(QPixmap &icon){
     m_desktopIcon = icon;
     iconLabel->setPixmap(m_desktopIcon.scaled(iconLabel->size()));
}

void DesktopItem::updateAppGroupIcon(){

}

DesktopItemInfoMap DesktopItem::getAppGroupItems(){
    return m_appGounpItems;
}

void DesktopItem::setAppGroupItems(QMap<QString, DesktopItemInfo> items){
    m_appGounpItems = items;
    m_desktopIcon = getAppGroupIcon().scaled(iconLabel->size());
    iconLabel->setPixmap(m_desktopIcon);
}


QPixmap DesktopItem::getAppGroupIcon(){
    QFrame* appGroupFrame = new QFrame;
    appGroupFrame->setObjectName("AppGroup");
    appGroupFrame->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout();

    for(int i = 0; i < 2; i++){
        for (int j=0; j < 2; j++){
            QLabel* label = new QLabel(appGroupFrame);
            label->setFixedSize(48, 48);
            if (m_appGounpItems.count() > i *2 + j){
                QString style = QString("QLabel{border-image:url(") + QString(m_appGounpItems.values().at(i*2 + j).Icon) + QString(")};");
                qDebug() << style;
                label->setStyleSheet(style);
            }
            layout->addWidget(label, i, j);
        }
    }
    layout->setContentsMargins(5, 5, 5, 5);
    appGroupFrame->setLayout(layout);
    appGroupFrame->setStyleSheet("QFrame#AppGroup{\
                                 background-color: rgba(0, 0, 0, 0);\
                                 border: 6px solid rgba(255, 255, 255, 0.8);\
                                 border-radius: 14px;\
                                 color:white\
                                 }");

    QPixmap appGroupIcon = appGroupFrame->grab();
    appGroupFrame->close();
    return appGroupIcon;
}


void DesktopItem::setUrl(QString url){
    m_url = url;
}

QString DesktopItem::getUrl(){
    return m_url;
}

bool DesktopItem::isHover(){
    return m_hover;
}


void DesktopItem::setHover(bool hover){
    if (m_hover != hover && !m_checked){
        if (hover){
            setObjectName(QString("Hover"));
        }else{
            setObjectName(QString("Normal"));
        }
        m_hover = hover;
        setStyleSheet(qApp->styleSheet());
    }

}

bool DesktopItem::isChecked(){
    return m_checked;
}

void DesktopItem::setChecked(bool checked){
     if (m_checked != checked){
        if (checked){
            setObjectName(QString("Checked"));
        }else{
            setObjectName(QString("Normal"));
        }
        m_checked = checked;
        setStyleSheet(qApp->styleSheet());
     }
}


QString DesktopItem::gridKey(){
    return QString("%1-%2").arg(QString::number(pos().x()), QString::number(pos()   .y()));
}

void DesktopItem::moveEvent(QMoveEvent *event){
    QFrame::moveEvent(event);
}

void DesktopItem::enterEvent(QEvent *event){
    emit hoverChanged(true);
    QFrame::enterEvent(event);
}

void DesktopItem::leaveEvent(QEvent *event){
    emit hoverChanged(false);
    QFrame::leaveEvent(event);
}

DesktopItem::~DesktopItem()
{
//    qDebug() << this << "Desktop Item delete";
}

