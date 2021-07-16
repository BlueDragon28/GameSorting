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

#include "UtilityInterfaceEditor.h"

#include "TableModel.h"
#include "TableModel_UtilityInterface.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>

UtilityInterfaceEditor::UtilityInterfaceEditor(
    UtilityTableName utilityTableName,
    long long int itemID,
    TableModel* dataModel,
    TableModel_UtilityInterface* dataInterface,
    SqlUtilityTable& utilityData,
    QSqlDatabase& db,
    QWidget* parent) :
        QDialog(parent),
        m_utilityTableName(utilityTableName),
        m_itemID(itemID),
        m_dataModel(dataModel),
        m_utilityData(utilityData),
        m_tableView(nullptr),
        m_model(new UtilityInterfaceEditorModel(
            utilityTableName,
            itemID,
            dataModel,
            dataInterface,
            utilityData,
            db,
            this))
{
    createWidgets();
    setModal(false);
}

UtilityInterfaceEditor::~UtilityInterfaceEditor()
{}

void UtilityInterfaceEditor::createWidgets()
{
    // Create the widget inside the dialog.
    // The table view used to display the available utility to the user.
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSortIndicatorClearable(true);
    m_tableView->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    m_tableView->setSortingEnabled(true);
    m_tableView->verticalHeader()->hide();

    // The buttons used apply change and cancel it.
    QPushButton* applyButton = new QPushButton(tr("Apply"), this);
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);

    // The layout to display correctly the content.
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 5);

    QHBoxLayout* hButtonLayout = new QHBoxLayout(this);
    hButtonLayout->addStretch(1);
    hButtonLayout->addWidget(applyButton);
    hButtonLayout->addWidget(cancelButton);
    hButtonLayout->addSpacing(5);

    vLayout->addWidget(m_tableView);
    vLayout->addLayout(hButtonLayout);

    // Make the apply button the default button to press when pressing enter.
    applyButton->setDefault(true);

    // Apply change when the user is clicking to the applyButton
    // and doing nothing when the user is cliking the cancel button.
    connect(applyButton, &QPushButton::clicked, this, &UtilityInterfaceEditor::applyChange);
    connect(cancelButton, &QPushButton::clicked, this, &UtilityInterfaceEditor::deleteLater);
}

void UtilityInterfaceEditor::applyChange()
{
    m_model->applyChange();
    deleteLater();
}