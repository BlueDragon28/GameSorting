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

#include "MoviesListView.h"
#include "TableModelMovies.h"
#include "FilterDialog.h"
#include "ListViewDelegate.h"
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

MoviesListView::MoviesListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModelMovies(tableName, m_db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    setupWidget();
    setupView();
    connect(m_model, &TableModelMovies::listEdited, this, &MoviesListView::listEdited);
}

MoviesListView::MoviesListView(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(ListType::UNKNOWN),
    m_view(new QTableView(this)),
    m_model(new TableModelMovies(data, db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    m_type = m_model->listType();
    setupWidget();
    setupView();
    setColumnsSizeAndSortingOrder(data);
    connect(m_model, &TableModelMovies::listEdited, this, &MoviesListView::listEdited);
}

MoviesListView::~MoviesListView()
{
    if (m_view)
    {
        // Deleting the item delegate of the view.
        QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
        if (itemDelegate)
            delete itemDelegate;
    }
}

void MoviesListView::setupWidget()
{
    QVBoxLayout* vLayout = new QVBoxLayout(this);
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

void MoviesListView::setupView()
{
    m_view->setModel(m_model);
}

void MoviesListView::addingItem()
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

QString MoviesListView::tableName() const
{
    if (m_model)
        return m_model->tableName();
    else
        return QString();
}

void MoviesListView::setTableName(const QString& tableName)
{
    if (m_model)
        m_model->setTableName(tableName);
}

void MoviesListView::createMenu(QVBoxLayout* vLayout)
{
    if (m_type == ListType::MOVIESLIST)
    {
        QToolBar* toolBar = new QToolBar(tr("Movie Menu Bar"), this);
        toolBar->setMovable(false);

        QIcon addIcon(":/Images/Add.svg");
        QAction* addAct = new QAction(addIcon, tr("Add New Movie"), this);
        addAct->setToolTip(tr("Adding a new movie into the current movie list."));
        connect(addAct, &QAction::triggered, this, &MoviesListView::addingItem);
        connect(m_model, &TableModelMovies::filterChanged, [addAct](bool value){addAct->setEnabled(!value);});
        toolBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Movies"), this);
        delAct->setToolTip(tr("Deleting selected movies in the current movie list."));
        connect(delAct, &QAction::triggered, this, &MoviesListView::deletingItems);
        connect(m_model, &TableModelMovies::filterChanged, [delAct](bool value){delAct->setEnabled(!value);});
        toolBar->addAction(delAct);

        QIcon copyIcon(":/Images/Copy.svg");
        QAction* copyAct = new QAction(copyIcon, tr("Copy Movies"), this);
        copyAct->setToolTip(tr("Copying Movie(s) into the clipboard."));
        connect(copyAct, &QAction::triggered, this, &MoviesListView::copy);
        connect(m_model, &TableModelMovies::filterChanged, [copyAct](bool value){copyAct->setEnabled(!value);});
        toolBar->addAction(copyAct);

        QIcon pasteIcon(":/Images/Paste.svg");
        QAction* pasteAct = new QAction(pasteIcon, tr("Paste Movies"), this);
        pasteAct->setToolTip(tr("Paste movie(s) from the clipboard into the view."));
        connect(pasteAct, &QAction::triggered, this, &MoviesListView::paste);
        connect(m_model, &TableModelMovies::filterChanged, [pasteAct](bool value){pasteAct->setEnabled(!value);});
        toolBar->addAction(pasteAct);

        // Move item up and down
        QIcon moveUPIcon(":/Images/MoveUP.svg");
        QAction* moveUpAct = new QAction(moveUPIcon, tr("Move up"), this);
        moveUpAct->setToolTip(tr("Move the selected items up by one row."));
        connect(moveUpAct, &QAction::triggered, this, &MoviesListView::moveItemUp);
        connect(m_model, &TableModelMovies::sortingChanged, [moveUpAct, this](bool value) { this->enableAction(moveUpAct, !value); });
        connect(m_model, &TableModelMovies::filterChanged, [moveUpAct, this] (bool value) { this->enableAction(moveUpAct, !value); });
        toolBar->addAction(moveUpAct);

        QIcon moveDownIcon(":/Images/MoveDown.svg");
        QAction* moveDownAct = new QAction(moveDownIcon, tr("Move down"), this);
        moveDownAct->setToolTip(tr("Move the selected items down by one row."));
        connect(moveDownAct, &QAction::triggered, this, &MoviesListView::moveItemDown);
        connect(m_model, &TableModelMovies::sortingChanged, [moveDownAct, this] (bool value) { this->enableAction(moveDownAct, !value); });
        connect(m_model, &TableModelMovies::filterChanged, [moveDownAct, this] (bool value) { this->enableAction(moveDownAct, !value);  });
        toolBar->addAction(moveDownAct);

        QIcon moveToIcon(":/Images/MoveTo.svg");
        QAction* moveToAct = new QAction(moveToIcon, tr("Move to"), this);
        moveToAct->setToolTip(tr("Move the selected items to"));
        connect(moveToAct, &QAction::triggered, this, &MoviesListView::moveItemTo);
        connect(m_model, &TableModelMovies::sortingChanged, [moveToAct, this] (bool value) { this->enableAction(moveToAct, !value); });
        connect(m_model, &TableModelMovies::filterChanged, [moveToAct, this] (bool value) { this->enableAction(moveToAct, !value); });
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
        setUrlAct->setToolTip(tr("Set the url to the selected movie."));
        connect(setUrlAct, &QAction::triggered, this, &MoviesListView::setUrl);
        urlMenu->addAction(setUrlAct);

        QAction* openUrlAct = new QAction(tr("Open Url"), this);
        openUrlAct->setToolTip(tr("Open the url of the selected movie."));
        connect(openUrlAct, &QAction::triggered, this, &MoviesListView::openUrl);
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
        connect(filterAct, &QAction::triggered, this, &MoviesListView::filter);
        toolBar->addAction(filterAct);

        vLayout->setMenuBar(toolBar);
    }
}

void MoviesListView::deletingItems()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->deleteRows(indexList);
    }
}

QVariant MoviesListView::listData() const
{
    // Return the data inside the model.
    if (m_model)
    {
        // Retrieve the data of the model
        QVariant variant = m_model->retrieveData();
        // If the list is a movie list.
        if (variant.canConvert<Movie::SaveDataTable>() && m_view)
        {
            // Retrieve the size of the column.
            Movie::SaveDataTable data = qvariant_cast<Movie::SaveDataTable>(variant);
            data.viewColumnsSize.name = m_view->columnWidth(Movie::NAME);
            data.viewColumnsSize.series = m_view->columnWidth(Movie::SERIES);
            data.viewColumnsSize.categories = m_view->columnWidth(Movie::CATEGORIES);
            data.viewColumnsSize.directors = m_view->columnWidth(Movie::DIRECTORS);
            data.viewColumnsSize.actors = m_view->columnWidth(Movie::ACTORS);
            data.viewColumnsSize.productions = m_view->columnWidth(Movie::PRODUCTIONS);
            data.viewColumnsSize.music = m_view->columnWidth(Movie::MUSIC);
            data.viewColumnsSize.services = m_view->columnWidth(Movie::SERVICES);
            data.viewColumnsSize.sensitiveContent = m_view->columnWidth(Movie::SENSITIVE_CONTENT);
            data.viewColumnsSize.rate = m_view->columnWidth(Movie::RATE);
            return QVariant::fromValue(data);
        }
    }

    return QVariant();
}

ListType MoviesListView::listType() const
{
    if (m_model)
        return m_model->listType();
    else
        return ListType::UNKNOWN;
}

void MoviesListView::setColumnsSizeAndSortingOrder(const QVariant& variant)
{
    // Set the size of the column inside the table view.
    if (m_type == ListType::MOVIESLIST)
    {
        if (variant.canConvert<Movie::SaveDataTable>() && m_view)
        {
            Movie::SaveDataTable data = qvariant_cast<Movie::SaveDataTable>(variant);
            m_view->setColumnWidth(Movie::NAME, data.viewColumnsSize.name);
            m_view->setColumnWidth(Movie::SERIES, data.viewColumnsSize.series);
            m_view->setColumnWidth(Movie::CATEGORIES, data.viewColumnsSize.categories);
            m_view->setColumnWidth(Movie::DIRECTORS, data.viewColumnsSize.directors);
            m_view->setColumnWidth(Movie::ACTORS, data.viewColumnsSize.actors);
            m_view->setColumnWidth(Movie::PRODUCTIONS, data.viewColumnsSize.productions);
            m_view->setColumnWidth(Movie::MUSIC, data.viewColumnsSize.music);
            m_view->setColumnWidth(Movie::SERVICES, data.viewColumnsSize.services);
            m_view->setColumnWidth(Movie::SENSITIVE_CONTENT, data.viewColumnsSize.sensitiveContent);
            m_view->setColumnWidth(Movie::RATE, data.viewColumnsSize.rate);

            m_view->horizontalHeader()->setSortIndicator(data.columnSort, (data.sortOrder == 0 ? Qt::AscendingOrder : Qt::DescendingOrder));
        }
    }
}

ViewType MoviesListView::viewType() const
{
    return ViewType::MOVIE;
}

void MoviesListView::setUrl()
{
    // Set the url to the selected movie.
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
            tr("No movie selected."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void MoviesListView::openUrl()
{
    // Open the url of the selected movie into the default web browser.
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
                tr("This movie does not have any url."),
                QMessageBox::Ok,
                QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(
            this,
            tr("Open Url!"),
            tr("You need to select a movie before."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void MoviesListView::moveItemUp()
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
        // Select the moved items.
        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void MoviesListView::moveItemDown()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        QItemSelection selectedIndex = m_model->moveItemsDown(indexList);
        selectionModel->clear();
        selectionModel->select(selectedIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void MoviesListView::moveItemTo()
{
    // Move the selected items to the position x.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);

        // Ask the user where he want to send the selection.
        bool result = false;
        int newPosition = QInputDialog::getInt(
            this,
            tr("New Item Pos"),
            tr("Position"),
            0, 0,
            m_model->size()-indexList.size(),
            1, &result);
        
        if (!result)
            return;
        
        QItemSelection selectedItems = m_model->moveItemsTo(indexList, newPosition);
        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void MoviesListView::filter()
{
    // Opening the filter dialog.
    FilterDialog filterDialog(m_model, m_model->utilityInterface(), m_utilityTable, m_db, this);
    filterDialog.exec();
}

void MoviesListView::enableAction(QAction* action, bool value) const
{
    if (this->m_model->isSortingEnabled() || this->m_model->isFilterEnabled())
        action->setEnabled(false);
    else
        action->setEnabled(true);
}

void MoviesListView::copy()
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
            tr("No movies selected!"),
            QMessageBox::Ok,
            QMessageBox::Ok);
}

void MoviesListView::paste()
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