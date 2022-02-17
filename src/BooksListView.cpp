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

#include "BooksListView.h"
#include "TableModelBooks.h"
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
#include <iostream>

BooksListView::BooksListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModelBooks(tableName, m_db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    setupWidget();
    setupView();
    connect(m_model, &TableModelBooks::listEdited, this, &BooksListView::listEdited);
}

BooksListView::BooksListView(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(ListType::UNKNOWN),
    m_view(new QTableView(this)),
    m_model(new TableModelBooks(data, db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    m_type = m_model->listType();
    setupWidget();
    setupView();
    setColumnsSizeAndSortingOrder(data);
    connect(m_model, &TableModelBooks::listEdited, this, &BooksListView::listEdited);
}

BooksListView::~BooksListView()
{
    if (m_view)
    {
        QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
        if (itemDelegate)
            delete itemDelegate;
    }
}

void BooksListView::setupWidget()
{
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_view);
    createMenu(vLayout);

    // Allos the user to only select all a row
    m_view->setSelectionBehavior(QTableView::SelectRows);
    m_view->setSelectionMode(QTableView::ExtendedSelection);
    m_view->horizontalHeader()->setSortIndicatorClearable(true);
    m_view->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
    m_view->setSortingEnabled(true);
    m_view->verticalHeader()->hide();

    // Setting custom item delegate ListViewDelegate.
    QAbstractItemDelegate* oldDelegate = m_view->itemDelegate();
    m_view->setItemDelegate(new ListViewDelegate(m_model, m_utilityTable, m_db));
    delete oldDelegate;
}

void BooksListView::setupView()
{
    m_view->setModel(m_model);
}

void BooksListView::addingItem()
{
    if (m_model)
        m_model->appendRows();
}

QString BooksListView::tableName() const
{
    if (m_model)
        return m_model->tableName();
    else
        return QString();
}

void BooksListView::setTableName(const QString& tableName)
{
    if (m_model)
        m_model->setTableName(tableName);
}

void BooksListView::createMenu(QVBoxLayout* vLayout)
{
    if (m_type == ListType::BOOKSLIST)
    {
        QToolBar* toolBar = new QToolBar(tr("Books Menu Bar"), this);
        toolBar->setMovable(false);

        QIcon addIcon(":/Images/Add.svg");
        QAction* addAct = new QAction(addIcon, tr("Add New Item"), this);
        addAct->setToolTip(tr("Adding a new item into the current list."));
        connect(addAct, &QAction::triggered, this, &BooksListView::addingItem);
        connect(m_model, &TableModelBooks::filterChanged, [addAct](bool value) {addAct->setEnabled(!value);});
        toolBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Items"), this);
        delAct->setToolTip(tr("Deleting selected items in the current list."));
        connect(delAct, &QAction::triggered, this, &BooksListView::deletingItems);
        connect(m_model, &TableModelBooks::filterChanged, [delAct](bool value){delAct->setEnabled(!value);}),
        toolBar->addAction(delAct);

        // Move item up and down
        QIcon moveUPIcon(":/Images/MoveUP.svg");
        QAction* moveUPAct = new QAction(moveUPIcon, tr("Move Up"), this);
        moveUPAct->setToolTip(tr("Move the selected items up by one row."));
        connect(moveUPAct, &QAction::triggered, this, &BooksListView::moveItemUp);
        connect(m_model, &TableModelBooks::sortingChanged, [moveUPAct, this](bool value){this->enableAction(moveUPAct);});
        connect(m_model, &TableModelBooks::filterChanged, [moveUPAct, this](bool value){this->enableAction(moveUPAct);});
        toolBar->addAction(moveUPAct);

        QIcon moveDownIcon(":/Images/MoveDown.svg");
        QAction* moveDownAct = new QAction(moveDownIcon, tr("Move Down"), this);
        moveDownAct->setToolTip(tr("Move the selected items down by one row."));
        connect(moveDownAct, &QAction::triggered, this, &BooksListView::moveItemDown);
        connect(m_model, &TableModelBooks::sortingChanged, [moveDownAct, this](bool value){this->enableAction(moveDownAct);});
        connect(m_model, &TableModelBooks::filterChanged, [moveDownAct, this](bool value){this->enableAction(moveDownAct);});
        toolBar->addAction(moveDownAct);

        QIcon moveToIcon(":/Images/MoveTo.svg");
        QAction* moveToAct = new QAction(moveToIcon, tr("Move To"), this);
        moveToAct->setToolTip(tr("Move the selected items to."));
        connect(moveToAct, &QAction::triggered, this, &BooksListView::moveItemTo);
        connect(m_model, &TableModelBooks::sortingChanged, [moveToAct, this](bool value){this->enableAction(moveToAct);});
        connect(m_model, &TableModelBooks::filterChanged, [moveToAct, this](bool value){this->enableAction(moveToAct);});
        toolBar->addAction(moveToAct);

        QIcon updateIcon(":/Images/Update.svg");
        QAction* updateAct = new QAction(updateIcon, tr("Synchronize SQL data with view."), this);
        updateAct->setToolTip(tr("Query all the rows from the list and update the entire view.\n"
                                 "Use it to check if there is no error between the data in the view and the SQL data."));
        connect(updateAct, &QAction::triggered, m_model, &TableModelBooks::updateQuery);
        toolBar->addAction(updateAct);

        // Make the two action for the manipulation of the urls inside a menu controlled by a QToolButton.
        QMenu* urlMenu = new QMenu(tr("Url menu"), this);
        QAction* setUrlAct = new QAction(tr("Set Url"), this);
        setUrlAct->setToolTip(tr("Set the url to the selected books."));
        connect(setUrlAct, &QAction::triggered, this, &BooksListView::setUrl);
        urlMenu->addAction(setUrlAct);

        QAction* openUrlAct = new QAction(tr("Open Url"), this);
        openUrlAct->setToolTip(tr("Open the url of the selected books."));
        connect(openUrlAct, &QAction::triggered, this, &BooksListView::openUrl);
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
        connect(filterAct, &QAction::triggered, this, &BooksListView::filter);
        toolBar->addAction(filterAct);

        vLayout->setMenuBar(toolBar);
    }
}

void BooksListView::deletingItems()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->deleteRows(indexList);
    }
}

QVariant BooksListView::listData() const
{
    // Return the data inside the model.
    if (m_model)
    {
        // Retrive the data of the model
        QVariant variant = m_model->retrieveData();
        // If the list is a books list.
        if (variant.canConvert<Books::SaveDataTable>() && m_view)
        {
            // Retrieve the size of the column.
            Books::SaveDataTable data = qvariant_cast<Books::SaveDataTable>(variant);
            data.viewColumnsSize.name = m_view->columnWidth(Books::NAME);
            data.viewColumnsSize.categories = m_view->columnWidth(Books::CATEGORIES);
            data.viewColumnsSize.authors = m_view->columnWidth(Books::AUTHORS);
            data.viewColumnsSize.publishers = m_view->columnWidth(Books::PUBLISHERS);
            data.viewColumnsSize.services = m_view->columnWidth(Books::SERVICES);
            data.viewColumnsSize.sensitiveContent = m_view->columnWidth(Books::SENSITIVE_CONTENT);
            data.viewColumnsSize.rate = m_view->columnWidth(Books::RATE);
            return QVariant::fromValue(data);
        }
    }

    return QVariant();
}

ListType BooksListView::listType() const
{
    if (m_model)
        return m_model->listType();
    else
        return ListType::UNKNOWN;
}

void BooksListView::setColumnsSizeAndSortingOrder(const QVariant& variant)
{
    // Set the size of the column inside the table view.
    if (m_type == ListType::BOOKSLIST)
    {
        Books::SaveDataTable data = qvariant_cast<Books::SaveDataTable>(variant);
        m_view->setColumnWidth(Books::NAME, data.viewColumnsSize.name);
        m_view->setColumnWidth(Books::CATEGORIES, data.viewColumnsSize.categories);
        m_view->setColumnWidth(Books::AUTHORS, data.viewColumnsSize.authors);
        m_view->setColumnWidth(Books::PUBLISHERS, data.viewColumnsSize.publishers);
        m_view->setColumnWidth(Books::SERVICES, data.viewColumnsSize.services);
        m_view->setColumnWidth(Books::SENSITIVE_CONTENT, data.viewColumnsSize.sensitiveContent);
        m_view->setColumnWidth(Books::RATE, data.viewColumnsSize.rate);

        m_view->horizontalHeader()->setSortIndicator(data.columnSort, (data.sortOrder == 0 ? Qt::AscendingOrder : Qt::DescendingOrder));
    }
}

ViewType BooksListView::viewType() const
{
    return ViewType::BOOKS;
}

void BooksListView::setUrl()
{
    // Set the url to the selected item.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);

        bool result = false;
        QString url = QInputDialog::getText(
            this,
            tr("Set Url: \"%1\"").arg(m_model->data(indexList.at(0)).toString()),
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
            tr("No item selected."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void BooksListView::openUrl()
{
    // Open the url of the selected item into the default web browser.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        QString url = m_model->url(indexList.at(0));
        if (!url.isEmpty())
            QDesktopServices::openUrl(QUrl(url));
        else
            QMessageBox::warning(
                this,
                tr("Open Url!"),
                tr("This item don't have any url."),
                QMessageBox::Ok,
                QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(
            this,
            tr("Open Url!"),
            tr("you need to select an item before."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void BooksListView::moveItemUp()
{
    // Get the selection model
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    // Check if the selection model has selection
    if (selectionModel->hasSelection())
    {
        // Extract the selection
        QModelIndexList indexList = selectionModel->selectedRows(0);
        // Send the selection to the model
        QItemSelection selectedItems = m_model->moveItemsUp(indexList);
        // Select the moved items
        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void BooksListView::moveItemDown()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        QItemSelection selectedItems = m_model->moveItemsDown(indexList);
        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void BooksListView::moveItemTo()
{
    // Move the selected items to hte position X.
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
            0,
            0,
            m_model->size()-indexList.size(),
            1, &result);
        
        if (!result)
            return;
        
        QItemSelection selectedItems = m_model->moveItemsTo(indexList, newPosition);
        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void BooksListView::filter()
{
    // Opening the filter dialog
    FilterDialog filterDialog(m_model, m_model->utilityInterface(), m_utilityTable, m_db, this);
    filterDialog.exec();
}

void BooksListView::enableAction(QAction* action) const
{
    if (m_model->isSortingEnabled() || m_model->isFilterEnabled())
        action->setEnabled(false);
    else
        action->setEnabled(true);
}