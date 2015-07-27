#ifndef DESKTOPITEM_H
#define DESKTOPITEM_H

#include "dbusinterface/dbustype.h"
#include "desktopenums.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>


class ElidedLabel;

class DesktopItem : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)
    Q_PROPERTY(bool hover READ isHover WRITE setHover)
public:

    explicit DesktopItem(QWidget *parent = 0);
    explicit DesktopItem(QString icon, QString name, QWidget *parent = 0);
    explicit DesktopItem(QPixmap icon, QString name, QWidget *parent = 0);
    explicit DesktopItem(QString url, QString icon, QString name, QWidget *parent = 0);
    ~DesktopItem();

    void initUI();
    void initConnect();

    bool isInAppGroup();
    QPixmap getDesktopIcon();
    QString getDesktopName();
    QPixmap getAppGroupIcon();
    QString getUrl();
    bool isChecked();
    bool isHover();
    bool isCuted();
    QString gridKey();
    DesktopItemInfo& getDesktopItemInfo();
    QMap<QString, DesktopItemInfo> getAppGroupItems();

signals:
    void desktopIconChanged(QString icon);
    void desktopIconChanged(QPixmap& icon);
    void desktopNameChanged(QString name);
    void hoverChanged(bool hover);
    void checkedChanged(bool checked);

public slots:
    void updateSizeByGridSize(SizeType type);
    void setIsInAppGroup(bool flag);
    void setDesktopIcon(QString icon);
    void setDesktopIcon(QPixmap& icon);
    void updateAppGroupIcon();
    void setDesktopName(QString name);
    void setChecked(bool checked);
    void setHover(bool hover);
    void setCuted();
    void cancelCuted();
    void setHoverObjectName(QString name);
    void setUrl(QString url);
    void setDesktopItemInfo(DesktopItemInfo desktopItemInfo);
    void setAppGroupItems(QMap<QString, DesktopItemInfo> items);
    void changeToBeAppGroupIcon();
    void changeBacktoNormal();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void moveEvent(QMoveEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    QLabel* iconLabel;
    bool m_isCuted = false;
    ElidedLabel* nameLabel;
    QPixmap m_desktopIcon;
    QString m_desktopName = "";
    QString m_url;
    bool m_isInAppGroup = false;
    bool m_mouseRightRelease = false;
    bool m_checked = false;
    bool m_hover = false;
    QString m_hoverObjectName = "Hover";
    DesktopItemInfo m_desktopItemInfo;
    QMap<QString, DesktopItemInfo> m_appGounpItems;
    QSettings m_settings;
};

#endif // DESKTOPITEM_H
