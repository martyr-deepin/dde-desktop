#ifndef ELIDELABEL_H
#define ELIDELABEL_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class ElidedLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ElidedLabel(QWidget *parent=0, Qt::WindowFlags f=0);
    explicit ElidedLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);

    void setFullText(const QString &text);
    void setTextLimitShrink(const QString &text, int width);
    void setTextLimitExpand(const QString &text);
    QString fullText() const;

protected:
    void resizeEvent(QResizeEvent * event);

private:
    void elideText();

private:
    QString m_fullText;
};

#endif // ELIDELABEL_H
