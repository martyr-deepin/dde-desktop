#ifndef EXECUTABLEOPTIONSDIALOG_H
#define EXECUTABLEOPTIONSDIALOG_H

#include "dbasedialog.h"

class ExecutableOptionsDialog : public DBaseDialog
{
    Q_OBJECT

public:
    static const QString OptionCancel;
    static const QString OptionRun;
    static const QString OptionRunInTerminal;
    static const QString OptionDisplay;

    ExecutableOptionsDialog(QString &icon, QString &fileDisplayName, QWidget * parent = 0);
};

#endif // EXECUTABLEOPTIONSDIALOG_H
