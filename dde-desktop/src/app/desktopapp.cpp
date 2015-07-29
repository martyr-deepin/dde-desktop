#include "desktopapp.h"
#include "views/global.h"
#include "views/desktopbox.h"
#include "views/desktopitemmanager.h"
#include "views/desktopframe.h"
#include "controllers/appcontroller.h"

DesktopApp::DesktopApp(QObject *parent) : QObject(parent)
{

    qApp->setOrganizationName("Linux Deepin");
    qApp->setOrganizationDomain("linuxdeepin.org");
    qApp->setApplicationName("dde-desktop");

    m_desktopBox = new DesktopBox;
    m_appController = new AppController;

//    XcbMisc::instance()->set_strut_partial(m_desktopBox->winId(),
//                                           XcbMisc::OrientationTop,
//                                           m_desktopBox->height(),
//                                           m_desktopBox->x(),
//                                           m_desktopBox->x() + m_desktopBox->width());

//    loadSettings();

    initConnect();


}

void DesktopApp::initConnect(){
    connect(signalManager, SIGNAL(gridModeChanged(bool)), this, SLOT(saveGridOn(bool)));
    connect(signalManager, SIGNAL(gridSizeTypeChanged(SizeType)), this, SLOT(saveSizeType(SizeType)));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlag)), this, SLOT(saveSortFlag(QDir::SortFlag)));
}

void DesktopApp::show(){
    m_desktopBox->show();
}

void DesktopApp::loadSettings(){

}

void DesktopApp::saveSettings(){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("isGridOn", m_desktopBox->getDesktopFrame()->isGridOn());
    settings.setValue("sizeType", m_desktopBox->getDesktopFrame()->getSizeType());
    settings.setValue("sortFlag", m_desktopBox->getDesktopFrame()->getDesktopItemManager()->getSortFlag());
    settings.endGroup();
}

void DesktopApp::saveGridOn(bool mode){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("isGridOn", mode);
    settings.endGroup();
}

void DesktopApp::saveSizeType(SizeType type){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("sizeType", type);
    settings.endGroup();
}

void DesktopApp::saveSortFlag(QDir::SortFlag flag){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("sortFlag", flag);
    settings.endGroup();
}

DesktopApp::~DesktopApp()
{

}

