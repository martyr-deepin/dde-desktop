#include "desktopitem.h"
#include "widgets/elidelabel.h"
#include "widgets/growingelidetextedit.h"
#include "widgets/util.h"
#include "global.h"
#include "controllers/dbuscontroller.h"
#include "appgroupiconframe.h"
#include "desktopframe.h"
#include <QSvgRenderer>
#include <QImage>
#include <QGraphicsEffect>


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

    m_textedit = new GrowingElideTextEdit();
    m_textedit->setObjectName("GrowingElideTextEdit");
    m_textedit->setFixedWidth(100 - 10);
    addTextShadow();

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
        QMimeDatabase mimeDataBae;
        QMimeType mimeType = mimeDataBae.mimeTypeForFile(deleteFilePrefix(m_url));

        QString cacheIcon = joinPath(getThumbnailsPath(), QFileInfo(icon).fileName());
        if (mimeType.genericIconName() == "image-x-generic"){
            if (QFileInfo(cacheIcon).exists()){
                m_desktopIcon = QPixmap(cacheIcon);
            }else{
                m_desktopIcon = applyShadowToPixmap(icon);
                m_desktopIcon.save(cacheIcon);
            }
            addImageShadow();
        }else if (mimeType.genericIconName() == "text-x-generic" && !isDesktopAppFile(m_url)){
            if (QFileInfo(cacheIcon).exists()){
                m_desktopIcon = QPixmap(cacheIcon);
            }else{
                m_desktopIcon = QPixmap(icon);
                QPixmap mask(m_iconLabel->size());
                QSvgRenderer maskRenderer(QString(":/images/skin/images/diban.svg"));
                mask.fill(Qt::transparent);
                QPainter painter;
                painter.begin(&mask);
                maskRenderer.render(&painter);
                painter.end();
                m_desktopIcon.setMask(mask.scaled(m_desktopIcon.size()).mask());

                QPixmap s(":/images/skin/images/textMask.png");
                QPainter painter2;
                painter2.begin(&s);
                painter2.setRenderHint(QPainter::Antialiasing);
                painter2.setRenderHint(QPainter::SmoothPixmapTransform);
                painter2.drawPixmap(3, 4, m_desktopIcon.scaled(QSize(44, 44)));
                painter2.end();
                m_desktopIcon = s;
                m_desktopIcon.save(cacheIcon);
            }
        }else{
            m_desktopIcon = QPixmap(icon);
        }
    }
    emit desktopIconChanged(icon);
    m_iconLabel->setPixmap(m_desktopIcon.scaled(m_iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//  m_iconLabel->setPixmap(m_desktopIcon);
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
        update();
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
     if (m_checked != checked){
        if (checked){
            setObjectName(QString("Checked"));
            if (isShowSimpleMode() && isExpanded){
                 showFullWrapName();
            }
        }else{
            setObjectName(QString("Normal"));
            if (!isEditing()){
                showSimpWrapName();
            }
        }
        m_checked = checked;
        m_hover = false;
        update();
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

void DesktopItem::paintEvent(QPaintEvent *event){
    if (m_checked){
        QPainter painter(this);
        painter.setPen(QPen(QColor(255, 255, 255, 51), 2));
        painter.setBrush(QColor(0, 0 , 0, 76));
        painter.setRenderHint(QPainter::Antialiasing, true);
        QRect r(2, 2, width() - 4, height() - 4);
        painter.drawRoundedRect(r, 10, 10, Qt::RelativeSize);
    }
    if (m_hover){
        QPainter painter(this);
        painter.setPen(QPen(QColor(255, 255, 255, 36), 2));
        painter.setBrush(QColor(0, 0 , 0, 36));
        painter.setRenderHint(QPainter::Antialiasing, true);
        QRect r(2, 2, width() - 4, height() - 4);
        painter.drawRoundedRect(r, 10, 10, Qt::RelativeSize);
    }
    QFrame::paintEvent(event);
}

bool DesktopItem::isEditing(){
    return m_isEditing;
}

void DesktopItem::setEdited(bool flag){
    m_isEditing = flag;
}

void DesktopItem::setEditing(){
    setEdited(true);
    setChecked(false);
    getTextEdit()->showEditing();
    getTextEdit()->setFocus();
}

QPixmap DesktopItem::applyShadowToPixmap(const QString filename){
    QImage source(filename);
    QSize imageSize = source.size();
    QRect borderRect(0, 0, imageSize.width() + 4, imageSize.height() + 4);
    QPixmap ret(borderRect.size());

    QPainter painter;
    painter.begin(&ret);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(borderRect, Qt::white);
    painter.drawImage(2, 2, source);
    painter.end();

    return ret;
}

void DesktopItem::addImageShadow(){
    QGraphicsDropShadowEffect *coverShadow = new QGraphicsDropShadowEffect;
    coverShadow->setBlurRadius(6);
    coverShadow->setColor(QColor(0, 0, 0, 76));
    coverShadow->setOffset(0, 2);
    m_iconLabel->setGraphicsEffect(coverShadow);
}

void DesktopItem::addTextShadow(){
    QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect;
    textShadow->setBlurRadius(6);
    textShadow->setColor(QColor(0, 0, 0, 178));
    textShadow->setOffset(1, 3);
    m_textedit->setGraphicsEffect(textShadow);
}

DesktopItem::~DesktopItem()
{
//    qDebug() << this << "Desktop Item delete";
}

