/*
* MIT Licence
*
* This file is part of the GameSorting
*
* Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
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

#include "SettingsDialog.h"
#include "Settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog(parent),
    m_legUtilEditCheckBox(nullptr)
{
    /*
    Creating two layout, one vertical and the other horizontal.
    vLayout have the data and the hLayout.
    hLayout have the apply and cancel buttons.
    */
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    
    m_legUtilEditCheckBox = new QCheckBox(tr("Legacy utility editor."), this);
    vLayout->addWidget(m_legUtilEditCheckBox);

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    QPushButton* cancelBtn = new QPushButton(tr("Cancel"), this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    QPushButton* applyBtn = new QPushButton(tr("Apply"), this);
    connect(applyBtn, &QPushButton::clicked, this, &SettingsDialog::apply);

    hLayout->addStretch(1);
    hLayout->addWidget(cancelBtn);
    hLayout->addWidget(applyBtn);
    vLayout->addLayout(hLayout);
}

SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::apply()
{
    // Apply the settings.
#ifdef NDEBUG
	QSettings settings("BlueDragon28", "gamesorting");
#else
	QSettings settings("BlueDragon28", "gamesorting_debug");
#endif

    settings.setValue("settings/isLegacyUtilityEditor", m_legUtilEditCheckBox->isChecked());
    Settings::instance().setLegacyUtilEditor(m_legUtilEditCheckBox->isChecked());

    accept();
}