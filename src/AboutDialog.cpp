/*
* MIT Licence
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

#include "AboutDialog.h"
#include "Common.h"

#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

AboutDialog::AboutDialog(QWidget* parent) :
    QDialog(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);

    QString strAbout(tr(
        "<b>About this application :</b><br><br>"
        "<b>Name :</b><br>Game Sorting v" GAMESORTING_VERSION "<br><br>"
        "<b>Library used :</b><br>"
        "Qt " QT_VERSION_STR "<br><br>"
        "<b>Author :</b><br>"
        "Erwan Saclier de la Bâtie (Erwan28250)"));

    QLabel *aboutText = new QLabel();
    aboutText->setAlignment(Qt::AlignCenter);
    aboutText->setText(strAbout);
    aboutText->setMargin(5);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(aboutText);

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(0, 0, 6, 6);
    QPushButton* okBtn = new QPushButton(tr("Ok"), this);
    connect(okBtn, &QPushButton::clicked, this, &AboutDialog::accept);
    hLayout->addStretch(1);
    hLayout->addWidget(okBtn, 0);

    vLayout->addWidget(scrollArea, 1);
    vLayout->addLayout(hLayout, 0);
}