#include "desktopitem.h"

#include <QVBoxLayout>
#include <QString>
#include <QDebug>

static QString mstyle = "\
        QFrame {\
            background-color: rgba(0, 0, 0, 0);\
            border: 2px solid rgba(255, 255, 255, 100);\
            border-radius: 4px;\
            color:white\
        }\
        QLabel#Icon {\
            background-color: rgba(0, 0, 0, 0);\
            border: 1px solid rgba(255, 255, 255, 255);\
            border-radius: 4px;\
            color:white\
        }\
        QLabel#Name {\
            background-color: rgba(0, 0, 0, 0);\
            border: 2px solid rgba(255, 255, 255, 255);\
            border-radius: 4px;\
            color:white\
        }\
\
        QFrame#Border{\
            background-color: transparent;\
            border: 2px solid rgba(0, 0, 0, 0);\
            border-radius: 4px;\
            color:white\
        }\
\
        QFrame#Normal{\
            background-color: rgba(0, 0, 0, 0);\
            border: 2px solid rgba(0, 0, 0, 0);\
            border-radius: 4px;\
            color:white\
        }\
\
        QFrame#Hover{\
            background-color: rgba(0, 0, 0, 0.15);\
            border: 2px solid rgba(255, 255, 255, 0.15);\
            border-radius: 4px;\
            color:white\
        }\
\
        QFrame#Checked{\
            background-color: rgba(0, 0, 0, 0.4);\
            border: 2px solid rgba(255, 255, 255, 0.4);\
            border-radius: 4px;\
            color:white\
        }\
\
        QLineEdit {\
            background-color: rgba(0, 0, 0, 0);\
            color:white;\
            border: None\
        }\
";


DesktopItem::DesktopItem(QWidget *parent) : QFrame(parent)
{
    _desktopIcon = "11";
    _desktopName = "225454";
    _checked = false;
    _hover = false;
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
    nameLabel = new QLabel;
    nameLabel->setObjectName("Name");

    iconLabel->resize(64, 64);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    setStyleSheet(mstyle);
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
    nameLabel->setText(name);
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
        setStyleSheet(mstyle);
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
        setStyleSheet(mstyle);
     }
}

DesktopItem::~DesktopItem()
{

}

