#ifndef DESKTOPITEM_H
#define DESKTOPITEM_H

#include "widgets/elidelabel.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopItem;
typedef QSharedPointer<DesktopItem> DesktopItemPointer;

class DesktopItem : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)
    Q_PROPERTY(bool hover READ isHover WRITE setHover)
public:

    explicit DesktopItem(QWidget *parent = 0);
    explicit DesktopItem(QString icon, QString name, QWidget *parent = 0);
    explicit DesktopItem(QString url, QString icon, QString name, QWidget *parent = 0);
    ~DesktopItem();

    void initUI();
    void initConnect();

    QString getDesktopIcon();
    QString getDesktopName();
    QString getUrl();
    bool isChecked();
    bool isHover();

signals:
    void desktopIconChanged(QString icon);
    void desktopNameChanged(QString name);
    void hoverChanged(bool hover);
    void checkedChanged(bool checked);

public slots:
    void setDesktopIcon(QString icon);
    void setDesktopName(QString name);
    void setChecked(bool checked);
    void setHover(bool hover);
    void setUrl(QString url);

protected:
    void moveEvent(QMoveEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    QLabel* iconLabel;
    ElidedLabel* nameLabel;
    QString m_desktopIcon;
    QString m_desktopName;
    QString m_url;
    bool m_checked;
    bool m_hover;

};

#endif // DESKTOPITEM_H
