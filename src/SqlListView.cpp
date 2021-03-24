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
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QIcon>
#include <QAction>
#include <QToolBar>

#include <iostream>

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

void SqlListView::setupWidget()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_view);
    createMenu(vLayout);
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

        vLayout->setMenuBar(menuBar);
    }
}