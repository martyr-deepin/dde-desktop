#include "desktopitem.h"
#include "widgets/elidelabel.h"
#include "widgets/util.h"



DesktopItem::DesktopItem(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    _desktopIcon = "";
    _desktopName = "";
    _checked = false;
    _hover = false;

    setObjectName("DesktopItem");

    initUI();
    initConnect();

    emit desktopIconChanged(_desktopIcon);
    emit desktopNameChanged(_desktopName);
}

DesktopItem::DesktopItem(QString icon, QString name, QWidget *parent):
    QFrame(parent)
{
    _desktopIcon = icon;
    _desktopName = name;
    _checked = false;
    _hover = false;
    initUI();
    initConnect();

    emit desktopIconChanged(_desktopIcon);
    emit desktopNameChanged(_desktopName);
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
    return _desktopName;
}

void DesktopItem::setDesktopName(QString name){
    _desktopName = name;
    nameLabel->setFullText(name);
}

QString DesktopItem::getDesktopIcon(){
    return _desktopIcon;
}

void DesktopItem::setDesktopIcon(QString icon){
    _desktopIcon = icon;
    iconLabel->setPixmap(QPixmap(icon).scaled(iconLabel->size()));
}

bool DesktopItem::isHover(){
    return _hover;
}


void DesktopItem::setHover(bool hover){
    if (_hover != hover && !_checked){
        if (hover){
            setObjectName(QString("Hover"));
        }else{
            setObjectName(QString("Normal"));
        }
        _hover = hover;
        setStyleSheet(qApp->styleSheet());
    }

}

bool DesktopItem::isChecked(){
    return _checked;
}


void DesktopItem::setChecked(bool checked){
     if (_checked != checked){
        if (checked){
            setObjectName(QString("Checked"));
        }else{
            setObjectName(QString("Normal"));
        }
        _checked = checked;
        setStyleSheet(qApp->styleSheet());
     }
}


void DesktopItem::enterEvent(QEvent *event){
    emit hoverChanged(true);
}

void DesktopItem::leaveEvent(QEvent *event){
    emit hoverChanged(false);
}

DesktopItem::~DesktopItem()
{
    qDebug() << "Desktop Item delete";
}

