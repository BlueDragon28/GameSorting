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
#include "GameListView.h"
#include "Common.h"
#include "SaveInterface.h"
#include "UtilityListView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabBar>
#include <QIcon>
#include <QPushButton>
#include <QStackedLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

TabAndList::TabAndList(QSqlDatabase& db, QWidget* parent) :
    QWidget(parent),
    m_db(db),
    m_tabBar(new QTabBar(this)),
    m_stackedViews(new QStackedLayout()),
    m_listType(ListType::UNKNOWN),
    m_sqlUtilityTable(m_listType, m_db)
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
    // First, check if the table is initialized.
    if (m_listType == ListType::UNKNOWN)
        return;

    // Asking the user the new table name.
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
        GameListView* newList = new GameListView(tableName, ListType::GAMELIST, m_db, m_sqlUtilityTable, this);
        m_stackedViews->addWidget(newList);
        m_tabBar->addTab(newList->tableName());
    }
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
        AbstractListView* listView = reinterpret_cast<AbstractListView*>(m_stackedViews->widget(i));
        m_stackedViews->removeWidget(listView);
        delete listView;
    }
    
    m_sqlUtilityTable.newList(ListType::GAMELIST);
    m_listType = ListType::GAMELIST;
    emit newList(ListType::GAMELIST);
}

void TabAndList::open()
{
    // Opening a list from a file.
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open list"),
        QString(),
        tr("List File (*.gld);;"
           "All Files (*)"));
        
    if (!filePath.isEmpty())
    {
        if (!openFile(filePath))
        {
            QMessageBox::critical(
                this,
                tr("Failed to open list."),
                tr("Failed to open file file %1.").arg(filePath),
                QMessageBox::Ok,
                QMessageBox::NoButton);
            newGameList();
            m_listType = ListType::UNKNOWN;
        }
    }
}

void TabAndList::saveAs()
{
    // Saving the file into a new file
    // or by writing over an existing file.
    // Game list.
    if (m_listType == ListType::GAMELIST)
    {
        QString filePath = QFileDialog::getSaveFileName(
            this,
            tr("Save Game List"),
            QString(),
            tr("Game List Data (*.gld);;"
               "All Files (*)"));
        
        if (!filePath.isEmpty())
        {
            if(saveFile(filePath))
                m_filePath = filePath;
            else
                QMessageBox::critical(
                    this,
                    tr("Save Game List"),
                    tr("Saving the game list into the file %1 failed.").arg(filePath),
                    QMessageBox::Ok,
                    QMessageBox::NoButton);
        }
    }
    else
        QMessageBox::warning(
            this,
            tr("Saving a list"),
            tr("No list created."),
            QMessageBox::Ok,
            QMessageBox::NoButton);
}

bool TabAndList::saveFile(const QString& filePath) const
{
    // Saving the list into a file.
    if (m_listType == ListType::GAMELIST)
    {
        // Saving the game list.
        Game::SaveData data = {};

        QVariant variant;

        // Getting the game data.
        for (int i = 0; i < m_tabBar->count(); i++)
        {
            AbstractListView* view = reinterpret_cast<AbstractListView*>(m_stackedViews->widget(i));
            
            if (view->viewType() == ViewType::GAME)
            {
                GameListView* gameView = reinterpret_cast<GameListView*>(view);
                variant = gameView->listData();
                if (!variant.canConvert<Game::SaveDataTable>())
                    return false;
                data.gameTables.append(qvariant_cast<Game::SaveDataTable>(variant));
            }
        }

        // Getting the utility data.
        variant = m_sqlUtilityTable.data();
        if (!variant.canConvert<Game::SaveUtilityData>())
            return false;
        data.utilityData = qvariant_cast<Game::SaveUtilityData>(variant);

        // Save to a file.
        return SaveInterface::save(filePath, QVariant::fromValue(data));
    }

    return false;
}

bool TabAndList::openFile(const QString& filePath)
{
    // Opening file and apply everything into the view.
    QVariant variant;
    // Opening the file.
    bool result = SaveInterface::open(filePath, variant);
    if (!result)
        return false;
    
    // Check if it's a game list data.
    if (variant.canConvert<Game::SaveData>())
    {
        // Creating a new empty game list data.
        newGameList();
        Game::SaveData data = qvariant_cast<Game::SaveData>(variant);
        result = m_sqlUtilityTable.setData(QVariant::fromValue(data.utilityData));
        if (!result)
            return false;
        
        for (int i = 0; i < data.gameTables.size(); i++)
        {
            GameListView* view = new GameListView(QVariant::fromValue(data.gameTables.at(i)), m_db, m_sqlUtilityTable, this);
            if (view->listType() == ListType::UNKNOWN)
                return false;
            m_stackedViews->addWidget(view);
            m_tabBar->addTab(view->tableName());
        }

        return true;
    }

    return false;
}

void TabAndList::openUtility(UtilityTableName tableName)
{
    // Opening the utility editor where table is tableName.
    // But first, check if the editor is not already open.
    bool isUtilityOpen = false;

    for (int i = 0; i < m_stackedViews->count(); i++)
    {
        if (reinterpret_cast<AbstractListView*>(m_stackedViews->widget(i))->viewType() == ViewType::UTILITY)
        {
            if (reinterpret_cast<UtilityListView*>(m_stackedViews->widget(i))->tableName() == tableName)
            {
                isUtilityOpen = true;
                break;
            }
        }
    }

    // If not, let's opening the editor.
    if (!isUtilityOpen)
    {
        UtilityListView* view = new UtilityListView(tableName, m_db, this);
        m_stackedViews->addWidget(view);
        m_tabBar->addTab(SqlUtilityTable::tableName(tableName));
    }
}