#include "dbasedialog.h"
#include "widgets/util.h"
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QCloseEvent>


DBaseDialog::DBaseDialog(QWidget *parent):DMovabelDialog(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void DBaseDialog::initUI(const QString &icon,
                         const QString &message,
                         const QString &tipMessage,
                         const QStringList &buttonKeys,
                         const QStringList &buttonTexts){
    m_icon = icon;
    m_message = message;
    m_tipMessage = tipMessage;
    m_buttonKeys = buttonKeys;
    m_buttonTexts = buttonTexts;

    QFrame* contentFrame = new QFrame;
    contentFrame->setObjectName("ContentFrame");

    m_iconLabel = new QLabel;
    m_iconLabel->setFixedSize(48, 48);
    setIcon(m_icon);

    m_messageLabel = new QLabel;
    m_messageLabel->setObjectName("MessageLabel");
    setMessage(m_message);

    m_tipMessageLabel = new QLabel;
    m_tipMessageLabel->setObjectName("TipMessageLabel");
    setTipMessage(m_tipMessage);

    m_buttonGroup = new QButtonGroup;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    foreach (QString label, m_buttonKeys) {
        QPushButton* button = new QPushButton(label);
        button->setObjectName("ActionButton");
        button->setAttribute(Qt::WA_NoMousePropagation);
        button->setFixedHeight(28);
        m_buttonGroup->addButton(button);
        buttonLayout->addWidget(button);

        int index = m_buttonKeys.indexOf(label);
        if (index < m_buttonKeys.length() - 1){
            QLabel* label = new QLabel;
            label->setObjectName("VLine");
            label->setFixedWidth(1);
            buttonLayout->addWidget(label);
        }
    }

    setButtonTexts(m_buttonTexts);
    buttonLayout->setSpacing(0);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* iconLayout = new QVBoxLayout;
    iconLayout->addWidget(m_iconLabel, Qt::AlignCenter);
    iconLayout->setContentsMargins(26, 22, 26, 22);

    m_messageLayout = new QVBoxLayout;
    m_messageLayout->addWidget(m_messageLabel);
    m_messageLayout->addWidget(m_tipMessageLabel);
    m_messageLayout->addStretch();
    m_messageLayout->setContentsMargins(5, 22, 0, 0);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addLayout(iconLayout);
    topLayout->addLayout(m_messageLayout);


    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    contentFrame->setLayout(mainLayout);

    QHBoxLayout* contentlayout = new QHBoxLayout;
    contentlayout->addWidget(contentFrame);
    contentlayout->setContentsMargins(5, 5, 5, 5);
    setLayout(contentlayout);

    m_closeButton = new QPushButton(this);
    m_closeButton->setObjectName("CloseButton");
    m_closeButton->setFixedSize(25, 25);
    m_closeButton->setAttribute(Qt::WA_NoMousePropagation);

    setFixedSize(m_defaultWidth, m_defaultHeight);
    m_closeButton->move(width() - m_closeButton->width() - 4, 4);

    setStyleSheet(getQssFromFile(":/qss/skin/dialogs/qss/dialogs.qss"));

    initConnect();
}


void DBaseDialog::initConnect(){
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(handleButtonsClicked(int)));
}


QVBoxLayout* DBaseDialog::getMessageLayout(){
    return m_messageLayout;
}

QString DBaseDialog::getIcon(){
    return m_icon;
}

void DBaseDialog::setIcon(const QString &icon){
    m_icon = icon;
    m_iconLabel->setPixmap(QPixmap(m_icon).scaled(m_iconLabel->size()));
}

QString DBaseDialog::getMessage(){
    return m_message;
}

void DBaseDialog::setMessage(const QString &message){
    m_message = message;
    m_messageLabel->setText(m_message);
}

QString DBaseDialog::getTipMessage(){
    return m_tipMessage;
}

void DBaseDialog::setTipMessage(const QString &tipMessage){
    m_tipMessage = tipMessage;
    m_tipMessageLabel->setText(m_tipMessage);
}

QStringList DBaseDialog::getButtons(){
    return m_buttonKeys;
}

void DBaseDialog::setButtons(const QStringList &buttons){
    m_buttonKeys = buttons;
}

QStringList DBaseDialog::getButtonTexts(){
    return m_buttonTexts;
}

void DBaseDialog::setButtonTexts(const QStringList& buttonTexts){
    if (buttonTexts.length() != m_buttonKeys.length()){
        return;
    }
    m_buttonTexts = buttonTexts;
    for (int i = 0; i < m_buttonGroup->buttons().length(); i++) {
        QPushButton* button = reinterpret_cast<QPushButton*>(m_buttonGroup->buttons().at(i));
        button->setText(m_buttonTexts.at(i));
    }
}

void DBaseDialog::handleButtonsClicked(int id){
    close();
    int index = m_buttonGroup->buttons().indexOf(m_buttonGroup->button(id));
    if (m_buttonKeys.length() > index){
        QString key = m_buttonKeys.at(index);
        emit buttonClicked(key);
        emit buttonClicked(index);
    }
}

void DBaseDialog::handleKeyEnter(){

}

void DBaseDialog::closeEvent(QCloseEvent *event){
    emit aboutToClose();
    DMovabelDialog::closeEvent(event);
    emit closed();
}


DBaseDialog::~DBaseDialog()
{

}

