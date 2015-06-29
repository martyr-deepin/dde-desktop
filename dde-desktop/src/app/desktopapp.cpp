#include "desktopapp.h"

DesktopApp::DesktopApp(QObject *parent) : QObject(parent)
{
    m_desktopFrame = new DesktopFrame;
    m_appController = new AppController;
}

void DesktopApp::show(){
    m_desktopFrame->show();
}

DesktopApp::~DesktopApp()
{

}

