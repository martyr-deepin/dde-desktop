#include "singleton.h"

template<typename T>
Singleton<T>* Singleton<T>::m_instance = NULL;

template<typename T>
Singleton<T>::Singleton()
{

}

template<typename T>
* Singleton<T>::instance(){
    static QMutex mutex;
    if (!m_instance) {
        QMutexLocker locker(&mutex);
        if (!m_instance)
            m_instance = new T;
    }

    return m_instance;
}

template<typename T>
Singleton<T>::~Singleton()
{

}

