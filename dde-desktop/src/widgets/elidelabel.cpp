#include "elidelabel.h"

ElidedLabel::ElidedLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    this->setMinimumWidth(0);
    setTextFormat(Qt::PlainText);
}

ElidedLabel::ElidedLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f), m_fullText(text)
{
    this->setMinimumWidth(0);
    setTextFormat(Qt::PlainText);
}

void ElidedLabel::setFullText(const QString &text)
{
    m_fullText = text;
    this->setText(text);
    elideText();
}

void ElidedLabel::setTextLimitShrink(const QString &text, int width)
{
    this->setMinimumWidth(qMin(this->fontMetrics().width(text), width));
    setFullText(text);
}

void ElidedLabel::setTextLimitExpand(const QString &text)
{
    this->setMaximumWidth(this->fontMetrics().width(text));
    setFullText(text);
}

QString ElidedLabel::fullText() const
{
    return m_fullText;
}

void ElidedLabel::resizeEvent(QResizeEvent *event)
{
    elideText();
    QLabel::resizeEvent(event);
}

void ElidedLabel::elideText()
{
    QFontMetrics fm = this->fontMetrics();
    if (fm.width(this->text()) != this->width()) {
        QString showText = fm.elidedText(m_fullText, Qt::ElideRight, this->width());
        this->setText(showText);
        if (showText != m_fullText) {
            this->setToolTip(m_fullText.left(1024));
        } else {
            this->setToolTip("");
        }
    }
}
