#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QtCore>

class ConsoleAppender;
class RollingFileAppender;

class LogManager
{
public:
    explicit LogManager();
    ~LogManager();
    void initConsoleAppender();
    void initRollingFileAppender();

signals:

public slots:

private:
    QString m_format;
    QString m_logPath;
    ConsoleAppender* m_consoleAppender;
    RollingFileAppender* m_rollingFileAppender;

    LogManager(const LogManager &);
    LogManager & operator = (const LogManager &);
};

#endif // LOGMANAGER_H
