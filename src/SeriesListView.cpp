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

#include "SeriesListView.h"
#include "TableModelSeries.h"
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

SeriesListView::SeriesListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(type),
    m_view(new QTableView(this)),
    m_model(new TableModelSeries(tableName, m_db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    setupWidget();
    setupView();
    connect(m_model, &TableModelSeries::listEdited, this, &SeriesListView::listEdited);
}

SeriesListView::SeriesListView(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent) :
    AbstractListView(parent),
    m_db(db),
    m_type(ListType::UNKNOWN),
    m_view(new QTableView(this)),
    m_model(new TableModelSeries(data, db, utilityTable, this)),
    m_utilityTable(utilityTable)
{
    m_type = m_model->listType();
    setupWidget();
    setupView();
    setColumnsSizeAndSortingOrder(data);
    connect(m_model, &TableModelSeries::listEdited, this, &SeriesListView::listEdited);
}

SeriesListView::~SeriesListView()
{
    if (m_view)
    {
        QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
        if (itemDelegate)
            delete itemDelegate;
    }
}

void SeriesListView::setupWidget()
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

void SeriesListView::setupView()
{
    m_view->setModel(m_model);
}

void SeriesListView::addingItem()
{
    if (m_model)
        m_model->appendRows();
}

QString SeriesListView::tableName() const
{
    if (m_model)
        return m_model->tableName();
    else
        return QString();
}

void SeriesListView::setTableName(const QString& tableName)
{
    if (m_model)
        m_model->setTableName(tableName);
}

void SeriesListView::createMenu(QVBoxLayout* vLayout)
{
    if (m_type == ListType::SERIESLIST)
    {
        QToolBar* toolBar = new QToolBar(tr("Common Menu Bar"), this);
        toolBar->setMovable(false);

        QIcon addIcon(":/Images/Add.svg");
        QAction* addAct = new QAction(addIcon, tr("Add New Item"), this);
        addAct->setToolTip(tr("Adding a new item into the current list."));
        connect(addAct, &QAction::triggered, this, &SeriesListView::addingItem);
        connect(m_model, &TableModelSeries::filterChanged, [addAct](bool value) {addAct->setEnabled(!value);});
        toolBar->addAction(addAct);

        QIcon delIcon(":/Images/Del.svg");
        QAction* delAct = new QAction(delIcon, tr("Delete Items"), this);
        delAct->setToolTip(tr("Deleting selected items in the current list."));
        connect(delAct, &QAction::triggered, this, &SeriesListView::deletingItems);
        connect(m_model, &TableModelSeries::filterChanged, [delAct](bool value){delAct->setEnabled(!value);}),
        toolBar->addAction(delAct);

        // Move item up and down
        QIcon moveUPIcon(":/Images/MoveUP.svg");
        QAction* moveUPAct = new QAction(moveUPIcon, tr("Move Up"), this);
        moveUPAct->setToolTip(tr("Move the selected items up by one row."));
        connect(moveUPAct, &QAction::triggered, this, &SeriesListView::moveItemUp);
        connect(m_model, &TableModelSeries::sortingChanged, [moveUPAct, this](bool value){this->enableAction(moveUPAct);});
        connect(m_model, &TableModelSeries::filterChanged, [moveUPAct, this](bool value){this->enableAction(moveUPAct);});
        toolBar->addAction(moveUPAct);

        QIcon moveDownIcon(":/Images/MoveDown.svg");
        QAction* moveDownAct = new QAction(moveDownIcon, tr("Move Down"), this);
        moveDownAct->setToolTip(tr("Move the selected items down by one row."));
        connect(moveDownAct, &QAction::triggered, this, &SeriesListView::moveItemDown);
        connect(m_model, &TableModelSeries::sortingChanged, [moveDownAct, this](bool value){this->enableAction(moveDownAct);});
        connect(m_model, &TableModelSeries::filterChanged, [moveDownAct, this](bool value){this->enableAction(moveDownAct);});
        toolBar->addAction(moveDownAct);

        QIcon moveToIcon(":/Images/MoveTo.svg");
        QAction* moveToAct = new QAction(moveToIcon, tr("Move To"), this);
        moveToAct->setToolTip(tr("Move the selected items to."));
        connect(moveToAct, &QAction::triggered, this, &SeriesListView::moveItemTo);
        connect(m_model, &TableModelSeries::sortingChanged, [moveToAct, this](bool value){this->enableAction(moveToAct);});
        connect(m_model, &TableModelSeries::filterChanged, [moveToAct, this](bool value){this->enableAction(moveToAct);});
        toolBar->addAction(moveToAct);

        QIcon updateIcon(":/Images/Update.svg");
        QAction* updateAct = new QAction(updateIcon, tr("Synchronize SQL data with view."), this);
        updateAct->setToolTip(tr("Query all the rows from the list and update the entire view.\n"
                                 "Use it to check if there is no error between the data in the view and the SQL data."));
        connect(updateAct, &QAction::triggered, m_model, &TableModelSeries::updateQuery);
        toolBar->addAction(updateAct);

        // Make the two action for the manipulation of the urls inside a menu controlled by a QToolButton.
        QMenu* urlMenu = new QMenu(tr("Url menu"), this);
        QAction* setUrlAct = new QAction(tr("Set Url"), this);
        setUrlAct->setToolTip(tr("Set the url to the selected movie."));
        connect(setUrlAct, &QAction::triggered, this, &SeriesListView::setUrl);
        urlMenu->addAction(setUrlAct);

        QAction* openUrlAct = new QAction(tr("Open Url"), this);
        openUrlAct->setToolTip(tr("Open the url of the selected movie."));
        connect(openUrlAct, &QAction::triggered, this, &SeriesListView::openUrl);
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
        connect(filterAct, &QAction::triggered, this, &SeriesListView::filter);
        toolBar->addAction(filterAct);

        vLayout->setMenuBar(toolBar);
    }
}

void SeriesListView::deletingItems()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList = selectionModel->selectedRows(0);
        m_model->deleteRows(indexList);
    }
}

QVariant SeriesListView::listData() const
{
    // Return the data inside the model.
    if (m_model)
    {
        // Retrieve the data of the model.
        QVariant variant = m_model->retrieveData();
        // If the list is a series list.
        if (variant.canConvert<Series::SaveDataTable>() && m_view)
        {
            // Retrieve the size of the column.
            Series::SaveDataTable data = qvariant_cast<Series::SaveDataTable>(variant);
            data.viewColumnsSize.name = m_view->columnWidth(Series::NAME);
            data.viewColumnsSize.episode = m_view->columnWidth(Series::EPISODE);
            data.viewColumnsSize.season = m_view->columnWidth(Series::SEASON);
            data.viewColumnsSize.categories = m_view->columnWidth(Series::CATEGORIES);
            data.viewColumnsSize.directors = m_view->columnWidth(Series::DIRECTORS);
            data.viewColumnsSize.actors = m_view->columnWidth(Series::ACTORS);
            data.viewColumnsSize.production = m_view->columnWidth(Series::PRODUCTION);
            data.viewColumnsSize.music = m_view->columnWidth(Series::MUSIC);
            data.viewColumnsSize.services = m_view->columnWidth(Series::SERVICES);
            data.viewColumnsSize.sensitiveContent = m_view->columnWidth(Series::SENSITIVE_CONTENT);
            data.viewColumnsSize.rate = m_view->columnWidth(Series::RATE);
            return QVariant::fromValue(data);
        }
    }

    return QVariant();
}

ListType SeriesListView::listType() const
{
    if (m_model)
        return m_model->listType();
    else
        return ListType::UNKNOWN;
}

void SeriesListView::setColumnsSizeAndSortingOrder(const QVariant& variant)
{
    // Set the size of the column inside the table view.
    if (m_type == ListType::SERIESLIST)
    {
        if (variant.canConvert<Series::SaveDataTable>() && m_view)
        {
            Series::SaveDataTable data = qvariant_cast<Series::SaveDataTable>(variant);
            m_view->setColumnWidth(Series::NAME, data.viewColumnsSize.name);
            m_view->setColumnWidth(Series::EPISODE, data.viewColumnsSize.episode);
            m_view->setColumnWidth(Series::SEASON, data.viewColumnsSize.season);
            m_view->setColumnWidth(Series::CATEGORIES, data.viewColumnsSize.categories);
            m_view->setColumnWidth(Series::DIRECTORS, data.viewColumnsSize.directors);
            m_view->setColumnWidth(Series::ACTORS, data.viewColumnsSize.actors);
            m_view->setColumnWidth(Series::PRODUCTION, data.viewColumnsSize.production);
            m_view->setColumnWidth(Series::MUSIC, data.viewColumnsSize.music);
            m_view->setColumnWidth(Series::SERVICES, data.viewColumnsSize.services);
            m_view->setColumnWidth(Series::SENSITIVE_CONTENT, data.viewColumnsSize.sensitiveContent);
            m_view->setColumnWidth(Series::RATE, data.viewColumnsSize.rate);
            m_view->horizontalHeader()->setSortIndicator(data.columnSort, (data.sortOrder == 0 ? Qt::AscendingOrder : Qt::DescendingOrder));
        }
    }
}

ViewType SeriesListView::viewType() const
{
    return ViewType::SERIES;
}

void SeriesListView::setUrl()
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

void SeriesListView::openUrl()
{
    // Open the url of the selected item into the default web browser.
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    if (selectionModel->hasSelection())
    {
        QModelIndexList indexList= selectionModel->selectedRows(0);
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
            tr("You need to select an item before."),
            QMessageBox::Ok,
            QMessageBox::Ok);
    }
}

void SeriesListView::moveItemUp()
{
    // Get the selection model
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    // Check if the selection model has any selection.
    if (selectionModel->hasSelection())
    {
        // Extract the selection
        QModelIndexList indexList = selectionModel->selectedRows(0);
        // Send the selection to the model
        QItemSelection selectedItems = m_model->moveItemsUp(indexList);
        // Select the moved items.
        selectionModel->clear();
        selectionModel->select(selectedItems, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void SeriesListView::moveItemDown()
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

void SeriesListView::moveItemTo()
{
    // Move the selected items to the positon X.
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

void SeriesListView::filter()
{
    // Opening the filter dialog
    FilterDialog filterDialog(m_model, m_model->utilityInterface(), m_utilityTable, m_db, this);
    filterDialog.exec();
}

void SeriesListView::enableAction(QAction* action) const
{
    if (m_model->isSortingEnabled() || m_model->isFilterEnabled())
        action->setEnabled(false);
    else
        action->setEnabled(true);
}