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
#include <QSqlError>
#include <iostream>

TableModel::TableModel(const QString& tableName, GameSorting::ListType type, QSqlDatabase& db, QObject* parent) :
    QAbstractTableModel(parent),
    m_db(db),
    m_query(m_db),
    m_tableName(tableName),
    m_listType(type),
    m_gameListData(nullptr),
    m_isTableCreated(false),
    m_isTableChanged(false),
    m_listCount(0)
{
    if (m_listType == GameSorting::GAMELIST)
        m_gameListData = new QList<GameItem>();

    createTable();
}

TableModel::~TableModel()
{
    if (m_gameListData)
        delete m_gameListData;
}

void TableModel::createTable()
{
    // Create the SQL table with all the columns.
    if (m_listType == GameSorting::GAMELIST)
        gameCreateTable();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    // Returning the number of columns in the table.
    // If the table is not created, returning 0.
    if (m_isTableCreated)
    {
        if (m_listType == GameSorting::GAMELIST)
            return gameColumnCount();
    }
    
    return 0;
}

int TableModel::rowCount(const QModelIndex& parent) const
{
    // Returning the number of items in the table.
    if (m_isTableCreated)
    {
        if (m_listType == GameSorting::GAMELIST)
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
    if (m_isTableCreated && role == Qt::DisplayRole)
    {
        if (m_listType == GameSorting::GAMELIST)
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
        if (m_listType == GameSorting::GAMELIST)
            gameUpdateQuery();
    }
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Returning the name of the headers.
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (m_listType == GameSorting::GAMELIST)
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
        if (m_listType == GameSorting::GAMELIST)
            return gameSetData(index, value, role);
    }

    return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (m_isTableCreated)
    {
        if (m_listType == GameSorting::GAMELIST)
            return gameInsertRows(row, count, parent);
    }

    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // Removing rows from the table.
    if (m_isTableCreated)
    {
        if (m_listType == GameSorting::GAMELIST)
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