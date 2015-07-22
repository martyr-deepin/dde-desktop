#include "desktopapp.h"
#include "views/global.h"

DesktopApp::DesktopApp(QObject *parent) : QObject(parent)
{

    qApp->setOrganizationName("Linux Deepin");
    qApp->setOrganizationDomain("linuxdeepin.org");
    qApp->setApplicationName("dde-desktop");

    m_desktopFrame = new DesktopFrame;
    m_appController = new AppController;


//    loadSettings();

    initConnect();


}

void DesktopApp::initConnect(){
    connect(signalManager, SIGNAL(gridModeChanged(bool)), this, SLOT(saveGridOn(bool)));
    connect(signalManager, SIGNAL(gridSizeTypeChanged(SizeType)), this, SLOT(saveSizeType(SizeType)));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlag)), this, SLOT(saveSortFlag(QDir::SortFlag)));
}

void DesktopApp::show(){
    m_desktopFrame->show();
    m_desktopFrame->move(100, 400);
}

void DesktopApp::loadSettings(){

}

void DesktopApp::saveSettings(){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("isGridOn", m_desktopFrame->isGridOn());
    settings.setValue("sizeType", m_desktopFrame->getSizeType());
    settings.setValue("sortFlag", m_desktopFrame->getTopDesktopItemManager()->getSortFlag());
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

