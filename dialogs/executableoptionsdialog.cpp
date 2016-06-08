#include "executableoptionsdialog.h"

const QString ExecutableOptionsDialog::OptionCancel = "option_cancel";
const QString ExecutableOptionsDialog::OptionRun = "option_run";
const QString ExecutableOptionsDialog::OptionRunInTerminal = "option_run_in_terminal";
const QString ExecutableOptionsDialog::OptionDisplay = "option_display";

ExecutableOptionsDialog::ExecutableOptionsDialog(QString &icon, QString &fileDisplayName, QWidget * parent) :
    DBaseDialog(parent)
{
    QString message = tr("Do you wan to run %1 or display its content?").arg(fileDisplayName);
    QString tipMessage = tr("It is an executable text file.");
    QStringList buttonKeys, buttonTexts;
    buttonKeys << OptionCancel << OptionRun << OptionRunInTerminal << OptionDisplay;
    buttonTexts << tr("Cancel") << tr("Run") << tr("Run in terminal") << tr("Display");

    initUI(icon, message, tipMessage, buttonKeys, buttonTexts);
}
