#include "keyeventmanager.h"

KeyEventManager::KeyEventManager(QObject *parent) : QObject(parent)
{
    qDebug() << parent;
}

void KeyEventManager::onKeyUpPressed(){

}

void KeyEventManager::onKeyDownPressed(){

}

void KeyEventManager::onKeyLeftPressed(){

}

void KeyEventManager::onKeyRightPressed(){

}


KeyEventManager::~KeyEventManager()
{

}

