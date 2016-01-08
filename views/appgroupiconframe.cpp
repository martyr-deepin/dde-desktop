#include "appgroupiconframe.h"

AppGroupIconFrame::AppGroupIconFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("AppGroup");
    setStyleSheet("QFrame#AppGroup{"
                  "padding:10px;"
                  "}");
}

QPixmap AppGroupIconFrame::getPixmap(QStringList icons){
    AppGroupIconFrame* appGroupIconFrame = new AppGroupIconFrame;
    QGridLayout* layout = new QGridLayout;
    for(int i = 0; i < 2; i++){
        for (int j=0; j < 2; j++){
            QLabel* label = new QLabel(appGroupIconFrame);
            label->setScaledContents(true);
            label->setObjectName("IconLabel");
            label->setFixedSize(48, 48);
            if (icons.count() > i *2 + j){
                label->setPixmap(QPixmap(icons.at(i*2 + j)));
            }
            layout->addWidget(label, i, j);
        }
    }
    layout->setSpacing(5);
    layout->setContentsMargins(5, 5, 5, 5);
    appGroupIconFrame->setLayout(layout);
    QPixmap appGroupIcon = appGroupIconFrame->grab();
    appGroupIconFrame->close();
    return appGroupIcon;
}

QPixmap AppGroupIconFrame::getPixmap(QList<QPixmap> icons){
    AppGroupIconFrame* appGroupIconFrame = new AppGroupIconFrame;
    QGridLayout* layout = new QGridLayout;
    for(int i = 0; i < 2; i++){
        for (int j=0; j < 2; j++){
            QLabel* label = new QLabel(appGroupIconFrame);

            if (icons.count() > i *2 + j){
                label->setPixmap(icons.at(i*2 + j).scaled(48, 48));
            }
            label->setFixedSize(48, 48);
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
    return appGroupIcon;
    }

void AppGroupIconFrame::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);

    const double borderWidth = 6.0;
    QRectF rect(borderWidth, borderWidth, width() - borderWidth * 2, height() - borderWidth * 2);

    QPainterPath border;
    border.addRoundedRect(rect, 10, 10);

    QPen pen;
    pen.setColor(QColor(255, 255, 255, 255 * 0.8));
    pen.setWidth(borderWidth);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.strokePath(border, pen);
}

AppGroupIconFrame::~AppGroupIconFrame()
{

}
