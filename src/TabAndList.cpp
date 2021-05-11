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
#include "TabLineEdit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabBar>
#include <QIcon>
#include <QPushButton>
#include <QStackedLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

TabAndList::TabAndList(QSqlDatabase& db, QWidget* parent) :
    QWidget(parent),
    m_db(db),
    m_tabBar(new QTabBar(this)),
    m_stackedViews(new QStackedLayout()),
    m_listType(ListType::UNKNOWN),
    m_sqlUtilityTable(m_listType, m_db),
    m_isListModified(false)
{
    setupView();
}

TabAndList::~TabAndList()
{}

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

    // Allowing the user to close tabs of the tab bar.
    m_tabBar->setTabsClosable(true);
    // Allowing the user to move the tabs.
    m_tabBar->setMovable(true);

    connect(addNewTableButton, &QPushButton::clicked, this, &TabAndList::addTable);
    connect(m_tabBar, &QTabBar::currentChanged, this, &TabAndList::tabChanged);
    connect(m_tabBar, &QTabBar::tabCloseRequested, this, &TabAndList::removeTable);
    connect(m_tabBar, &QTabBar::tabMoved, this, &TabAndList::tabMoved);
    connect(m_tabBar, &QTabBar::tabBarDoubleClicked, this, &TabAndList::tabAskEdit);
    connect(&m_sqlUtilityTable, &SqlUtilityTable::utilityEdited, this, &TabAndList::listUpdated);
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
        m_isListModified = true;
        emit listChanged(true);
        connect(newList, &GameListView::listEdited, this, &TabAndList::listUpdated);
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
        if (reinterpret_cast<AbstractListView*>(widget)->viewType() == ViewType::GAME)
        {
            m_isListModified = true;
            emit listChanged(true);
        }
        delete widget;
    }
}

void TabAndList::newGameList()
{
    // Deleting the existing list and creating a new list.
    // First, check if there is any unsaved change.
    if (!maybeSave())
        return;

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
    m_isListModified = false;
    m_filePath = QString();
    emit newList(ListType::GAMELIST);
    emit newListFileName(QString());
}

void TabAndList::open()
{
    // Opening a list from a file.
    // First, check if there is unsaved change.
    if (!maybeSave())
        return;

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open list"),
        QString(),
        tr("List File (*.gld);;"
           "All Files (*)"));
        
    if (!filePath.isEmpty())
    {
        if (openFile(filePath))
        {
            m_filePath = filePath;
            emit newListFileName(m_filePath);
        }
        else
        {
            QMessageBox::critical(
                this,
                tr("Failed to open list."),
                tr("Failed to open file file %1.").arg(filePath),
                QMessageBox::Ok,
                QMessageBox::Ok);
            m_isListModified = false;
            newGameList();
            m_listType = ListType::UNKNOWN;
        }
    }
}

void TabAndList::save()
{
    // Saving the list into the current file,
    // or a new file if there is no current file.
    if (m_listType == ListType::GAMELIST)
    {
        if (m_filePath.isEmpty())
        {
            saveAs();
            return;
        }

        if (saveFile(m_filePath))
        {
            m_isListModified = false;
            emit listChanged(false);
        }
        else
            QMessageBox::critical(
                this,
                tr("Save Game List"),
                tr("Saving the game list into the file %1 failed.").arg(m_filePath),
                QMessageBox::Ok,
                QMessageBox::Ok);
    }
    else
        QMessageBox::warning(
            this,
            tr("Saving a list"),
            tr("No list created."),
            QMessageBox::Ok,
            QMessageBox::Ok);
}

void TabAndList::saveAs()
{
    // Saving the list into a new file
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
            {
                m_filePath = filePath;
                m_isListModified = false;
                emit newListFileName(m_filePath);
            }
            else
                QMessageBox::critical(
                    this,
                    tr("Save Game List"),
                    tr("Saving the game list into the file %1 failed.").arg(filePath),
                    QMessageBox::Ok,
                    QMessageBox::Ok);
        }
    }
    else
        QMessageBox::warning(
            this,
            tr("Saving a list"),
            tr("No list created."),
            QMessageBox::Ok,
            QMessageBox::Ok);
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
        m_isListModified = false;
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
            connect(view, &GameListView::listEdited, this, &TabAndList::listUpdated);
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
        UtilityListView* view = new UtilityListView(&m_sqlUtilityTable, tableName, m_db, this);
        m_stackedViews->addWidget(view);
        m_tabBar->addTab(SqlUtilityTable::tableName(tableName));
    }
}

void TabAndList::tabMoved(int from, int to)
{
    // This member function is called when the user moved
    // a tab. This function then change the place of the 
    // data inside the stacked view.
    QWidget* view = m_stackedViews->widget(from);
    m_stackedViews->removeWidget(view);
    m_stackedViews->insertWidget(to, view);
}
void TabAndList::tabAskEdit(int index)
{
    // Show a QLineEdit for the edition of the
    // name of a tab (only if it's not an utility view tab)
    // when the use double click on it.
    AbstractListView* view = reinterpret_cast<AbstractListView*>(m_stackedViews->widget(index));
    // Don't allow renaming utility SQL table.
    if (view->viewType() == ViewType::UTILITY)
        return;

    TabLineEdit* lineEdit = new TabLineEdit(m_tabBar->currentIndex(),
                                            m_tabBar->tabText(m_tabBar->currentIndex()),
                                            m_tabBar->tabRect(m_tabBar->currentIndex()),
                                            this);
    connect(lineEdit, &TabLineEdit::finished, this, &TabAndList::tabChangeApplying);
    lineEdit->show();
    lineEdit->setFocus();
}

void TabAndList::tabChangeApplying(int tabIndex, const QString& tabName)
{
    // Apply the change to the tab and the SQL Table.
    if (tabIndex < 0 || tabIndex >= m_tabBar->count() || tabName.isEmpty())
        return;

    AbstractListView* view = reinterpret_cast<AbstractListView*>(m_stackedViews->widget(tabIndex));

    if (view->viewType() == ViewType::GAME)
    {
        GameListView* gameView = reinterpret_cast<GameListView*>(view);
        gameView->setTableName(tabName);
        m_tabBar->setTabText(tabIndex, gameView->tableName());
    }
}

void TabAndList::listUpdated()
{
    // This member function is called when the list is changed.
    // It is used to update the windows name and ask the user if he realy want to 
    // leave without exiting when he is going to exit the application.

    m_isListModified = true;
    emit listChanged(true);
}

bool TabAndList::maybeSave()
{
    // This function is called when the user try to close the list or close the program.
    // This function check is there is unsaved change and if, ask the user if he want to save them.

    if (m_isListModified)
    {
        QMessageBox::StandardButton button = QMessageBox::warning(
            this,
            tr("Unsaved change"),
            tr("The list have unsaved change, do you want to save them?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);
        
        if (button == QMessageBox::Save)
        {
            save();
            return true;
        }
        else if (button == QMessageBox::Cancel)
            return false;
    }

    return true;
}