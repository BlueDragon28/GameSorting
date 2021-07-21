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

#include "UtilitySensitiveContentEditor.h"
#include "TableModel_UtilityInterface.h"
#include "StarWidget.h"
#include "Common.h"

#include <iostream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QSqlError>

UtilitySensitiveContentEditor::UtilitySensitiveContentEditor(
    long long int itemID,
    TableModel_UtilityInterface* dataInterface,
    QSqlDatabase& db,
    QWidget* parent) :
        QDialog(parent),
        m_itemID(itemID),
        m_dataInterface(dataInterface),
        m_db(db),
        m_nExplicitContent(0),
        m_nViolenceContent(0),
        m_nBadLanguageContent(0)
{
    setWindowTitle(tr("Sensitive content selection"));
    retrieveSensitiveContentOnItem();
    createWidgets();
}

UtilitySensitiveContentEditor::~UtilitySensitiveContentEditor()
{}

void UtilitySensitiveContentEditor::createWidgets()
{
    // Main vertical layout
    QVBoxLayout* vLayout = new QVBoxLayout(this);

    // Creating a QFormLayout, it will contain the 3 row
    // of the SensitiveContent SQL table. The from layout will
    // separe the row name and the editor widget in a perfec line.
    QFormLayout* formLayout = new QFormLayout(this);

    // Creating the three spin boxes.
    StarWidget* explicitStar = new StarWidget(this);
    StarWidget* violenceStar = new StarWidget(this);
    StarWidget* languageStar = new StarWidget(this);

    // Set the value of the retrieved value (if any)
    explicitStar->setValue(m_nExplicitContent);
    violenceStar->setValue(m_nViolenceContent);
    languageStar->setValue(m_nBadLanguageContent);

    // Every time the value of the spinboxes are modified,
    // update the member variable of this class.
    connect(explicitStar, &StarWidget::valueChanged, [this](int value){this->m_nExplicitContent = value;});
    connect(violenceStar, &StarWidget::valueChanged, [this](int value){this->m_nViolenceContent = value;});
    connect(languageStar, &StarWidget::valueChanged, [this](int value){this->m_nBadLanguageContent = value;});

    // Adding the three spin boxes into the form layout.
    formLayout->addRow(tr("Explicit Content:"), explicitStar);
    formLayout->addRow(tr("Violence Content:"), violenceStar);
    formLayout->addRow(tr("Bad Language Content:"), languageStar);

    // Adding the form layout into the main vertical layout
    vLayout->addLayout(formLayout);

    // Adding the apply and cancel button
    QPushButton* applyButton = new QPushButton(tr("Apply"), this);
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);

    // Connect applyButton to applyChange member function
    // and connect cancelButton to QObject::deleteLayout member function.
    connect(applyButton, &QPushButton::clicked, this, &UtilitySensitiveContentEditor::applyChange);
    connect(cancelButton, &QPushButton::clicked, this, &QObject::deleteLater);

    // Make the applyButton the default button.
    applyButton->setDefault(true);

    // Adding a horizontal layout to store the buttons.
    QHBoxLayout* hLayout = new QHBoxLayout(this);

    // Adding the button into the hLayout
    hLayout->addStretch(1);
    hLayout->addWidget(applyButton);
    hLayout->addWidget(cancelButton);

    // Adding the horizontal layout into the vertical layout.
    vLayout->addLayout(hLayout);
}

void UtilitySensitiveContentEditor::applyChange()
{
    Game::SensitiveContentData data = {};
    data.explicitContent = m_nExplicitContent;
    data.violenceContent = m_nViolenceContent;
    data.badLanguageContent = m_nBadLanguageContent;
    m_dataInterface->updateItemUtility(m_itemID, UtilityTableName::SENSITIVE_CONTENT, QVariant::fromValue(data));
    deleteLater();
}

void UtilitySensitiveContentEditor::retrieveSensitiveContentOnItem()
{
    // Retrieve the sensitive content of the item (m_itemID).
    QString statement = QString(
        "SELECT\n"
        "   ExplicitContent,\n"
        "   ViolenceContent,\n"
        "   BadLanguage\n"
        "FROM\n"
        "   \"%1\"\n"
        "WHERE\n"
        "   ItemID = %2;")
        .arg(m_dataInterface->tableName(UtilityTableName::SENSITIVE_CONTENT))
        .arg(m_itemID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    QSqlQuery query(m_db);
    if (query.exec(statement))
    {
        if (query.next())
        {
            m_nExplicitContent = inRange(query.value(0).toInt(), 0, 5);
            m_nViolenceContent = inRange(query.value(1).toInt(), 0, 5);
            m_nBadLanguageContent = inRange(query.value(2).toInt(), 0, 5);
        }
    }
    else
        std::cerr << QString("Failed to extract sensitive content of the item %1 from the table %2.\n\t%3")
            .arg(m_itemID)
            .arg(m_dataInterface->tableName(UtilityTableName::SENSITIVE_CONTENT), query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
}