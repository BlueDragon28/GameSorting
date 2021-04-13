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

#include "TabAndList.h"
#include "SqlListView.h"
#include "Common.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabBar>
#include <QIcon>
#include <QPushButton>
#include <QStackedLayout>
#include <QInputDialog>

TabAndList::TabAndList(QSqlDatabase& db, QWidget* parent) :
    QWidget(parent),
    m_db(db),
    m_tabBar(new QTabBar(this)),
    m_stackedViews(new QStackedLayout()),
    m_sqlUtilityTable(ListType::GAMELIST, m_db)
{
    setupView();
}

void TabAndList::setupView()
{
    // Creating the two layouts, on vertical, the other horizontal.
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QHBoxLayout* hBarLayout = new QHBoxLayout(this);

    // Creating a pushButton, it'll be use to add new table.
    // Adding the tabBar and the push button to the hBarLayout.
    QIcon addIcon(":/Images/Add.svg");
    QPushButton* addNewTableButton = new QPushButton(addIcon, QString(), this);
    hBarLayout->addWidget(m_tabBar, 1);
    hBarLayout->addWidget(addNewTableButton, 0);

    // Adding the hBarLayout and the stacked widget to the vLayout.
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addLayout(hBarLayout);
    vLayout->addLayout(m_stackedViews, 1);

    // Allow the user to close tabs of the tab bar.
    m_tabBar->setTabsClosable(true);

    connect(addNewTableButton, &QPushButton::clicked, this, &TabAndList::addTable);
    connect(m_tabBar, &QTabBar::currentChanged, this, &TabAndList::tabChanged);
    connect(m_tabBar, &QTabBar::tabCloseRequested, this, &TabAndList::removeTable);
}

void TabAndList::tabChanged(int index)
{
    if (index >= 0 && index < m_stackedViews->count())
        m_stackedViews->setCurrentIndex(index);
}

void TabAndList::addTable()
{
    // Adding a new list table.
    // First, asking the user the new table name.
    bool ok;
    QString tableName = QInputDialog::getText(
        this,
        tr("New Table Name"),
        tr("Table name:"),
        QLineEdit::Normal,
        QString(),
        &ok);
    if (ok && !tableName.isEmpty())
    {
        // Creating a new table using the tableName got from the user.
        SqlListView* newList = new SqlListView(tableName, ListType::GAMELIST, m_db, m_sqlUtilityTable, this);
        m_stackedViews->addWidget(newList);
        m_tabBar->addTab(newList->tableName());
    }
}

void TabAndList::addingItem()
{
    // Adding a new row into at the end of the list.
    SqlListView* listView = reinterpret_cast<SqlListView*>(m_stackedViews->currentWidget());

    if (listView)
        listView->addingItem();
}

void TabAndList::removeTable(int index)
{
    // Removing a table when requested by the user.
    if (index >= 0 && index < m_tabBar->count())
    {
        QWidget* widget = m_stackedViews->widget(index);
        m_stackedViews->removeWidget(widget);
        m_tabBar->removeTab(index);
        delete widget;
    }
}

void TabAndList::newGameList()
{
    // Deleting the existing list and creating a new list.
    for (int i = m_tabBar->count()-1; i >= 0; i--)
        m_tabBar->removeTab(i);
    for (int i = m_stackedViews->count()-1; i >= 0; i--)
    {
        SqlListView* listView = reinterpret_cast<SqlListView*>(m_stackedViews->widget(i));
        m_stackedViews->removeWidget(listView);
        delete listView;
    }
    
    m_sqlUtilityTable.newList(ListType::GAMELIST);
}