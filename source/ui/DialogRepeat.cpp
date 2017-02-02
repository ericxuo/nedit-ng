
#include <QMessageBox>
#include <QIntValidator>
#include "DialogRepeat.h"
#include "TextBuffer.h"
#include "DocumentWidget.h"
#include "macro.h"

DialogRepeat::DialogRepeat(DocumentWidget *document, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f), document_(document) {
	ui.setupUi(this);
	ui.lineEdit->setValidator(new QIntValidator(0, INT_MAX, this));
}

DialogRepeat::~DialogRepeat() {
}

void DialogRepeat::setCommand(const QString &command) {
	
	
	/* make a label for the Last command item of the dialog, which includes
	   the last executed action name */
	int index = command.indexOf(QLatin1Char('('));
	if(index == -1) {
		return;
	}

	lastCommand_ = command;
	ui.radioLastCommand->setText(tr("Last &Command (%1)").arg(command.mid(0, index)));
}

void DialogRepeat::on_buttonBox_accepted() {
	if(!doRepeatDialogAction()) {
		return;
	}
	
	accept();
}

bool DialogRepeat::doRepeatDialogAction() {

    int how;

	// Find out from the dialog how to repeat the command 
	if (ui.radioInSelection->isChecked()) {
        if (!document_->buffer_->primary_.selected) {
			QMessageBox::warning(this, tr("Repeat Macro"), tr("No selection in window to repeat within"));
            return false;
		}
        how = REPEAT_IN_SEL;
	} else if (ui.radioToEnd->isChecked()) {
        how = REPEAT_TO_END;
	} else {
	
		QString strTimes = ui.lineEdit->text();
		
		if(strTimes.isEmpty()) {
			QMessageBox::warning(this, tr("Warning"), tr("Please supply a valur for number of times"));
			return false;
		}
		
		bool ok;
		int nTimes = strTimes.toInt(&ok);
		if(!ok) {
			QMessageBox::warning(this, tr("Warning"), tr("Can't read integer value \"%1\" in number of times").arg(strTimes));
			return false;
		}
	
        how = nTimes;
	}

    QString macro;

	// Figure out which command user wants to repeat 
	if (ui.radioLastCommand->isChecked()) {
        macro = lastCommand_;
	} else {
		if (ReplayMacro.empty()) {
			return false;
		}
        macro = QString::fromStdString(ReplayMacro);
	}

    // call the action routine repeat_macro to do the work
    document_->repeatMacro(macro, how);
	return true;
}
