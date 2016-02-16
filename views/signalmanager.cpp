/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "signalmanager.h"

SignalManager* SignalManager::m_instance=NULL;

SignalManager::SignalManager()
{

}

SignalManager* SignalManager::instance(){
    static QMutex mutex;
    if (!m_instance) {
        QMutexLocker locker(&mutex);
        if (!m_instance)
            m_instance = new SignalManager;
    }
    return m_instance;
}

SignalManager::~SignalManager()
{

}
