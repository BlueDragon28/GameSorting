#include "AboutDialog.h"
#include "CommonStruct.h"

#include <QLabel>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget* parent) :
	QDialog(parent)
{
	setDialogParameters();
	createAboutText();
}

void AboutDialog::setDialogParameters()
{
	setModal(false);

	Qt::WindowFlags wFlags = windowFlags();
	wFlags &= ~Qt::WindowContextHelpButtonHint;
	setWindowFlags(wFlags);

	setWindowTitle(tr("About"));
}

void AboutDialog::createAboutText()
{
	QString strAbout =
		tr("<b>About this application :</b><br><br>"
			"<b>Name :</b><br>Game Sorting v" GAMESORTING_VERSION "<br><br>"
			"<b>Library used :</b><br>"
			"Qt " QT_VERSION_STR "<br><br>"
			"<b>Author :</b><br>"
			"Erwan28250");

	QLabel* aboutLabel = new QLabel();
	aboutLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	aboutLabel->setText(strAbout);

	QVBoxLayout *vLayout = new QVBoxLayout();
	vLayout->addWidget(aboutLabel);

	setLayout(vLayout);
}