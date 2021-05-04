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

#include "UtilityListModel.h"
#include "SqlUtilityTable.h"
#include <QSqlError>

#include <iostream>

UtilityListModel::UtilityListModel(UtilityTableName tableName, QSqlDatabase& db, QObject* parent) :
    QAbstractListModel(parent),
    m_tableName(tableName),
    m_db(db),
    m_query(m_db)
{
    queryTable();
}

UtilityListModel::~UtilityListModel()
{}

int UtilityListModel::rowCount(const QModelIndex& parent) const
{
    return m_data.size();
}

QVariant UtilityListModel::data(const QModelIndex& index, int role) const
{
    // Return the data of the specific index.
    if (index.isValid() && index.row() < m_data.size())
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return m_data.at(index.row()).name;
        else if (role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
    }
    return QVariant();
}

bool UtilityListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // Set the data at the index.
    if (index.isValid() && index.row() < m_data.size() && !value.isNull())
    {
        if (value.canConvert<QString>())
        {
            // Insert data into the SQL Table
            QString statement = QString(
                "UPDATE \"%1\"\n"
                "SET\n"
                "   Name = \"%2\"\n"
                "WHERE\n"
                "   \"%1ID\" = %3")
                .arg(SqlUtilityTable::tableName(m_tableName))
                .arg(value.toString())
                .arg(m_data.at(index.row()).utilityID);
            
#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            m_query.clear();
            if (m_query.exec(statement))
            {
                m_data[index.row()].name = value.toString();
                dataChanged(index, index, {Qt::EditRole});
                return true;
            }
            else
            {
#ifndef NDEBUG
                std::cerr << QString("Failed to update row %1 of %2\n\t%3")
                    .arg(index.row())
                    .arg(SqlUtilityTable::tableName(m_tableName))
                    .arg(m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
#endif
                return false;
            }
        }
        else
            return false;
    }
    return true;
}

bool UtilityListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting rows into the model.
    if (row > -1 && row <= rowCount() && count > 0)
    {
        QString statement = QString(
            "INSERT INTO \"%1\" (Name)\n"
            "VALUES")
            .arg(SqlUtilityTable::tableName(m_tableName));
        
        for (int i = 0; i < count; i++)
            statement += QString("\n\t(\"New %1\"),").arg(SqlUtilityTable::tableName(m_tableName));
        statement[statement.size()-1] = ';';

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        m_query.clear();
        if (m_query.exec(statement))
        {
            // query back the new inserted row.
            statement = QString(
                "SELECT\n"
                "   \"%1ID\",\n"
                "   Name\n"
                "FROM\n"
                "   \"%1\"\n"
                "ORDER BY\n"
                "   \"%1ID\" DESC\n"
                "LIMIT\n"
                "   %2;")
                .arg(SqlUtilityTable::tableName(m_tableName))
                .arg(count);
            
#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            m_query.clear();
            if (m_query.exec(statement))
            {
                QList<ItemUtilityData> newData;
                while (m_query.next())
                {
                    ItemUtilityData data = {};
                    data.utilityID = m_query.value(0).toLongLong();
                    data.name = m_query.value(1).toString();
                    newData.prepend(data);
                }

                if (newData.size() > 0)
                {
                    beginInsertRows(QModelIndex(), 0, newData.size()-1);
                    m_data.append(newData);
                    endInsertRows();
                }
            }
            else
            {
#ifndef NDEBUG
                std::cerr << QString("Failed to query inserted rows of %1.\n\t%2")
                    .arg(SqlUtilityTable::tableName(m_tableName))
                    .arg(m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
#endif
                return false;
            }
        }
        else
        {
#ifndef NDEBUG
            std::cerr << QString("Failed to insert rows into %1.\n\t%2")
                .arg(SqlUtilityTable::tableName(m_tableName))
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
#endif
            return false;
        }
    }

    return true;
}

bool UtilityListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // Remove rows from the model
    if (row > -1 && row < rowCount() && count > 0)
    {
        QString statement = QString(
            "DELETE FROM \"%1\"\n"
            "WHERE\n"
            "   \"%1ID\" ")
            .arg(SqlUtilityTable::tableName(m_tableName));

        if (count == 1)
            statement += QString("= %1;").arg(m_data.at(row).utilityID);
        else
        {
            statement += "(";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ",";
                statement += QString::number(m_data.at(row+i).utilityID);
            }
            statement += ");";
        }
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        m_query.clear();
        if (m_query.exec(statement))
        {
            beginRemoveRows(QModelIndex(), row, row+count-1);
            m_data.remove(row, count);
            endRemoveRows();
        }
        else
        {
            std::cerr << QString("Failed to remove rows from %1\n\t%2")
                .arg(SqlUtilityTable::tableName(m_tableName))
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            return false;
        }
    }
    
    return true;
}

QVariant UtilityListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // There is only one column, returning the name of that column.
    if (section == 0 && orientation == Qt::Horizontal)
        return "Name";
    return QVariant();
}

Qt::ItemFlags UtilityListModel::flags(const QModelIndex& index) const
{
    // Set the rows editable.
    return Qt::ItemIsEditable | QAbstractListModel::flags(index);
}

void UtilityListModel::queryTable()
{
    // Query the whole table and apply the result in the model.
    // Thirst, removing everything in the view.
    if (rowCount() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, rowCount()-1);
        m_data.clear();
        endRemoveRows();
    }

    // Query the table;
    QString statement = QString(
        "SELECT\n"
        "   \"%1ID\",\n"
        "   Name\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   \"%1ID\";")
        .arg(SqlUtilityTable::tableName(m_tableName));

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif
    
    m_query.clear();
    if (m_query.exec(statement))
    {
        // Apply the result in the view.
        while (m_query.next())
        {
            ItemUtilityData data = {};
            data.utilityID = m_query.value(0).toLongLong();
            data.name = m_query.value(1).toString();
            m_data.append(data);
        }

        // Tell the view to show the new rows.
        if (m_data.size() > 0)
        {
            beginInsertRows(QModelIndex(), 0, rowCount()-1);
            endInsertRows();
        }
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query the utility table %1.\n\t%2")
            .arg(SqlUtilityTable::tableName(m_tableName))
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
}

void UtilityListModel::appendRow()
{
    insertRow(rowCount());
}

void UtilityListModel::deleteIndexs(const QModelIndexList& indexList)
{
    // Removing the rows from the indexs (indexList).
    // First, create a copy of the indexs list (indexList).
    QModelIndexList cpyIndexList(indexList);
    std::sort(cpyIndexList.begin(), cpyIndexList.end(),
        [] (const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() > index2.row();
        });

    // Then, delete the rows.
    for (int i = 0; i < cpyIndexList.size(); i++)
        removeRow(cpyIndexList.at(i).row());
}