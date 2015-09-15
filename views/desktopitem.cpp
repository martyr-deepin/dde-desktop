#include "desktopitem.h"
#include "widgets/elidelabel.h"
#include "widgets/growingelidetextedit.h"
#include "widgets/util.h"
#include "global.h"
#include "controllers/dbuscontroller.h"
#include "appgroupiconframe.h"
#include "desktopframe.h"
#include <QSvgRenderer>

DesktopItem::DesktopItem(QWidget *parent) : QFrame(parent)
{
    m_desktopIcon = QPixmap();
    init();
}

DesktopItem::DesktopItem(QString icon, QString name, QWidget *parent):
    QFrame(parent)
{

    m_desktopIcon = QPixmap(icon);
    m_desktopName = name;
    init();
}

DesktopItem::DesktopItem(QPixmap icon, QString name, QWidget *parent):
    QFrame(parent)
{
    m_desktopIcon = icon;
    m_desktopName = name;
    init();
}

DesktopItem::DesktopItem(QString url, QString icon, QString name, QWidget *parent):
    QFrame(parent)
{
    m_url = url;
    m_desktopIcon = QPixmap(icon);
    m_desktopName = name;
    m_checked = false;
    m_hover = false;
    init();
}

void DesktopItem::init(){
//    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::ClickFocus);
    setObjectName("DesktopItem");
    initUI();
    initConnect();
    setDesktopIcon(m_desktopIcon);
    setDesktopName(m_desktopName);
}

void DesktopItem::initUI(){
    m_iconLabel = new QLabel;
    m_iconLabel->setObjectName("Icon");
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedSize(48, 48);
    m_iconLabel->setScaledContents(false);
    m_iconLabel->setMask(QBitmap(":/images/skin/mask.png"));

    m_textedit = new GrowingElideTextEdit();
    m_textedit->setObjectName("GrowingElideTextEdit");
    m_textedit->setFixedWidth(100 - 10);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addWidget(m_textedit, 0, Qt::AlignHCenter);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}

void DesktopItem::initConnect(){
    connect(m_textedit, SIGNAL(heightChanged(int)), this, SLOT(updateHeight(int)));
    connect(m_textedit, SIGNAL(renameFinished()), signalManager, SIGNAL(renameFinished()));
}

void DesktopItem::updateHeight(int textHeight){

    int h = m_iconLabel->height() + textHeight;
    if (h > 100){
        setFixedHeight(h + 15);
        m_textedit->setFixedHeight(textHeight);
    }else{
        setFixedHeight(100);
        m_textedit->setFixedHeight(textHeight);
    }
}

GrowingElideTextEdit* DesktopItem::getTextEdit(){
    return m_textedit;
}

void DesktopItem::updateSizeByGridSize(SizeType type){
    switch (type) {
    case SizeType::Small:
        m_iconLabel->setFixedSize(16, 16);
        m_textedit->setFixedWidth(72 - 10);
        break;
    case SizeType::Middle:
        m_iconLabel->setFixedSize(48, 48);
        m_textedit->setFixedWidth(100 - 10);
        break;
    case SizeType::Large:
        m_iconLabel->setFixedSize(96, 96);
        m_textedit->setFixedWidth(140 - 10);
        break;
    default:
        break;
    }
}

DesktopItemInfo& DesktopItem::getDesktopItemInfo(){
    return m_desktopItemInfo;
}


void DesktopItem::setDesktopItemInfo(DesktopItemInfo& desktopItemInfo){
    m_desktopItemInfo = desktopItemInfo;
}


QString DesktopItem::getDesktopName(){
    return m_desktopName;
}

void DesktopItem::setDesktopName(QString name){
    m_desktopName = name;
    emit desktopNameChanged(name);
    m_textedit->setFullText(name);
    if (m_textedit->getTexts().length()  == 1){
        m_textedit->setFixedHeight(26);
    }else{
        m_textedit->setFixedHeight(44);
    }
}

QPixmap DesktopItem::getDesktopIcon(){
    return m_desktopIcon;
}

void DesktopItem::setDesktopIcon(QString icon){
    if (icon.endsWith(".svg")){
        m_desktopIcon = QPixmap(m_iconLabel->size());
        QSvgRenderer renderer(icon);
        m_desktopIcon.fill(Qt::transparent);
        QPainter painter;
        painter.begin(&m_desktopIcon);
        renderer.render(&painter);
        painter.end();
    }else{
        m_desktopIcon = QPixmap(icon);
    }
     emit desktopIconChanged(icon);
     m_iconLabel->setPixmap(m_desktopIcon.scaled(m_iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     m_iconLabel->setPixmap(m_desktopIcon);
}

void DesktopItem::setDesktopIcon(QPixmap &icon){
     m_desktopIcon = icon;
     emit desktopIconChanged(icon);
     m_iconLabel->setPixmap(m_desktopIcon.scaled(m_iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     m_iconLabel->setPixmap(m_desktopIcon);
}


bool DesktopItem::isInAppGroup(){
    return m_isInAppGroup;
}

void DesktopItem::setIsInAppGroup(bool flag){
    m_isInAppGroup = flag;
    if (m_isInAppGroup){
        m_hoverObjectName = "AppGroupHover";
    }else{
        m_hoverObjectName = "Hover";
    }
}

void DesktopItem::updateAppGroupIcon(){

}

DesktopItemInfoMap DesktopItem::getAppGroupItems(){
    return m_appGounpItems;
}

void DesktopItem::setAppGroupItems(QMap<QString, DesktopItemInfo> items){
    m_appGounpItems = items;
    m_desktopIcon = getAppGroupIcon().scaled(m_iconLabel->size());
    m_iconLabel->setPixmap(m_desktopIcon);
}


QPixmap DesktopItem::getAppGroupIcon(){
    QStringList icons;
    for(int i = 0; i < 2; i++){
        for (int j=0; j < 2; j++){
            if (m_appGounpItems.count() > i *2 + j){
                icons.append(m_appGounpItems.values().at(i*2 + j).Icon);
            }
        }
    }
    QPixmap appGroupIcon = AppGroupIconFrame::getPixmap(icons);
    return appGroupIcon;
}

void DesktopItem::changeToBeAppGroupIcon(){
    QStringList icons;
    icons.append(m_desktopItemInfo.Icon);
    QPixmap appGroupIcon = AppGroupIconFrame::getPixmap(icons);
    m_iconLabel->setPixmap(appGroupIcon.scaled(m_iconLabel->size()));
}

void DesktopItem::changeBacktoNormal(){
    m_iconLabel->setPixmap(m_desktopIcon);
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
            setObjectName(m_hoverObjectName);
        }else{
            setObjectName(QString("Normal"));
        }
        m_hover = hover;
        m_checked = false;
        setStyleSheet(qApp->styleSheet());
    }

    if (m_hover!=hover){
        emit hoverChanged(m_hover);
    }

}

void DesktopItem::setHoverObjectName(QString name){
    m_hoverObjectName = name;
}

bool DesktopItem::isChecked(){
    return m_checked;
}

void DesktopItem::setChecked(bool checked, bool isExpanded){
    qDebug() << "======setChecked===========/////////";
     if (m_checked != checked){
        if (checked){
            setObjectName(QString("Checked"));
            if (isShowSimpleMode() && isExpanded){
                 showFullWrapName();
            }
        }else{
            setObjectName(QString("Normal"));
            if (!isEditing()){
                qDebug() << "=================/////////";
                showSimpWrapName();
            }
        }
        m_checked = checked;
        m_hover = false;
        setStyleSheet(qApp->styleSheet());
        emit checkedChanged(checked);
     }
}


void DesktopItem::setAllChecked(bool flag){
    m_isAllChecked = flag;
}


void DesktopItem::setCuted(){
    if (!m_isCuted){
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
        effect->setOpacity(0.5);
        setGraphicsEffect(effect);
    }
    m_isCuted = true;
}

void DesktopItem::cancelCuted(){
    if (m_isCuted){
        static_cast<QGraphicsOpacityEffect*>(graphicsEffect())->setOpacity(1);
    }
    m_isCuted = false;
}

bool DesktopItem::isCuted(){
    return m_isCuted;
}


QString DesktopItem::gridKey(){
    return QString("%1-%2").arg(QString::number(pos().x()), QString::number(pos()   .y()));
}

bool DesktopItem::isShowSimpleMode(){
    return m_textedit->isSimpleWrapMode();
}

void DesktopItem::showFullWrapName(){
    m_textedit->showFullWrapText();
}

void DesktopItem::showSimpWrapName(){
    m_textedit->showSimpleElideText();
}

void DesktopItem::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton){
        if (m_isInAppGroup){
            emit signalManager->contextMenuShowed(m_url, mapToGlobal(event->pos()));
        }
    }
    QFrame::mousePressEvent(event);
}

void DesktopItem::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::RightButton){
        m_mouseRightRelease = true;
    }

    if (m_isEditing){
        showSimpWrapName();
        m_textedit->tryRenamed();
    }

    QFrame::mouseReleaseEvent(event);
}

void DesktopItem::moveEvent(QMoveEvent *event){

    QPoint pos = event->pos();
    m_settings.beginGroup("DesktopItems");
    if (m_url.length() > 0){
        m_settings.setValue(getUrl(), pos);
    }
    m_settings.endGroup();

    QFrame::moveEvent(event);
}

void DesktopItem::enterEvent(QEvent *event){
    setHover(true);
    QFrame::enterEvent(event);
}

void DesktopItem::leaveEvent(QEvent *event){
    setHover(false);
    QFrame::leaveEvent(event);
}

bool DesktopItem::isEditing(){
    return m_isEditing;
}

void DesktopItem::setEdited(bool flag){
    m_isEditing = flag;
}


DesktopItem::~DesktopItem()
{
//    LOG_INFO() << this << "Desktop Item delete";
}

