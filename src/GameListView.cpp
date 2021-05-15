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

#include "GameListView.h"
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
#include <QInputDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QToolButton>
#include <QMenu>

#include <iostream>

#include "ListViewDelegate.h"

GameListView::GameListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModel(tableName, m_type, m_db, utilityTable, this)),
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
    m_model(new TableModel(data, db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    m_type = m_model->listType();
    setupWidget();
    setupView();
    setColumnsSize(data);
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
    if (m_model)
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
        toolBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Games"), this);
        delAct->setToolTip(tr("Deleting selected games in the current game list."));
        connect(delAct, &QAction::triggered, this, &GameListView::deletingItems);
        toolBar->addAction(delAct);

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

void GameListView::setColumnsSize(const QVariant& variant)
{
    // Set the size of the columns inside the table view.
    if (m_type == ListType::GAMELIST)
    {
        if (variant.canConvert<Game::SaveDataTable>() && m_view)
        {
            Game::SaveDataTable data = qvariant_cast<Game::SaveDataTable>(variant);
            m_view->setColumnWidth(Game::NAME, data.viewColumnsSize.name);
            m_view->setColumnWidth(Game::CATEGORIES, data.viewColumnsSize.categories);
            m_view->setColumnWidth(Game::DEVELOPPERS, data.viewColumnsSize.developpers);
            m_view->setColumnWidth(Game::PUBLISHERS, data.viewColumnsSize.publishers);
            m_view->setColumnWidth(Game::PLATFORMS, data.viewColumnsSize.platform);
            m_view->setColumnWidth(Game::SERVICES, data.viewColumnsSize.services);
            m_view->setColumnWidth(Game::SENSITIVE_CONTENT, data.viewColumnsSize.sensitiveContent);
            m_view->setColumnWidth(Game::RATE, data.viewColumnsSize.rate);
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
            QString(),
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
            QDesktopServices::openUrl(QUrl(url));
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