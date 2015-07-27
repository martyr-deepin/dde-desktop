#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QtCore>

class ConsoleAppender;
class RollingFileAppender;

class LogManager : public QObject
{
    Q_OBJECT
public:
    explicit LogManager(QObject *parent = 0);
    ~LogManager();
    static LogManager* instance();

    void initConsoleAppender();
    void initRollingFileAppender();
    void startLog();

signals:

public slots:

private:
    static LogManager* m_instance;
    Q_DISABLE_COPY(LogManager)
    QString m_format;
    QString m_logPath;
    ConsoleAppender* m_consoleAppender;
    RollingFileAppender* m_rollingFileAppender;
};

#endif // LOGMANAGER_H
