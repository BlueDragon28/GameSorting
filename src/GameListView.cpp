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

#include "GameListView.h"
#include "TableModelGame.h"
#include "FilterDialog.h"
#include <QTableView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QIcon>
#include <QAction>
#include <QToolBar>
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QToolButton>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

#include <iostream>

#include "ListViewDelegate.h"

GameListView::GameListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModelGame(tableName, m_db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    setupWidget();
    setupView();
    connect(m_model, &TableModel::listEdited, this, &GameListView::listEdited);
}

GameListView::GameListView(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(ListType::UNKNOWN),
    m_view(new QTableView(this)),
    m_model(new TableModelGame(data, db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    m_type = m_model->listType();
    setupWidget();
    setupView();
    setColumnsSizeAndSortingOrder(data);
    connect(m_model, &TableModel::listEdited, this, &GameListView::listEdited);
}

GameListView::~GameListView()
{
    if (m_view)
    {
        // Deleting the item delegate of the view.
        QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
        if (itemDelegate)
            delete itemDelegate;
    }
}

void GameListView::setupWidget()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_view);
    createMenu(vLayout);

    // Allow the user to only select all a row.
    m_view->setSelectionBehavior(QTableView::SelectRows);
    m_view->setSelectionMode(QTableView::ExtendedSelection);
    m_view->horizontalHeader()->setSortIndicatorClearable(true);
    m_view->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    m_view->setSortingEnabled(true);
    m_view->verticalHeader()->hide();

    // Setting the custom item delegate ListViewDelegate.
    QAbstractItemDelegate* oldDelegate = m_view->itemDelegate();
    m_view->setItemDelegate(new ListViewDelegate(m_model, m_utilityTable, m_db));
    delete oldDelegate;
}

void GameListView::setupView()
{
    m_view->setModel(m_model);
}

void GameListView::addingItem()
{
    if (!m_model)
        return;

    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->appendRows(indexList);
    }
    else
        m_model->appendRows();
}

QString GameListView::tableName() const
{
    if (m_model)
        return m_model->tableName();
    else
        return QString();
}

void GameListView::setTableName(const QString& tableName)
{
    if (m_model)
        m_model->setTableName(tableName);
}

void GameListView::createMenu(QVBoxLayout* vLayout)
{
    if (m_type == ListType::GAMELIST)
    {
        QToolBar* toolBar = new QToolBar(tr("Game Menu Bar"), this);
        toolBar->setMovable(false);

        QIcon addIcon(":/Images/Add.svg");
        QAction* addAct = new QAction(addIcon, tr("Add New Game"), this);
        addAct->setToolTip(tr("Adding a new game into the current game list."));
        connect(addAct, &QAction::triggered, this, &GameListView::addingItem);
        connect(m_model, &TableModelGame::filterChanged, [addAct](bool value){addAct->setEnabled(!value);});
        toolBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Games"), this);
        delAct->setToolTip(tr("Deleting selected games in the current game list."));
        connect(delAct, &QAction::triggered, this, &GameListView::deletingItems);
        connect(m_model, &TableModelGame::filterChanged, [delAct](bool value){delAct->setEnabled(!value);});
        toolBar->addAction(delAct);

        QIcon copyIcon(":/Images/Copy.svg");
        QAction* copyAct = new QAction(copyIcon, tr("Copy games"), this);
        copyAct->setToolTip(tr("Copying game(s) into the clipboard."));
        connect(copyAct, &QAction::triggered, this, &GameListView::copy);
        connect(m_model, &TableModelGame::filterChanged, [copyAct](bool value){copyAct->setEnabled(!value);});
        toolBar->addAction(copyAct);

        QIcon pasteIcon(":/Images/Paste.svg");
        QAction* pasteAct = new QAction(pasteIcon, tr("Paste games"), this);
        pasteAct->setToolTip(tr("Paste game(s) from the clipboard into the list."));
        connect(pasteAct, &QAction::triggered, this, &GameListView::paste);
        connect(m_model, &TableModelGame::filterChanged, [pasteAct](bool value){pasteAct->setEnabled(!value);});
        toolBar->addAction(pasteAct);

        // Move item up and down
        QIcon moveUPIcon(":/Images/MoveUP.svg");
        QAction* moveUpAct = new QAction(moveUPIcon, tr("Move up"), this);
        moveUpAct->setToolTip(tr("Move the selected items up by one row."));
        connect(moveUpAct, &QAction::triggered, this, &GameListView::moveItemUp);
        connect(m_model, &TableModelGame::sortingChanged, [moveUpAct, this](bool value) { this->enableAction(moveUpAct, !value); });
        connect(m_model, &TableModelGame::filterChanged, [moveUpAct, this] (bool value) { this->enableAction(moveUpAct, !value); });
        toolBar->addAction(moveUpAct);

        QIcon moveDownIcon(":/Images/MoveDown.svg");
        QAction* moveDownAct = new QAction(moveDownIcon, tr("Move down"), this);
        moveDownAct->setToolTip(tr("Move the selected items down by one row."));
        connect(moveDownAct, &QAction::triggered, this, &GameListView::moveItemDown);
        connect(m_model, &TableModelGame::sortingChanged, [moveDownAct, this] (bool value) { this->enableAction(moveDownAct, !value); });
        connect(m_model, &TableModelGame::filterChanged, [moveDownAct, this] (bool value) { this->enableAction(moveDownAct, !value);  });
        toolBar->addAction(moveDownAct);

        QIcon moveToIcon(":/Images/MoveTo.svg");
        QAction* moveToAct = new QAction(moveToIcon, tr("Move to"), this);
        moveToAct->setToolTip(tr("Move the selected items to"));
        connect(moveToAct, &QAction::triggered, this, &GameListView::moveItemTo);
        connect(m_model, &TableModelGame::sortingChanged, [moveToAct, this] (bool value) { this->enableAction(moveToAct, !value); });
        connect(m_model, &TableModelGame::filterChanged, [moveToAct, this] (bool value) { this->enableAction(moveToAct, !value); });
        toolBar->addAction(moveToAct);

        QIcon updateIcon(":/Images/Update.svg");
        QAction* updateAct = new QAction(updateIcon, tr("Synchronize SQL data with view."), this);
        updateAct->setToolTip(tr("Query all the rows from the list and update the entire view.\n"
                                 "Use it to check if there is no error between the data in the view and the SQL data."));
        connect(updateAct, &QAction::triggered, m_model, &TableModel::updateQuery);
        toolBar->addAction(updateAct);

        // Make the two action for the manipulation of the urls inside a menu controlled by a QToolButton.
        QMenu* urlMenu = new QMenu(tr("Url menu"), this);
        QAction* setUrlAct = new QAction(tr("Set Url"), this);
        setUrlAct->setToolTip(tr("Set the url to the selected game."));
        connect(setUrlAct, &QAction::triggered, this, &GameListView::setUrl);
        urlMenu->addAction(setUrlAct);

        QAction* openUrlAct = new QAction(tr("Open Url"), this);
        openUrlAct->setToolTip(tr("Open the url of the selected game."));
        connect(openUrlAct, &QAction::triggered, this, &GameListView::openUrl);
        urlMenu->addAction(openUrlAct);

        QIcon urlIcon(":/Images/Url.svg");
        QToolButton* urlToolButton = new QToolButton(this);
        urlToolButton->setIcon(urlIcon);
        urlToolButton->setPopupMode(QToolButton::InstantPopup);
        urlToolButton->setMenu(urlMenu);
        toolBar->addWidget(urlToolButton);

        QIcon filterIcon(":/Images/Filter.svg");
        QAction* filterAct = new QAction(filterIcon, tr("Filter"), this);
        filterAct->setToolTip(tr("Applying a filter on the list."));
        connect(filterAct, &QAction::triggered, this, &GameListView::filter);
        toolBar->addAction(filterAct);

        vLayout->setMenuBar(toolBar);
    }
}

void GameListView::deletingItems()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->deleteRows(indexList);
    }
}

QVariant GameListView::listData() const
{
    // Return the data inside the model.
    if (m_model)
    {
        // Retrieve the data of the model.
        QVariant variant = m_model->retrieveData();
        // If the list is a game list.
        if (m_type == ListType::GAMELIST)
        {
            if (variant.canConvert<Game::SaveDataTable>() && m_view)
            {
                // Retrieve the size of the column.
                Game::SaveDataTable data = qvariant_cast<Game::SaveDataTable>(variant);
                data.viewColumnsSize.name = m_view->columnWidth(Game::NAME);
                data.viewColumnsSize.series = m_view->columnWidth(Game::SERIES);
                data.viewColumnsSize.categories = m_view->columnWidth(Game::CATEGORIES);
                data.viewColumnsSize.developpers = m_view->columnWidth(Game::DEVELOPPERS);
                data.viewColumnsSize.publishers = m_view->columnWidth(Game::PUBLISHERS);
                data.viewColumnsSize.platform = m_view->columnWidth(Game::PLATFORMS);
                data.viewColumnsSize.services = m_view->columnWidth(Game::SERVICES);
                data.viewColumnsSize.sensitiveContent = m_view->columnWidth(Game::SENSITIVE_CONTENT);
                data.viewColumnsSize.rate = m_view->columnWidth(Game::RATE);
                return QVariant::fromValue(data);
            }
            else
                return QVariant();
        }
    }
    
    return QVariant();
}

ListType GameListView::listType() const
{
    if (m_model)
        return m_model->listType();
    else
        return ListType::UNKNOWN;
}

void GameListView::setColumnsSizeAndSortingOrder(const QVariant& variant)
{
    // Set the size of the columns inside the table view.
    if (m_type == ListType::GAMELIST)
    {
        if (variant.canConvert<Game::SaveDataTable>() && m_view)
        {
            Game::SaveDataTable data = qvariant_cast<Game::SaveDataTable>(variant);
            m_view->setColumnWidth(Game::NAME, data.viewColumnsSize.name);
            m_view->setColumnWidth(Game::SERIES, data.viewColumnsSize.series);
            m_view->setColumnWidth(Game::CATEGORIES, data.viewColumnsSize.categories);
            m_view->setColumnWidth(Game::DEVELOPPERS, data.viewColumnsSize.developpers);
            m_view->setColumnWidth(Game::PUBLISHERS, data.viewColumnsSize.publishers);
            m_view->setColumnWidth(Game::PLATFORMS, data.viewColumnsSize.platform);
            m_view->setColumnWidth(Game::SERVICES, data.viewColumnsSize.services);
            m_view->setColumnWidth(Game::SENSITIVE_CONTENT, data.viewColumnsSize.sensitiveContent);
            m_view->setColumnWidth(Game::RATE, data.viewColumnsSize.rate);

            m_view->horizontalHeader()->setSortIndicator(data.columnSort, (data.sortOrder == 0 ? Qt::AscendingOrder : Qt::DescendingOrder));
        }
    }
}

ViewType GameListView::viewType() const
{
    return ViewType::GAME;
}

void GameListView::setUrl()
{
    // Set the url to the selected game.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);

        bool result = false;
        QString url = QInputDialog::getText(
            this,
            tr("Set URL: \"%1\"").arg(m_model->data(indexList.at(0)).toString()),
            tr("Url"),
            QLineEdit::Normal,
            m_model->url(indexList.at(0)),
            &result);
        if (result)
            m_model->setUrl(indexList.at(0), url);
    }
    else
    {
        QMessageBox::warning(
            this,
            tr("Set Url!"),
            tr("No game selected."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void GameListView::openUrl()
{
    // Open the url of the selected game into the default web browser.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        QString url = m_model->url(indexList.at(0));
        if (!url.isEmpty())
            QDesktopServices::openUrl(QUrl::fromUserInput(url));
        else
        {
            QMessageBox::warning(
                this,
                tr("Open Url!"),
                tr("This game does not have any url."),
                QMessageBox::Ok,
                QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(
            this,
            tr("Open Url!"),
            tr("You need to select a game before."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void GameListView::moveItemUp()
{
    // Get the selection model
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    // Check if the selection model has selection
    if (selectionModel->hasSelection())
    {
        // Extract the selection.
        QModelIndexList indexList = selectionModel->selectedRows(0);
        // Send the selection to the model.
        QItemSelection selectedItems = m_model->moveItemsUp(indexList);

        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void GameListView::moveItemDown()
{
    // Get the selection model
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    // Check if the selection model has selection
    if (selectionModel->hasSelection())
    {
        // Extract the selection.
        QModelIndexList indexList = selectionModel->selectedRows(0);
        // Send the selection to the model.
        QItemSelection selectedItems = m_model->moveItemsDown(indexList);

        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void GameListView::moveItemTo()
{
    // Move the selected items to the position x.
    // Get the selection model
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    // Check if the selection model has selection
    if (selectionModel->hasSelection())
    {
        // Extract the selection
        QModelIndexList indexList = selectionModel->selectedRows(0);

        // Ask the user where he want to send the selection.
        bool result = false;
        int newPosition = QInputDialog::getInt(
            this,
            tr("New Item Pos"),
            tr("Position"),
            0,
            0,
            m_model->size()-indexList.size(),
            1, &result);
        
        if (!result)
            return;
        
        // Send the selection to the model with the new position.
        QItemSelection selectedItems = m_model->moveItemsTo(indexList, newPosition);

        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select);
    }
}

void GameListView::filter()
{
    // Opening the filter dialog.
    FilterDialog filterDialog(m_model, m_model->utilityInterface(), m_utilityTable, m_db, this);
    filterDialog.exec();
}

void GameListView::enableAction(QAction* action, bool value) const
{
    if (this->m_model->isSortingEnabled() || this->m_model->isFilterEnabled())
        action->setEnabled(false);
    else
        action->setEnabled(true);
}

void GameListView::copy()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->copyToClipboard(indexList);
    }
    else
        QMessageBox::warning(
            this,
            tr("Copy to clipboard"),
            tr("No games selected!"),
            QMessageBox::Ok,
            QMessageBox::Ok);
}

void GameListView::paste()
{
    // Paste the clipboard text into the list.
    QClipboard* clipboard = QApplication::clipboard();
    QString strClipboard = clipboard->text();
    if (!strClipboard.isEmpty())
    {
        QItemSelectionModel* selectionModel = m_view->selectionModel();
        QModelIndexList indexList;
        if (selectionModel->hasSelection())
            indexList = selectionModel->selectedRows(0);

        if (strClipboard.at(0) == ':')
            m_model->appendRows(indexList, strClipboard.split(':', Qt::SkipEmptyParts));
        else
            m_model->appendRows(indexList, {strClipboard});
    }
    else 
        QMessageBox::warning(
            this,
            tr("Paste from clipboard"),
            tr("The clipboard is empty!"),
            QMessageBox::Ok,
            QMessageBox::Ok);
}