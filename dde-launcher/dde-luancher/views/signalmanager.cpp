#include "signalmanager.h"
#include <QMutex>
#include <QMutexLocker>

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
