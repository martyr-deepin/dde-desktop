#ifndef SCHEMAWATCHER_H
#define SCHEMAWATCHER_H

#include <QObject>

class QGSettings;
class SchemaWatcher : public QObject
{
    Q_OBJECT
public:
    explicit SchemaWatcher(QObject *parent = 0);

private:
    QGSettings *m_gsettings;

private slots:
    void onGSettingsChanged();
};

#endif // SCHEMAWATCHER_H
