#include "views/translucentframe.h"
#include "app/xcb_misc.h"

TranslucentFrame::TranslucentFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setWindowState(Qt::WindowMaximized);
    setGeometry(qApp->desktop()->availableGeometry());
    XcbMisc::instance()->set_window_type(winId(),
                                         XcbMisc::Desktop);
}

TranslucentFrame::~TranslucentFrame()
{
//    LOG_INFO() << "delete";
}
