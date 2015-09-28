#include "confirmdeletedialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>

ConfirmDeleteDialog::ConfirmDeleteDialog(QWidget *parent) : DBaseDialog(parent)
{
    QString icon = ":/images/skin/dialogs/images/user-trash-full.png";
    QString message = "Are you sure to delete this item?";
    QString tipMessage = "This action cannot be restored";
    QStringList buttons;
    buttons << "Cancel" << "Ok";
    initUI(icon, message, tipMessage, buttons, buttons);
    moveCenter();
    getButtonsGroup()->button(1)->setFocus();
}

void ConfirmDeleteDialog::handleKeyEnter(){
    handleButtonsClicked(1);
}

ConfirmDeleteDialog::~ConfirmDeleteDialog()
{

}

