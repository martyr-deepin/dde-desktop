#include "appgroupiconframe.h"

AppGroupIconFrame::AppGroupIconFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("AppGroup");
    setStyleSheet("QFrame#AppGroup{\
                 background-color: rgba(0, 0, 0, 0);\
                 border: 6px solid rgba(255, 255, 255, 0.8);\
                 border-radius: 14px;\
                 color:white\
                 }");
}

QPixmap AppGroupIconFrame::getPixmap(QStringList icons){
    AppGroupIconFrame* appGroupIconFrame = new AppGroupIconFrame;
    QGridLayout* layout = new QGridLayout;
    for(int i = 0; i < 2; i++){
        for (int j=0; j < 2; j++){
            QLabel* label = new QLabel(appGroupIconFrame);
            label->setFixedSize(48, 48);
            if (icons.count() > i *2 + j){
                QString style = QString("QLabel{border-image:url(") + icons.at(i * 2 + j) + QString(")};");
                label->setStyleSheet(style);
            }
            layout->addWidget(label, i, j);
        }
    }
    layout->setContentsMargins(5, 5, 5, 5);
    appGroupIconFrame->setLayout(layout);
    appGroupIconFrame->setStyleSheet("QFrame#AppGroup{\
                                 background-color: rgba(0, 0, 0, 0);\
                                 border: 6px solid rgba(255, 255, 255, 0.8);\
                                 border-radius: 14px;\
                                 color:white\
                                 }");

    QPixmap appGroupIcon = appGroupIconFrame->grab();
    appGroupIconFrame->close();
    return appGroupIcon;}

AppGroupIconFrame::~AppGroupIconFrame()
{

}

