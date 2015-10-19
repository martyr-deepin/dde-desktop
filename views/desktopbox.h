#ifndef DESKTOPBOX_H
#define DESKTOPBOX_H

#include "translucentframe.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class DesktopFrame;
class GrowingTextEdit;
class BackgroundLabel;

class DesktopBox : public TranslucentFrame
{
    Q_OBJECT
public:
    explicit DesktopBox(QWidget *parent = 0);
    ~DesktopBox();

    DesktopFrame* getDesktopFrame();

signals:

public slots:
    void handleRename();
    void renameFinished();

    void handleScreenGeometryChanged();

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void closeEvent(QCloseEvent* event);

private:
    GrowingTextEdit* m_textEdit;
    int m_currentPage = 0;
    DesktopFrame* m_desktopFrame;
    BackgroundLabel* m_backgroundLabel;
};

#endif // DESKTOPBOX_H
