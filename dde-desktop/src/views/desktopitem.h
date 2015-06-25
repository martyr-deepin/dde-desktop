#ifndef DESKTOPITEM_H
#define DESKTOPITEM_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QString>


//static const QString mstyle;

class DesktopItem : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)
    Q_PROPERTY(bool hover READ isHover WRITE setHover)
public:

    explicit DesktopItem(QWidget *parent = 0);
    explicit DesktopItem(QString icon, QString name, QWidget *parent = 0);
    ~DesktopItem();

    void initUI();
    void initConnect();

    QString getDesktopIcon();
    QString getDesktopName();
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

private:
    QLabel* iconLabel;
    QLabel* nameLabel;
    QString _desktopIcon;
    QString _desktopName;
    bool _checked;
    bool _hover;

};

#endif // DESKTOPITEM_H
