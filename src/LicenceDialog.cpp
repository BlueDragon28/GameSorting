/*
* MIT License
*
* This file is part of the GameSorting
*
* Copyright © 2021 Erwan Saclier de la Bâtie
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "LicenceDialog.h"
#include <QLabel>
#include <QVBoxLayout>

LicenceDialog::LicenceDialog(QWidget* parent) :
	QDialog(parent)
{
	setDialogParameters();
	createLicenceText();
}

void LicenceDialog::setDialogParameters()
{
	setModal(false);

	Qt::WindowFlags flags = windowFlags();
	flags &= ~Qt::WindowContextHelpButtonHint;
	setWindowFlags(flags);

	setWindowTitle(tr("Licence"));
}

void LicenceDialog::createLicenceText()
{
	QString strLicence =
		tr("This program is under MIT Licence\n\n"
			"Copyright %1 2021 Erwan Saclier de la B%2tie (Erwan28250)\n\n"
			"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), "
			"to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, "
			"and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\n"
			"The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\n"
			"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, "
			"WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\n"
			"This program use Qt " QT_VERSION_STR ", Qt is licenced under LGPLv3.").arg(QChar('©')).arg(QChar('â'));

	QLabel* licenceLabel = new QLabel();
	licenceLabel->setText(strLicence);
	licenceLabel->setWordWrap(true);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(licenceLabel);

	setLayout(vLayout);
}