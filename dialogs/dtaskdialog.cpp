#include "dtaskdialog.h"
#include "widgets/util.h"
#include "widgets/dcircleprogress.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QCloseEvent>

MoveCopyTaskWidget::MoveCopyTaskWidget(const QMap<QString, QString> &jobDetail, QWidget *parent):
    QFrame(parent),
    m_jobDetail(jobDetail)
{
    initUI();
    initConnect();
    setProgress(50);
    if (m_jobDetail.contains("target")){
        setTargetObj(m_jobDetail.value("target"));
    }

    if (m_jobDetail.contains("destination")){
        setDestinationObj(m_jobDetail.value("destination"));
    }
    setSpeed(25);
}

void MoveCopyTaskWidget::initUI(){
    qDebug() << this << "==========";
    m_cirleWidget = new DCircleProgress;
    m_cirleWidget->setStyleSheet("color: white;font-size: 8px;");
    m_cirleWidget->setFixedSize(32, 32);
    m_cirleWidget->setBackgroundColor(QColor(20, 20, 20));
    m_cirleWidget->setChunkColor(QColor("#3cadff"));
    m_cirleWidget->setLineWidth(2);
    m_closeButton = new QPushButton;
    m_closeButton->setObjectName("CloseButton");
    m_closeButton->setFixedSize(25, 25);
    m_closeButton->setAttribute(Qt::WA_NoMousePropagation);

    m_messageLabel = new QLabel;
    m_messageLabel->setFixedHeight(32);
    m_messageLabel->setObjectName("MessageLabel");
    m_tipMessageLabel = new QLabel;
    m_tipMessageLabel->setFixedHeight(18);
    m_tipMessageLabel->setObjectName("TaskTipMessageLabel");

    QVBoxLayout* messageLayout = new QVBoxLayout;
    messageLayout->addWidget(m_messageLabel);
    messageLayout->addWidget(m_tipMessageLabel);
    messageLayout->addSpacing(12);

    QHBoxLayout* messageBoxLayout = new QHBoxLayout;
    messageBoxLayout->addLayout(messageLayout);
    messageBoxLayout->addWidget(m_closeButton);

    QFrame* lineLabel = new QFrame;
    lineLabel->setFixedHeight(1);
    lineLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0.2)");
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addLayout(messageBoxLayout);
    rightLayout->addWidget(lineLabel);
    rightLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_cirleWidget);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(rightLayout);
    mainLayout->setContentsMargins(5, 0, 5, 0);
    setLayout(mainLayout);
}

void MoveCopyTaskWidget::initConnect(){
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(handleClose()));
}

void MoveCopyTaskWidget::updateMessage(const QMap<QString, QString> &data){
    QString file, destination, speed, remainTime;
    if (data.contains("file")){
        file = data.value("file");
    }
    if (data.contains("destination")){
        destination = data.value("destination");
    }
    if (data.contains("speed")){
        speed = data.value("speed");
    }
    if (data.contains("remainTime")){
        remainTime = data.value("remainTime");
    }

    QString message = tr("<span style=\"color: #3cadff\"> %1 </span> \
                         is moved to <span style=\"color: #3cadff\"> %2 </span>")
            .arg(file, destination);

    QString tipMessage = tr("current speed:%1 time Left:%2 ")
               .arg(speed, remainTime);

    setMessage(message);
    setTipMessage(tipMessage);
}

void MoveCopyTaskWidget::updateTipMessage(){
    QString tipMessage = tr("current speed:%1 time Left:%2 ")
               .arg(QString::number(m_speed), QString::number(m_timeLeft));
    setTipMessage(tipMessage);
}

void MoveCopyTaskWidget::handleClose()
{
    emit closed(m_jobDetail);
}

QString MoveCopyTaskWidget::getTargetObj(){
    return m_targetObj;
}

void MoveCopyTaskWidget::setTargetObj(QString targetObj){
    m_targetObj = targetObj;
}

QString MoveCopyTaskWidget::getDestinationObj(){
    return m_destinationObj;
}

void MoveCopyTaskWidget::setDestinationObj(QString destinationObj){
    m_destinationObj = destinationObj;
}

int MoveCopyTaskWidget::getProgress(){
    return m_progress;
}

void MoveCopyTaskWidget::setProgress(int value){
    m_progress = value;
    m_cirleWidget->setValue(value);
    m_cirleWidget->setText(QString("%1%").arg(QString::number(value)));
}

float MoveCopyTaskWidget::getSpeed(){
    return m_speed;
}

void MoveCopyTaskWidget::setSpeed(float speed){
    m_speed = speed;
}

int MoveCopyTaskWidget::getTimeLeft(){
    return m_timeLeft;
}

void MoveCopyTaskWidget::setTimeLeft(int time){
    m_timeLeft = time;
}

QString MoveCopyTaskWidget::getMessage(){
    return m_message;
}

void MoveCopyTaskWidget::setMessage(QString message){
    m_message = message;
    m_messageLabel->setText(m_message);
}

QString MoveCopyTaskWidget::getTipMessage(){
    return m_tipMessage;
}

void MoveCopyTaskWidget::setTipMessage(QString tipMessage){
    m_tipMessage = tipMessage;
    m_tipMessageLabel->setText(tipMessage);
}


DTaskDialog::DTaskDialog(QWidget *parent) :
    DMovabelDialog(parent)
{
    setModal(false);
    setFixedWidth(420);
    initUI();
    initConnect();
    moveTopRight();
}

void DTaskDialog::initUI(){
    QFrame* contentFrame = new QFrame;
    contentFrame->setObjectName("ContentFrame");

    m_titleLabel = new QLabel;
    m_titleLabel->setObjectName("titleLable");

    m_titleBarCloseButton = new QPushButton(this);
    m_titleBarCloseButton->setObjectName("CloseButton");
    m_titleBarCloseButton->setFixedSize(25, 25);
    m_titleBarCloseButton->setAttribute(Qt::WA_NoMousePropagation);

    m_titleBarMinimizeButton = new QPushButton(this);
    m_titleBarMinimizeButton->setObjectName("MinimizeButton");
    m_titleBarMinimizeButton->setFixedSize(25, 25);
    m_titleBarMinimizeButton->setAttribute(Qt::WA_NoMousePropagation);
    m_titleBarMinimizeButton->hide();

    QFrame* lineLabel = new QFrame;
    lineLabel->setFixedHeight(1);
    lineLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0.2);");

    m_taskListWidget = new QListWidget;
    m_taskListWidget->setSelectionMode(QListWidget::NoSelection);

    QFrame* titleFrame = new QFrame;
    titleFrame->setFixedHeight(25);
    QHBoxLayout* m_titleLayout = new QHBoxLayout;
    m_titleLayout->addWidget(m_titleLabel);
    m_titleLayout->addStretch();
    m_titleLayout->addWidget(m_titleBarMinimizeButton);
    m_titleLayout->addWidget(m_titleBarCloseButton);
    m_titleLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* titleBoxLayout = new QVBoxLayout;
    titleBoxLayout->addLayout(m_titleLayout);
    titleBoxLayout->addWidget(lineLabel);

    QVBoxLayout* m_layout = new QVBoxLayout;
    m_layout->addLayout(titleBoxLayout);
    m_layout->addWidget(m_taskListWidget);
//    m_layout->setSpacing(0);
    contentFrame->setLayout(m_layout);

    QHBoxLayout* contentlayout = new QHBoxLayout;
    contentlayout->addWidget(contentFrame);

    contentlayout->setContentsMargins(5, 5, 5, 5);
    setLayout(contentlayout);

    setStyleSheet(getQssFromFile(":/qss/skin/dialogs/qss/dialogs.qss"));
}

void DTaskDialog::initConnect(){
    connect(m_titleBarCloseButton, SIGNAL(clicked()), this, SLOT(close()));
}

void DTaskDialog::setTitle(QString title){
    m_titleLabel->setText(title);
}

void DTaskDialog::setTitle(int taskCount){
    QString title;
    if (taskCount == 1){
        title = tr("There is %1 task in progress").arg(QString::number(taskCount));
    }else{
        title = tr("There are %1 task in progress").arg(QString::number(taskCount));
    }
    setTitle(title);
}

void DTaskDialog::addCopyMoveTask(const QMap<QString, QString> &jobDetail){
    if (jobDetail.contains("jobPath")){
        MoveCopyTaskWidget* moveWidget = new MoveCopyTaskWidget(jobDetail);
        moveWidget->setFixedHeight(60);
        connect(moveWidget, SIGNAL(closed(QMap<QString,QString>)),
                this, SLOT(handleTaskClose(QMap<QString,QString>)));



        QListWidgetItem* item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(item->sizeHint().width(), 60));
        m_taskListWidget->addItem(item);
        m_taskListWidget->setItemWidget(item, moveWidget);
        m_jobPathItems.insert(jobDetail.value("jobPath"), item);
        setTitle(m_taskListWidget->count());
        adjustSize();
        show();
    }
}

void DTaskDialog::addConflictTask(const QMap<QString, QString> &jobDetail){
    if (jobDetail.contains("jobPath")){
        MoveCopyTaskWidget* moveWidget = new MoveCopyTaskWidget(jobDetail);
        moveWidget->setFixedHeight(85);
        connect(moveWidget, SIGNAL(closed(QMap<QString,QString>)),
                this, SLOT(handleTaskClose(QMap<QString,QString>)));
        QListWidgetItem* item = new QListWidgetItem();
        item->setFlags(Qt::NoItemFlags);
        item->setSizeHint(QSize(item->sizeHint().width(), 85));
        m_taskListWidget->addItem(item);
        m_taskListWidget->setItemWidget(item, moveWidget);
        m_jobPathItems.insert(jobDetail.value("jobPath"), item);
        setTitle(m_taskListWidget->count());
        adjustSize();
        show();
    }
}

void DTaskDialog::adjustSize(){
    int listHeight = 2;
    for(int i=0; i < m_taskListWidget->count(); i++){
        listHeight += m_taskListWidget->itemWidget(m_taskListWidget->item(i))->height();
    }
    if (listHeight < qApp->desktop()->availableGeometry().height() - 40){
        m_taskListWidget->setFixedHeight(listHeight);
        setFixedHeight(listHeight + 60);
    }else{
        setFixedHeight(qApp->desktop()->availableGeometry().height() - 40);
    }
}

void DTaskDialog::removeTaskWidget(QString jobPath){
    if (m_jobPathItems.contains(jobPath)){
        QListWidgetItem* item = m_jobPathItems.value(jobPath);
        m_taskListWidget->removeItemWidget(item);
        m_taskListWidget->takeItem(m_taskListWidget->row(item));
        m_jobPathItems.remove(jobPath);
        setTitle(m_taskListWidget->count());
        if (m_taskListWidget->count() == 0){
            hide();
        }
    }
}

void DTaskDialog::handleTaskClose(const QMap<QString, QString> &jobDetail){
    removeTaskWidget(jobDetail);
    setTitle(m_taskListWidget->count());
    emit abortCopyTask(jobDetail);
}

void DTaskDialog::removeTaskWidget(const QMap<QString, QString> &jobDetail){
    if (jobDetail.contains("jobPath")){
        removeTaskWidget(jobDetail.value("jobPath"));
        adjustSize();
    }
}

void DTaskDialog::handleUpdateTaskWidget(const QMap<QString, QString> &jobDetail,
                                         const QMap<QString, QString> &data){
    if (jobDetail.contains("jobPath")){
        QString jobPath = jobDetail.value("jobPath");
        if (m_jobPathItems.contains(jobPath)){
            QListWidgetItem* item = m_jobPathItems.value(jobPath);
            MoveCopyTaskWidget* w = static_cast<MoveCopyTaskWidget*>(m_taskListWidget->itemWidget(item));
            w->updateMessage(data);
        }
    }
}


void DTaskDialog::closeEvent(QCloseEvent *event){
    qDebug() << "close===========";
    DMovabelDialog::closeEvent(event);
}
