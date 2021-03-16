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

#define TABLE_COLUMN_COUNT 7

TableModel::TableModel(const QString& tableName, QSqlDatabase& db, QObject* parent) :
    QAbstractTableModel(parent),
    m_db(db),
    m_query(m_db),
    m_tableName(tableName),
    m_isTableCreated(false),
    m_isTableChanged(false),
    m_listCount(0)
{
    createTable();
}

void TableModel::createTable()
{
    // Create the SQL table with all the columns.
    if (m_db.isOpen())
    {
        QString statement = QString(
            "CREATE TABLE %1 ( "
            "   GameID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "   Name TEXT DEFAULT \"New Game\", "
            "   Categories INTEGER, "
            "   Developpers INTEGER, "
            "   Publishers INTEGER, "
            "   Platform INTEGER, "
            "   Services INTEGER, "
            "   Url TEXT, "
            "   Rate INTEGER);")
                .arg(m_tableName);

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

        if (m_query.exec(statement))
            m_isTableCreated = true;
        else
            std::cerr << "Failed to create table " << m_tableName.toLocal8Bit().constData() << "\n\t"
                << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
    }
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    // Returning the number of columns in the table.
    // If the table is not created, returning 0.
    if (m_isTableCreated)
        return TABLE_COLUMN_COUNT;
    else
        return 0;
}

int TableModel::rowCount(const QModelIndex& parent) const
{
    // Returning the number of items in the table.
    return m_listCount;
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
        if (index.column() >= 0 && index.column() < TABLE_COLUMN_COUNT &&
            index.row() >= 0 && index.row() < m_listCount)
        {
            // Returning to the tableView the data from the list.
            switch (index.column())
            {
            case 0:
                return m_listData.at(index.row()).name;
            case 1:
                return m_listData.at(index.row()).categories;
            case 2:
                return m_listData.at(index.row()).developpers;
            case 3:
                return m_listData.at(index.row()).publishers;
            case 4:
                return m_listData.at(index.row()).platform;
            case 5:
                return m_listData.at(index.row()).services;
            case 6:
                return m_listData.at(index.row()).rate;
            }
        }
    }

    return QVariant();
}

void TableModel::updateQuery()
{
    // Retrieve the entrire data of the table and 
    // and put it into the view.
    if (m_isTableCreated)
    {
        if (m_listCount > 0)
        {
            beginRemoveRows(QModelIndex(), 0, m_listData.size()-1);
            endRemoveRows();
        }

        // Getting the new data from the table.
        m_query.clear();

        QString statement = QString(
            "SELECT "
            "   GameID, "
            "   Name, "
            "   Categories, "
            "   Developpers, "
            "   Publishers, "
            "   Platform, "
            "   Services, "
            "   Url, "
            "   Rate "
            "FROM "
            "   %1;")
                .arg(m_tableName);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

        if(m_query.exec(statement))
        {
            // Putting the queried data into a QList.
            m_listData.clear();
            
            while (m_query.next())
            {
                GameItem game = {};
                game.gameID = m_query.value(0).toInt();
                game.name = m_query.value(1).toString();
                game.categories = m_query.value(2).toInt();
                game.developpers = m_query.value(3).toInt();
                game.publishers = m_query.value(4).toInt();
                game.platform = m_query.value(5).toInt();
                game.services = m_query.value(6).toInt();
                game.url = m_query.value(7).toString();
                game.rate = m_query.value(8).toInt();
                m_listData.append(game);
            }

            if (m_listData.size() > 0)
            {
                m_listCount = m_listData.size();
                beginInsertRows(QModelIndex(), 0, m_listData.size()-1);
                endInsertRows();
                emit dataChanged(index(0, 0), index(0, 8), {Qt::EditRole});
            }
            m_query.clear();
        }
#ifndef NDEBUG
        else
            std::cerr << "Failed to update cell values of table " << m_tableName.toLocal8Bit().constData() << "\n\t"
                << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
#endif
    }
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Returning the name of the headers.
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return "Name";
        case 1:
            return "Categories";
        case 2:
            return "Developpers";
        case 3:
            return "Publishers";
        case 4:
            return "Platform";
        case 5:
            return "Services";
        case 6:
            return "Rate";
        }
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // Setting field data.
    // Each column is different, so we working on each column independently.
    if (role == Qt::EditRole && 
        index.column() >= 0 && index.column() <= 6 &&
        index.row() >= 0 && index.row() < m_listData.size())
    {
        switch (index.column())
        {
        case 0:
        {
            if (value.canConvert<QString>())
            {
                QString gameName = value.toString();
                m_listData[index.row()].name = gameName;

                QSqlQuery query(m_db);
                bool result = updateField<const QString&>(query, "Name", index.row(), gameName);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else
                return false;
        }break;

        case 1:
        {
            if (value.canConvert<int>())
            {
                int categories = value.toInt();
                m_listData[index.row()].categories = categories;

                QSqlQuery query(m_db);
                bool result = updateField<int>(query, "Categories", index.row(), categories);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else
                return false;
        } break;

        case 2:
        {
            if (value.canConvert<int>())
            {
                int developpers = value.toInt();
                m_listData[index.row()].developpers = developpers;

                QSqlQuery query(m_db);
                bool result = updateField<int>(query, "Developpers", index.row(), developpers);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else
                return false;
        } break;

        case 3:
        {
            if (value.canConvert<int>())
            {
                int publishers = value.toInt();
                m_listData[index.row()].publishers = publishers;

                QSqlQuery query(m_db);
                bool result = updateField<int>(query, "Publishers", index.row(), publishers);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else
                return false;
        } break;

        case 4:
        {
            if (value.canConvert<int>())
            {
                int platform = value.toInt();
                m_listData[index.row()].platform = platform;

                QSqlQuery query(m_db);
                bool result = updateField<int>(query, "Platform", index.row(), platform);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else 
                return false;
        } break;

        case 5:
        {
            if (value.canConvert<int>())
            {
                int services = value.toInt();
                m_listData[index.row()].services = services;

                QSqlQuery query(m_db);
                bool result = updateField<int>(query, "Services", index.row(), services);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;            }
            else
                return false;
        } break;

        case 6:
        {
            if (value.canConvert<int>())
            {
                int rate = value.toInt();
                m_listData[index.row()].rate = rate;

                QSqlQuery query(m_db);
                bool result = updateField<int>(query, "Rate", index.row(), rate);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else
                return false;
        } break;
        }
    }

    return true;
}

template<typename T>
bool TableModel::updateField(QSqlQuery& query, const QString& columnName, int rowNB, T value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE %1 "
        "SET "
        "   %2 = \"%3\" "
        "WHERE "
        "   GameID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_listData.at(rowNB).gameID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

    if (!query.exec(statement))
    {
        std::cout << QString("Failed to update field %1 of GameID: %2.\n\t%3")
            .arg(columnName)
            .arg(m_listData.at(rowNB).gameID)
            .arg(query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
        return false;
    }

    return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting new rows into the table.
    if (row >= 0 && row <= m_listCount &&
        count > 0)
    {
        // Executing the sql statement for inserting new rows.
        QSqlQuery query(m_db);
        QString statement = QString(
            "INSERT INTO %1 ( "
            "   Name, "
            "   Categories, "
            "   Developpers, "
            "   Publishers, "
            "   Platform, "
            "   Services, "
            "   Url, "
            "   Rate ) "
            "VALUES ")
                .arg(m_tableName);

        for (int i = 0; i < count; i++)
        {
            statement += 
                "   (\"New Game\", NULL, NULL, NULL, NULL, NULL, NULL, NULL),";
        }
        statement[statement.size() - 1] = ';';

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

        // Then, querying the new inserted data and add it to the m_listData
        if (query.exec(statement))
        {
            query.clear();
            statement = QString(
            "SELECT "
            "   GameID, "
            "   Name, "
            "   Categories, "
            "   Developpers, "
            "   Publishers, "
            "   Platform, "
            "   Services, "
            "   Url, "
            "   Rate "
            "FROM "
            "   %1 "
            "ORDER BY "
            "   GameID DESC "
            "LIMIT "
            "   %2;")
                .arg(m_tableName)
                .arg(count);

#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif
            
            if (query.exec(statement))
            {
                if (count == 1)
                    beginInsertRows(parent, m_listCount, m_listCount);
                else
                    beginInsertRows(parent, m_listCount, m_listCount + (count - 1));

                QList<GameItem> gameList;
                while(query.next())
                {
                    GameItem game = {};
                    game.gameID = query.value(0).toInt();
                    game.name = query.value(1).toString();
                    game.categories = query.value(2).toInt();
                    game.developpers = query.value(3).toInt();
                    game.publishers = query.value(4).toInt();
                    game.platform = query.value(5).toInt();
                    game.services = query.value(6).toInt();
                    game.url = query.value(7).toString();
                    game.rate = query.value(8).toInt();
                    m_listData.prepend(game);
                }
                m_listData.append(gameList.cbegin(), gameList.cend());
                m_listCount = m_listData.size();

                endInsertRows();
            }
            else
                updateQuery();
        }
        else
        {
#ifndef NDEBUG
            std::cerr << QString("Failed to insert row of table %1\n\t%2")
                .arg(m_tableName)
                .arg(query.lastError().text()).toLocal8Bit().constData() << std::endl;
#endif
            
            return false;
        }
    }

    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent)
{
    // Removing rows from the table.
    if (row >= 0 && row < m_listCount &&
        row + (count - 1) < m_listCount)
    {
        // Defining the SQL statement.
        QSqlQuery query(m_db);
        QString statement  = QString(
            "DELETE FROM %1 "
            "WHERE GameID ")
            .arg(m_tableName);
        
        if (count == 1)
            statement += QString("= %1;")
                .arg(m_listData.at(row).gameID);
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ", ";
                statement += QString::number(m_listData.at(i).gameID);
            }
            statement += ");";
        }

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

        // Execute the SQL statement.
        // If the statement success, then
        // removing the data from the list
        // and updating the view.
        if (query.exec(statement))
        {
            if (count == 1)
                beginRemoveRows(QModelIndex(), row, row);
            else
                beginRemoveRows(QModelIndex(), row, row + (count - 1));
            
            m_listData.remove(row, count);

            endRemoveRows();
        }
        else
        {
#ifndef NDEBUG
            std::cerr << "Failed to remove rows from the table " << m_tableName.toLocal8Bit().constData() << "\n\t" <<
                query.lastError().text().toLocal8Bit().constData() << std::endl;
#endif
            return false;
        }
    }

    return true;
}