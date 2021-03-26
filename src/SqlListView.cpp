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

#include "SqlListView.h"
#include "TableModel.h"
#include <QTableView>
#include <QItemSelectionModel>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QIcon>
#include <QAction>
#include <QToolBar>

#include <iostream>

#include "ListViewDelegate.h"

SqlListView::SqlListView(const QString& tableName, ListType type, QSqlDatabase& db, QWidget* parent) :
    QWidget(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModel(tableName, m_type, m_db, this))
{
    setupWidget();
    setupView();

    addingItem();
    addingItem();
    addingItem();
}

SqlListView::~SqlListView()
{
    if (m_view)
    {
        // Deleting the item delegate of the view.
        QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
        if (itemDelegate)
            delete itemDelegate;
    }
}

void SqlListView::setupWidget()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_view);
    createMenu(vLayout);

    // Allow the user to only select all a row.
    m_view->setSelectionBehavior(QTableView::SelectRows);
    m_view->setSelectionMode(QTableView::ExtendedSelection);

    // Setting the custom item delegate ListViewDelegate.
    QAbstractItemDelegate* oldDelegate = m_view->itemDelegate();
    m_view->setItemDelegate(new ListViewDelegate());
    delete oldDelegate;
}

void SqlListView::setupView()
{
    m_view->setModel(m_model);
}

void SqlListView::addingItem()
{
    m_model->appendRows();
}

QString SqlListView::tableName() const
{
    return m_model->tableName();
}

void SqlListView::createMenu(QVBoxLayout* vLayout)
{
    if (m_type == ListType::GAMELIST)
    {
        QToolBar* menuBar = new QToolBar(tr("Game Menu Bar"), this);
        menuBar->setMovable(false);

        QIcon addIcon(":/Images/Add.svg");
        QAction* addAct = new QAction(addIcon, tr("Add New Game"), this);
        addAct->setToolTip(tr("Adding a new game into the current game list."));
        connect(addAct, &QAction::triggered, this, &SqlListView::addingItem);
        menuBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Games"), this);
        delAct->setToolTip(tr("Deleting selected games in the current game list."));
        connect(delAct, &QAction::triggered, this, &SqlListView::deletingItems);
        menuBar->addAction(delAct);

        QIcon updateIcon(":/Images/Update.svg");
        QAction* updateAct = new QAction(updateIcon, tr("Synchronize SQL data with view."), this);
        updateAct->setToolTip(tr("Query all the rows from the list and update the entire view.\n"
                                 "Use it to check if there is no error between the data in the view and the SQL data."));
        connect(updateAct, &QAction::triggered, m_model, &TableModel::updateQuery);
        menuBar->addAction(updateAct);

        vLayout->setMenuBar(menuBar);
    }
}

void SqlListView::deletingItems()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->deleteRows(indexList);
    }
}