#include "views/translucentframe.h"
#include "app/xcb_misc.h"

TranslucentFrame::TranslucentFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setGeometry(qApp->desktop()->availableGeometry());
}

TranslucentFrame::~TranslucentFrame()
{
//    LOG_INFO() << "delete";
}
