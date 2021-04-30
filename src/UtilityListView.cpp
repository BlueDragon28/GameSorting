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

#include "UtilityListView.h"
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QItemSelectionModel>
#include <QHeaderView>

UtilityListView::UtilityListView(UtilityTableName tableName, QSqlDatabase& db, QWidget* parent) :
    AbstractListView(parent),
    m_uName(tableName),
    m_view(nullptr),
    m_model(new UtilityListModel(tableName, db, this))
{
    createMenu();
}

ViewType UtilityListView::viewType() const
{
    return ViewType::UTILITY;
}

void UtilityListView::createMenu()
{
    // Create the main layout and the menus.
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);

    QToolBar* menuBar = new QToolBar(this);

    QIcon addIcon(":/Images/Add.svg");
    QAction* addAct = new QAction(addIcon, tr("Add new item"), this);
    addAct->setToolTip(tr("Adding a new item into the list."));
    connect(addAct, &QAction::triggered, m_model, &UtilityListModel::appendRow);
    menuBar->addAction(addAct);

    QIcon delIcon(":/Images/Del.svg");
    QAction* delAct = new QAction(delIcon, tr("Removing an item"), this);
    delAct->setToolTip(tr("Removing an item from the list."));
    connect(delAct, &QAction::triggered, this, &UtilityListView::deleteSelectedRows);
    menuBar->addAction(delAct);

    QIcon updateIcon(":/Images/Update.svg");
    QAction* updateAct = new QAction(updateIcon, tr("Update the list"), this);
    updateAct->setToolTip(tr("Synchronising the list between the view and the SQL table."));
    connect(updateAct, &QAction::triggered, m_model, &UtilityListModel::queryTable);
    menuBar->addAction(updateAct);

    vLayout->setMenuBar(menuBar);

    createView();
    vLayout->addWidget(m_view);
}

void UtilityListView::createView()
{
    // Create the view and set the model to the view.
    m_view = new QTableView(this);
    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

UtilityTableName UtilityListView::tableName() const
{
    return m_uName;
}

void UtilityListView::deleteSelectedRows()
{
    // Delete the selected rows.
    // Retrieve the selection model from the view.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        // If the there is item selected, delete them.
        QModelIndexList indexList = selectionModel->selectedRows();
        // Send the list to the model.
        m_model->deleteIndexs(indexList);
    }
}