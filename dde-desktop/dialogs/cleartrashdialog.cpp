#include "cleartrashdialog.h"
#include <QVBoxLayout>
#include <QLabel>

ClearTrashDialog::ClearTrashDialog(QWidget *parent):
    DBaseDialog(parent)
{

    QString icon = ":/images/skin/dialogs/images/user-trash-full.png";
    QString message = tr("Are you sure to clear items in trash?");
    QString tipMessage = tr("This action cannot be restored");
    QStringList buttons;
    buttons << "Cancel" << "Clear";
    initUI(icon, message, tipMessage, buttons, buttons);
    moveCenter();
}

void ClearTrashDialog::handleKeyEnter(){
    handleButtonsClicked(1);
}

ClearTrashDialog::~ClearTrashDialog()
{

}

