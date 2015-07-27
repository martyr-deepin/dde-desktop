#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H


#include <QObject>


class SignalManager: public QObject
{
    Q_OBJECT
public:
    static SignalManager* instance();

signals:

private:
    SignalManager();
    ~SignalManager();
    static SignalManager* m_instance;
    Q_DISABLE_COPY(SignalManager)
};

#endif // SIGNALMANAGER_H
