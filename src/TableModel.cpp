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

#include "TableModel.h"
#include "Common.h"
#include <QSqlError>
#include <iostream>
#include <algorithm>

TableModel::TableModel(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    QAbstractTableModel(parent),
    m_db(db),
    m_utilityTable(utilityTable),
    m_query(m_db),
    m_listType(type),
    m_gameListData(nullptr),
    m_isTableCreated(false),
    m_isTableChanged(false),
    m_listCount(0)
{
    if (m_listType == ListType::GAMELIST)
        m_gameListData = new QList<GameItem>();

    m_tableName = checkingIfNameFree(replaceSpaceByUnderscore(replaceMultipleSpaceByOne(removeFirtAndLastSpaces(tableName))));

    createTable();
}

TableModel::~TableModel()
{
    deleteSqlTable();

    if (m_gameListData)
        delete m_gameListData;
}

void TableModel::createTable()
{
    // Create the SQL table with all the columns.
    if (m_listType == ListType::GAMELIST)
        gameCreateTable();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    // Returning the number of columns in the table.
    // If the table is not created, returning 0.
    if (m_isTableCreated)
    {
        if (m_listType == ListType::GAMELIST)
            return gameColumnCount();
    }
    
    return 0;
}

int TableModel::rowCount(const QModelIndex& parent) const
{
    // Returning the number of items in the table.
    if (m_isTableCreated)
    {
        if (m_listType == ListType::GAMELIST)
            return gameRowCount();
    }

    return 0;
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    // Returning the table data into the view.
    if (m_isTableCreated && (role == Qt::DisplayRole || role == Qt::EditRole))
    {
        if (m_listType == ListType::GAMELIST)
            return gameData(index, role);
    }

    return QVariant();
}

void TableModel::updateQuery()
{
    // Retrieve the entrire data of the table and 
    // and put it into the view.
    if (m_isTableCreated)
    {
        if (m_listType == ListType::GAMELIST)
            gameUpdateQuery();
    }
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Returning the name of the headers.
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (m_listType == ListType::GAMELIST)
            return gameHeaderData(section, orientation, role);
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // Setting field data.
    // Each column is different, so we working on each column independently.
    if (m_isTableCreated)
    {
        if (m_listType == ListType::GAMELIST)
            return gameSetData(index, value, role);
    }

    return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (m_isTableCreated)
    {
        if (m_listType == ListType::GAMELIST)
            return gameInsertRows(row, count, parent);
    }

    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // Removing rows from the table.
    if (m_isTableCreated)
    {
        if (m_listType == ListType::GAMELIST)
            return gameRemoveRows(row, count, parent);
    }

    return true;
}

bool TableModel::appendRows(int count)
{
    if (count > 0)
        return insertRows(m_listCount, count);

    return false;
}

int TableModel::size() const
{
    if (m_isTableCreated)
        return m_listCount;
    
    return 0;
}

QString TableModel::checkingIfNameFree(const QString& name, int n) const
{
    // String using for the comparison with the available tables name.
    QString cName;
    if (n < 0)
        cName = name;
    else
        cName = QString("%1_%2").arg(name).arg(n);

    // Then comparing the name with all the available name into the database.
    QStringList tablesName = m_db.tables();
    for (QStringList::const_iterator it = tablesName.cbegin();
         it != tablesName.cend();
         it++)
    {
        // If the name is already exist, testing the name with a number attached to it 
        // using recursive function.
        if ((*it).compare(cName, Qt::CaseInsensitive) == 0)
        {
            if (n < 0)
                return checkingIfNameFree(name, 1);
            else
                return checkingIfNameFree(name, n+1);
        }
    }

    // Otherwise, the name is free, we can use it.
    return cName;
}

QString TableModel::tableName() const
{
    return replaceUnderscoreBySpace(m_tableName);
}

QString TableModel::rowTableName() const
{
    return m_tableName;
}

void TableModel::deleteSqlTable()
{
    // Removing the SQL table when this object is destoyed.
    m_query.clear();

    QString statement = QString(
        "DROP TABLE IF EXISTS \"%1\";"
        ).arg(m_tableName);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
        std::cerr << "Failed to delete the table \"" 
            << m_tableName.toLocal8Bit().constData()
            << "\"\n\t"
            << m_query.lastError().text().toLocal8Bit().constData()
            << std::endl;
}

void TableModel::deleteRows(const QModelIndexList& indexList)
{
    // Used to delete selected rows in the view.
    QModelIndexList indexListCopy(indexList);
    std::sort(indexListCopy.begin(), indexListCopy.end(),
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() > index2.row();
        });

    for (QModelIndexList::const_iterator it = indexListCopy.cbegin();
        it != indexListCopy.cend();
        it++)
        removeRow((*it).row(), QModelIndex());
}

ListType TableModel::listType() const
{
    return m_listType;
}