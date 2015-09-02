#include "systembackground.h"
#include <QPainter>
#include <QImage>
#include <QVariant>
#include <QUrl>
#include <QDebug>


QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT void qt_blurImage( QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0 );
QT_END_NAMESPACE

const QString SystemBackground::WallpaperKey = "pictureUri";

SystemBackground::SystemBackground(QSize size, bool isBlur, QObject *parent):
    QObject(parent),
    m_backgroundSize(size),
    m_isBlur(isBlur),
    m_gsettings(new QGSettings("com.deepin.wrap.gnome.desktop.background",
                               "/com/deepin/wrap/gnome/desktop/background/"))
{
    m_backgroundUrl = QUrl(m_gsettings->get(WallpaperKey).toString()).toLocalFile();
    updateBackgroud();
    initConnect();
}

void SystemBackground::initConnect(){
    connect(m_gsettings, SIGNAL(changed(QString)),
            this, SLOT(handleBackgroundChanged(QString)));
}

bool SystemBackground::isBlur() const{
    return m_isBlur;
}

void SystemBackground::setBlur(bool isBlur){
    m_isBlur = isBlur;
    updateBackgroud();
}

QSize SystemBackground::getBackgroundSize() const {
    return m_backgroundSize;
}

void SystemBackground::setBackgroundSize(QSize size){
    m_backgroundSize = size;
    updateBackgroud();
}

const QPixmap& SystemBackground::getBackground(){
    return m_backgroundPixmap;
}

void SystemBackground::updateBackgroud(){
    m_backgroundPixmap = QPixmap(m_backgroundUrl);
    QPixmap tempPixmap = m_backgroundPixmap.scaled(m_backgroundSize);
    if (m_isBlur){
        QPainter painter;
        painter.begin(&tempPixmap);
        QImage backgroundImage = tempPixmap.toImage();
        qt_blurImage(&painter, backgroundImage, 100, false, false);
        painter.end();
    }
    m_backgroundPixmap = tempPixmap;
    emit backgroundChanged(m_backgroundPixmap);
}

void SystemBackground::handleBackgroundChanged(const QString &key){
    if (key == WallpaperKey){
        m_backgroundUrl = QUrl(m_gsettings->get(WallpaperKey).toString()).toLocalFile();
        qDebug() << "background changed: " << m_backgroundUrl;
        updateBackgroud();
    }
}
