#include "desktopitem.h"
#include "widgets/elidelabel.h"
#include "widgets/util.h"


DesktopItem::DesktopItem(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_desktopIcon = "";
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


void DesktopItem::initUI(){
    iconLabel = new QLabel;
    iconLabel->setObjectName("Icon");
    nameLabel = new ElidedLabel;
    nameLabel->setObjectName("Name");

    iconLabel->resize(64, 64);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}

void DesktopItem::initConnect(){
    connect(this, SIGNAL(desktopIconChanged(QString)), this,  SLOT(setDesktopIcon(QString)));
    connect(this, SIGNAL(desktopNameChanged(QString)), this, SLOT(setDesktopName(QString)));
    connect(this, SIGNAL(hoverChanged(bool)), this, SLOT(setHover(bool)));
    connect(this, SIGNAL(checkedChanged(bool)), this, SLOT(setChecked(bool)));
}


QString DesktopItem::getDesktopName(){
    return m_desktopName;
}

void DesktopItem::setDesktopName(QString name){
    m_desktopName = name;
    nameLabel->setFullText(name);
}

QString DesktopItem::getDesktopIcon(){
    return m_desktopIcon;
}

void DesktopItem::setDesktopIcon(QString icon){
    m_desktopIcon = icon;
    iconLabel->setPixmap(QPixmap(icon).scaled(iconLabel->size()));
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

