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

#include "CommonListView.h"
#include "TableModelCommon.h"
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

CommonListView::CommonListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModelCommon(tableName, m_db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    setupWidget();
    setupView();
    connect(m_model, &TableModelCommon::listEdited, this, &CommonListView::listEdited);
}

CommonListView::CommonListView(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(ListType::UNKNOWN),
    m_view(new QTableView(this)),
    m_model(new TableModelCommon(data, db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    m_type = m_model->listType();
    setupWidget();
    setupView();
    setColumnsSizeAndSortingOrder(data);
    connect(m_model, &TableModelCommon::listEdited, this, &CommonListView::listEdited);
}

CommonListView::~CommonListView()
{
    if (m_view)
    {
        QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
        if (itemDelegate)
            delete itemDelegate;
    }
}

void CommonListView::setupWidget()
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

void CommonListView::setupView()
{
    m_view->setModel(m_model);
}

void CommonListView::addingItem()
{
    if (m_model)
        m_model->appendRows();
}

QString CommonListView::tableName() const
{
    if (m_model)
        return m_model->tableName();
    else
        return QString();
}

void CommonListView::setTableName(const QString& tableName)
{
    if (m_model)
        m_model->setTableName(tableName);
}

void CommonListView::createMenu(QVBoxLayout* vLayout)
{
    if (m_type == ListType::COMMONLIST)
    {
        QToolBar* toolBar = new QToolBar(tr("Common Menu Bar"), this);
        toolBar->setMovable(false);

        QIcon addIcon(":/Images/Add.svg");
        QAction* addAct = new QAction(addIcon, tr("Add New Item"), this);
        addAct->setToolTip(tr("Adding a new item into the current list."));
        connect(addAct, &QAction::triggered, this, &CommonListView::addingItem);
        connect(m_model, &TableModelCommon::filterChanged, [addAct](bool value) {addAct->setEnabled(!value);});
        toolBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Items"), this);
        delAct->setToolTip(tr("Deleting selected items in the current list."));
        connect(delAct, &QAction::triggered, this, &CommonListView::deletingItems);
        connect(m_model, &TableModelCommon::filterChanged, [delAct](bool value){delAct->setEnabled(!value);}),
        toolBar->addAction(delAct);

        QIcon copyIcon(":/Images/Copy.svg");
        QAction* copyAct = new QAction(copyIcon, tr("Copy common"), this);
        copyAct->setToolTip(tr("Copying common into the clipboard."));
        connect(copyAct, &QAction::triggered, this, &CommonListView::copy);
        connect(m_model, &TableModelCommon::filterChanged, [copyAct](bool value){copyAct->setEnabled(!value);});
        toolBar->addAction(copyAct);

        // Move item up and down
        QIcon moveUPIcon(":/Images/MoveUP.svg");
        QAction* moveUPAct = new QAction(moveUPIcon, tr("Move Up"), this);
        moveUPAct->setToolTip(tr("Move the selected items up by one row."));
        connect(moveUPAct, &QAction::triggered, this, &CommonListView::moveItemUp);
        connect(m_model, &TableModelCommon::sortingChanged, [moveUPAct, this](bool value){this->enableAction(moveUPAct);});
        connect(m_model, &TableModelCommon::filterChanged, [moveUPAct, this](bool value){this->enableAction(moveUPAct);});
        toolBar->addAction(moveUPAct);

        QIcon moveDownIcon(":/Images/MoveDown.svg");
        QAction* moveDownAct = new QAction(moveDownIcon, tr("Move Down"), this);
        moveDownAct->setToolTip(tr("Move the selected items down by one row."));
        connect(moveDownAct, &QAction::triggered, this, &CommonListView::moveItemDown);
        connect(m_model, &TableModelCommon::sortingChanged, [moveDownAct, this](bool value){this->enableAction(moveDownAct);});
        connect(m_model, &TableModelCommon::filterChanged, [moveDownAct, this](bool value){this->enableAction(moveDownAct);});
        toolBar->addAction(moveDownAct);

        QIcon moveToIcon(":/Images/MoveTo.svg");
        QAction* moveToAct = new QAction(moveToIcon, tr("Move To"), this);
        moveToAct->setToolTip(tr("Move the selected items to."));
        connect(moveToAct, &QAction::triggered, this, &CommonListView::moveItemTo);
        connect(m_model, &TableModelCommon::sortingChanged, [moveToAct, this](bool value){this->enableAction(moveToAct);});
        connect(m_model, &TableModelCommon::filterChanged, [moveToAct, this](bool value){this->enableAction(moveToAct);});
        toolBar->addAction(moveToAct);

        QIcon updateIcon(":/Images/Update.svg");
        QAction* updateAct = new QAction(updateIcon, tr("Synchronize SQL data with view."), this);
        updateAct->setToolTip(tr("Query all the rows from the list and update the entire view.\n"
                                 "Use it to check if there is no error between the data in the view and the SQL data."));
        connect(updateAct, &QAction::triggered, m_model, &TableModelCommon::updateQuery);
        toolBar->addAction(updateAct);

        // Make the two action for the manipulation of the urls inside a menu controlled by a QToolButton.
        QMenu* urlMenu = new QMenu(tr("Url menu"), this);
        QAction* setUrlAct = new QAction(tr("Set Url"), this);
        setUrlAct->setToolTip(tr("Set the url to the selected movie."));
        connect(setUrlAct, &QAction::triggered, this, &CommonListView::setUrl);
        urlMenu->addAction(setUrlAct);

        QAction* openUrlAct = new QAction(tr("Open Url"), this);
        openUrlAct->setToolTip(tr("Open the url of the selected movie."));
        connect(openUrlAct, &QAction::triggered, this, &CommonListView::openUrl);
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
        connect(filterAct, &QAction::triggered, this, &CommonListView::filter);
        toolBar->addAction(filterAct);

        vLayout->setMenuBar(toolBar);
    }
}

void CommonListView::deletingItems()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->deleteRows(indexList);
    }
}

QVariant CommonListView::listData() const
{
    // Return the data inside the model.
    if (m_model)
    {
        // Retrive the data of the model
        QVariant variant = m_model->retrieveData();
        // If the list is a common list.
        if (variant.canConvert<Common::SaveDataTable>() && m_view)
        {
            // Retrieve the size of the column.
            Common::SaveDataTable data = qvariant_cast<Common::SaveDataTable>(variant);
            data.viewColumnsSize.name = m_view->columnWidth(Common::NAME);
            data.viewColumnsSize.series = m_view->columnWidth(Common::SERIES);
            data.viewColumnsSize.categories = m_view->columnWidth(Common::CATEGORIES);
            data.viewColumnsSize.authors = m_view->columnWidth(Common::AUTHORS);
            data.viewColumnsSize.sensitiveContent = m_view->columnWidth(Common::SENSITIVE_CONTENT);
            data.viewColumnsSize.rate = m_view->columnWidth(Common::RATE);
            return QVariant::fromValue(data);
        }
    }

    return QVariant();
}

ListType CommonListView::listType() const
{
    if (m_model)
        return m_model->listType();
    else
        return ListType::UNKNOWN;
}

void CommonListView::setColumnsSizeAndSortingOrder(const QVariant& variant)
{
    // Set the size of the column inside the table view.
    if (m_type == ListType::COMMONLIST)
    {
        Common::SaveDataTable data = qvariant_cast<Common::SaveDataTable>(variant);
        m_view->setColumnWidth(Common::NAME, data.viewColumnsSize.name);
        m_view->setColumnWidth(Common::SERIES, data.viewColumnsSize.series);
        m_view->setColumnWidth(Common::CATEGORIES, data.viewColumnsSize.categories);
        m_view->setColumnWidth(Common::AUTHORS, data.viewColumnsSize.authors);
        m_view->setColumnWidth(Common::SENSITIVE_CONTENT, data.viewColumnsSize.sensitiveContent);
        m_view->setColumnWidth(Common::RATE, data.viewColumnsSize.rate);

        m_view->horizontalHeader()->setSortIndicator(data.columnSort, (data.sortOrder == 0 ? Qt::AscendingOrder : Qt::DescendingOrder));
    }
}

ViewType CommonListView::viewType() const
{
    return ViewType::COMMON;
}

void CommonListView::setUrl()
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
            tr("No item selected."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void CommonListView::openUrl()
{
    // Open the url of the selected item into the default web browser.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        QString url = m_model->url(indexList.at(0));
        if (!url.isEmpty())
            QDesktopServices::openUrl(QUrl::fromUserInput(url));
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

void CommonListView::moveItemUp()
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

void CommonListView::moveItemDown()
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

void CommonListView::moveItemTo()
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

void CommonListView::filter()
{
    // Opening the filter dialog
    FilterDialog filterDialog(m_model, m_model->utilityInterface(), m_utilityTable, m_db, this);
    filterDialog.exec();
}

void CommonListView::enableAction(QAction* action) const
{
    if (m_model->isSortingEnabled() || m_model->isFilterEnabled())
        action->setEnabled(false);
    else
        action->setEnabled(true);
}

void CommonListView::copy()
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
            tr("No common selected!"),
            QMessageBox::Ok,
            QMessageBox::Ok);
}