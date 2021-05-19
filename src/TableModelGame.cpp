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

#include "TableModelGame.h"
#include "TableModel_UtilityInterface.h"
#include <QSqlError>
#include <iostream>
#include <algorithm>

#define GAME_TABLE_COLUMN_COUNT 8

template<typename T>
bool TableModelGame::updateField(const QString& columnName, int rowNB, T value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = %3\n"
        "WHERE\n"
        "   GameID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_data.at(rowNB).gameID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to update field %1 of GameID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).gameID)
            .arg(m_tableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
        return false;
    }
    m_query.clear();
    return true;
}

template<>
bool TableModelGame::updateField(const QString& columnName, int rowNB, const QString& value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = \"%3\"\n"
        "WHERE\n"
        "   GameID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_data.at(rowNB).gameID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to update field %1 of GameID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).gameID)
            .arg(m_tableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
        return false;
    }
    m_query.clear();
    return true;
}

TableModelGame::TableModelGame(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(tableName, db, utilityTable, parent),
    m_interface(nullptr)
{
    createTable();
    m_interface = new TableModel_UtilityInterface(rawTableName(), listType(), m_db);
    connect(m_interface, &TableModel_UtilityInterface::interfaceChanged, this, &TableModelGame::utilityChanged);
    connect(m_interface, &TableModel_UtilityInterface::interfaceChanged, this, &TableModelGame::listEdited);
}

TableModelGame::TableModelGame(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(db, utilityTable, parent),
    m_interface(nullptr)
{
    setItemData(data);
    connect(m_interface, &TableModel_UtilityInterface::interfaceChanged,  this, &TableModelGame::utilityChanged);
    connect(m_interface, &TableModel_UtilityInterface::interfaceChanged, this, &TableModelGame::listEdited);
}

TableModelGame::~TableModelGame()
{
    deleteTable();
    if (m_interface)
        delete m_interface;
}

int TableModelGame::columnCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return GAME_TABLE_COLUMN_COUNT;
    else 
        return 0;
}

int TableModelGame::rowCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return m_data.size();
    else
        return 0;
}

QVariant TableModelGame::data(const QModelIndex& index, int role) const
{
    // Returning the game table data into the view.
    if ((role == Qt::EditRole || role == Qt::DisplayRole) && m_isTableCreated)
    {
        if (index.column() >= 0 && index.column() < columnCount() &&
            index.row() >= 0 && index.row() < rowCount())
        {
            // Returning to the tableView the data from the list.
            switch (index.column())
            {
            case Game::NAME:
                return m_data.at(index.row()).name;
            case Game::CATEGORIES:
                return m_data.at(index.row()).categories;
            case Game::DEVELOPPERS:
                return m_data.at(index.row()).developpers;
            case Game::PUBLISHERS:
                return m_data.at(index.row()).publishers;
            case Game::PLATFORMS:
                return m_data.at(index.row()).platform;
            case Game::SERVICES:
                return m_data.at(index.row()).services;
            case Game::SENSITIVE_CONTENT:
                return m_data.at(index.row()).sensitiveContent;
            case Game::RATE:
                return m_data.at(index.row()).rate;
            }
        }
    }

    return QVariant();
}

bool TableModelGame::setData(const QModelIndex& index, const QVariant& data, int role)
{
    // Setting field data.
    // Each column is different, so we working on each column independently.
    if (role == Qt::EditRole && m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
    {
        switch (index.column())
        {
        case Game::NAME:
        {
            if (data.canConvert<QString>())
            {
                QString gameName = data.toString();
                m_data[index.row()].name = gameName;

                bool result = updateField<const QString&>("Name", index.row(), gameName);
                if (result)
                {
                    emit dataChanged(index, index, {Qt::EditRole});
                    emit listEdited();
                }
                return result;
            }
            else
                return false;
        } break;

        case Game::RATE:
        {
            if (data.canConvert<int>())
            {
                int rate = data.toInt();
                m_data[index.row()].rate = rate;

                bool result = updateField<int>("Rate", index.row(), rate);
                if (result)
                {
                    emit dataChanged(index, index, {Qt::EditRole});
                    emit listEdited();
                }
                return result;
            }
            else
                return false;
        } break;
        }
    }

    return true;
}

bool TableModelGame::insertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting new rows into the table.
    if (row >= 0 && row <= rowCount() &&
        count > 0 && m_isTableCreated)
    {
        // Executing the sql statement for inserting new rows.
        QSqlQuery query(m_db);
        QString statement = QString(
            "INSERT INTO \"%1\" (\n"
            "   GamePos,\n"
            "   Name,\n"
            "   Url,\n"
            "   Rate )\n"
            "VALUES")
                .arg(m_tableName);

        for (int i = 0; i < count; i++)
        {
            statement += 
                "\n   (NULL, \"New Game\", NULL, NULL),";
        }
        statement[statement.size() - 1] = ';';

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        // Then, querying the new inserted data and add it to the m_listData
        if (query.exec(statement))
        {
            query.clear();
            statement = QString(
                "SELECT\n"
                "   GameID,\n"
                "   GamePos,\n"
                "   Name,\n"
                "   Url,\n"
                "   Rate\n"
                "FROM\n"
                "   \"%1\"\n"
                "ORDER BY\n"
                "   GameID DESC\n"
                "LIMIT\n"
                "   %2;")
                    .arg(m_tableName)
                    .arg(count);
                
#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (query.exec(statement))
            {
                if (count == 1)
                    beginInsertRows(QModelIndex(), rowCount(), rowCount());
                else
                    beginInsertRows(QModelIndex(), rowCount(), rowCount() + (count - 1));

                QList<GameItem> gameList;
                while(m_query.next())
                {
                    GameItem game = {};
                    game.gameID = query.value(0).toLongLong();
                    game.gamePos = query.value(1).toLongLong();
                    game.name = query.value(2).toString();
                    game.url = query.value(3).toString();
                    game.rate = query.value(4).toInt();
                    gameList.prepend(game);
                }
                m_data.append(gameList.cbegin(), gameList.cend());

                endInsertRows();
            }
            else
                updateQuery();
            
            // Emit the signal listEdited, this signal is used to tell that the list has been edited.
            emit listEdited();
            m_query.clear();
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

bool TableModelGame::removeRows(int row, int count, const QModelIndex& parent)
{
        // Removing rows from the table.
    if (row >= 0 && row < rowCount() &&
        row + (count - 1) < rowCount() && m_isTableCreated)
    {
        // Defining the SQL statement.
        QSqlQuery query(m_db);
        QString statement  = QString(
            "DELETE FROM \"%1\"\n"
            "WHERE GameID ")
            .arg(m_tableName);
        QList<long long int> itemsID(count);
        
        if (count == 1)
        {
            statement += QString("= %1;")
                .arg(m_data.at(row).gameID);
                itemsID[0] = m_data.at(row).gameID;
        }
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ", ";
                statement += QString::number(m_data.at(row+i).gameID);
                itemsID[i] = m_data.at(row+i).gameID;
            }
            statement += ");";
        }

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
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
            
            m_data.remove(row, count);
            m_interface->rowRemoved(itemsID);

            endRemoveRows();

            emit listEdited();
            m_query.clear();
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

Qt::ItemFlags TableModelGame::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant TableModelGame::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        // Setting the headers' name.
    switch (section)
    {
    case Game::NAME:
        return "Name";
    case Game::CATEGORIES:
        return "Categories";
    case Game::DEVELOPPERS:
        return "Developpers";
    case Game::PUBLISHERS:
        return "Publishers";
    case Game::PLATFORMS:
        return "Platform";
    case Game::SERVICES:
        return "Services";
    case Game::SENSITIVE_CONTENT:
        return "Sensitive Content";
    case Game::RATE:
        return "Rate";
    default:
        return QVariant();
    }
    }

    return QVariant();
}

void TableModelGame::appendRows(int count)
{
    if (count > 0 && m_isTableCreated)
        insertRows(rowCount(), count);
}

void TableModelGame::deleteRows(const QModelIndexList& indexList)
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
    
    if (indexList.size() > 0)
        emit listEdited();
}

int TableModelGame::size() const
{
    return rowCount();
}

ListType TableModelGame::listType() const
{
    return ListType::GAMELIST;
}

long long int TableModelGame::itemID(const QModelIndex& index) const
{
    if (m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
        return m_data.at(index.row()).gameID;
    return -1;
}

void TableModelGame::updateQuery()
{
    // Retrieve the entrire game data of the table and 
    // and put it into the view.
    if (size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, size()-1);
        endRemoveRows();
    }

    // Getting the new data from the table.
    QString statement = QString(
        "SELECT\n"
        "   GameID,\n"
        "   GamePos,\n"
        "   Name,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\";")
            .arg(m_tableName);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if(m_query.exec(statement))
    {
        // Putting the queried data into a QList.
        m_data.clear();
        
        while (m_query.next())
        {
            GameItem game = {};
            game.gameID = m_query.value(0).toLongLong();
            game.gamePos = m_query.value(1).toLongLong();
            game.name = m_query.value(2).toString();
            game.url = m_query.value(3).toString();
            game.rate = m_query.value(4).toInt();
            m_data.append(game);
        }

        if (size() > 0)
        {
            // Quering the util table and set it into the specifics rows.
            queryCategoriesField();
            queryDeveloppersField();
            queryPublishersField();
            queryPlatformField();
            queryServicesField();
            querySensitiveContentField();

            beginInsertRows(QModelIndex(), 0, size()-1);
            endInsertRows();
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << "Failed to update cell values of table " << m_tableName.toLocal8Bit().constData() << "\n\t"
            << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
#endif
}

QVariant TableModelGame::retrieveData() const
{
    // Return the data of the table and the utility interface data.
    Game::SaveDataTable data = {};
    data.tableName = m_tableName;

    // Retrieve game data.
    QSqlQuery query(m_db);

    QString statement = QString(
        "SELECT\n"
        "   GameID,\n"
        "   GamePos,\n"
        "   Name,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   GameID ASC;")
            .arg(m_tableName);
    
    if (!query.exec(statement))
        return QVariant();
    
    while (query.next())
    {
        Game::SaveItem game = {};
        game.gameID = query.value(0).toLongLong();
        game.gamePos = query.value(1).toLongLong();
        game.name = query.value(2).toString();
        game.url = query.value(3).toString();
        game.rate = query.value(4).toInt();
        data.gameList.append(game);
    }

    QVariant utilityInterface = m_interface->data();
    if (!utilityInterface.canConvert<Game::SaveUtilityInterfaceData>())
        return QVariant();
    
    data.interface = qvariant_cast<Game::SaveUtilityInterfaceData>(utilityInterface);

    return QVariant::fromValue(data);
}

bool TableModelGame::setItemData(const QVariant& variant)
{
    // Set the data into the TableModel SQL Tables.
    Game::SaveDataTable data = qvariant_cast<Game::SaveDataTable>(variant);

    // Set the table name and create the SQL tables.
    m_tableName = data.tableName;
    if (m_tableName.isEmpty())
        return false;
    createTable();

    // Set the game list.
    QString statement = QString(
        "INSERT INTO \"%1\" (GameID, GamePos, Name, Url, Rate)\n"
        "VALUES")
        .arg(m_tableName);
    
    for (long long int i = 0; i < data.gameList.size(); i+=10)
    {
        QString strData;
        for (long long int j = i; j < i+10 && j < data.gameList.size(); j++)
        {
            strData +=
                QString("\n\t(%1, %2, \"%3\", \"%4\", %5),")
                    .arg(data.gameList.at(j).gameID)
                    .arg(data.gameList.at(j).gamePos)
                    .arg(data.gameList.at(j).name)
                    .arg(data.gameList.at(j).url)
                    .arg(data.gameList.at(j).rate);
        }
        if (strData.size() > 0)
        {
            strData[strData.size()-1] = ';';

#ifndef NDEBUG
            std::cout << (statement + strData).toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (!m_query.exec(statement + strData))
            {
#ifndef NDEBUG
                std::cerr << QString("Failed to exec statement for setting data into the table %1.\n\t%2")
                    .arg(m_tableName)
                    .arg(m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
#endif

                return false;
            }
            m_query.clear();
        }
    }
    // Set the utility interface.
    if (m_interface)
    {
        delete m_interface;
        m_interface = nullptr;
    }
    m_interface = new TableModel_UtilityInterface(m_tableName, listType(), m_db, QVariant::fromValue(data.interface));
    if (!m_interface->isTableReady())
    {
        m_isTableCreated = false;
        return false;
    }

    // Then, query the whole table.
    updateQuery();

    return true;
}

void TableModelGame::createTable()
{
    // Create the Game SQL table
    QString statement = QString(
        "CREATE TABLE \"%1\" (\n"
        "   GameID INTEGER PRIMARY KEY,\n"
        "   GamePos INTEGER,\n"
        "   Name TEXT,\n"
        "   SensitiveContent INTEGER,\n"
        "   Url TEXT,\n"
        "   Rate INTEGER);")
            .arg(m_tableName);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
        m_isTableCreated = true;
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to create the table %1.\n\t%2")
            .arg(m_tableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
    m_query.clear();
}

void TableModelGame::deleteTable()
{
    // Delete the Sql Utility Interface
    if (!m_isTableCreated)
        return;
    
    QString statement = QString(
        "DROP TABLE IF EXISTS \"%1\";")
            .arg(m_tableName);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to delete table %1.\n\t%2")
            .arg(m_tableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
    }
    m_query.clear();
}

void TableModelGame::utilityChanged(long long int gameID, UtilityTableName tableName)
{
    // This member function is called when the utility interface if changed.
    if (gameID >= 0 && size() > 0 && m_isTableCreated)
    {
        if (tableName == UtilityTableName::CATEGORIES)
            queryCategoriesField(gameID);
        else if (tableName == UtilityTableName::DEVELOPPERS)
            queryDeveloppersField(gameID);
        else if (tableName == UtilityTableName::PUBLISHERS)
            queryPublishersField(gameID);
        else if (tableName == UtilityTableName::PLATFORM)
            queryPlatformField(gameID);
        else if (tableName == UtilityTableName::SERVICES)
            queryServicesField(gameID);
        else if (tableName ==  UtilityTableName::SENSITIVE_CONTENT)
            querySensitiveContentField(gameID);
    }
}

void TableModelGame::queryUtilityField(UtilityTableName tableName)
{
    // Standard interface to query the utility data except the sensitive data.
    if (!m_isTableCreated)
        return;

    QString statement = QString(
        "SELECT\n"
        "   \"%1\".GameID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".\"UtilityID\"\n"
        "INNER JOIN \"%3\" ON \"%3\".\"ItemID\" = \"%1\".GameID\n"
        "GROUP BY\n"
        "   \"%1\".GameID\n"
        "ORDER BY\n"
        "   \"%1\".GameID;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName));
        
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Apply the queried data into model data.
        while (m_query.next())
        {
            long long int gameID = m_query.value(0).toLongLong();
            QString utilityName = m_query.value(1).toString();

            for (int i = 0; i < rowCount(); i++)
            {
                if (m_data.at(i).gameID == gameID)
                {
                    if (tableName == UtilityTableName::CATEGORIES)
                        m_data[i].categories = utilityName;
                    else if (tableName == UtilityTableName::DEVELOPPERS)
                        m_data[i].developpers = utilityName;
                    else if (tableName == UtilityTableName::PUBLISHERS)
                        m_data[i].publishers = utilityName;
                    else if (tableName == UtilityTableName::PLATFORM)
                        m_data[i].platform = utilityName;
                    else if (tableName == UtilityTableName::SERVICES)
                        m_data[i].services = utilityName;
                    break;
                }
            }
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Categories of the %1 table.\n\t%2")
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

void TableModelGame::queryUtilityField(UtilityTableName tableName, long long int gameID)
{
    // Stardard interface to update the utility field of the game (gameID).
    if (!m_isTableCreated)
        return;

    QString statement = QString(
        "SELECT\n"
        "   \"%1\".GameID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
        "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".GameID\n"
        "WHERE\n"
        "   \"%1\".GameID = %4\n"
        "GROUP BY\n"
        "   \"%1\".GameID;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName))
            .arg(gameID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findGamePos(gameID);
        if (pos >= 0 && pos <= rowCount())
        {
            QString utilityName;
            if (m_query.next())
                utilityName = m_query.value(1).toString();

            if (tableName == UtilityTableName::CATEGORIES)
                m_data[pos].categories = utilityName;
            else if (tableName == UtilityTableName::DEVELOPPERS)
                m_data[pos].developpers = utilityName;
            else if (tableName == UtilityTableName::PUBLISHERS)
                m_data[pos].publishers = utilityName;
            else if (tableName == UtilityTableName::PLATFORM)
                m_data[pos].platform = utilityName;
            else if (tableName == UtilityTableName::SERVICES)
                m_data[pos].services = utilityName;
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Categories of the game %1 in the table %2.\n\t%3")
            .arg(gameID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

int TableModelGame::findGamePos(long long int gameID) const
{
    for (int i = 0; i < size(); i++)
        if (m_data.at(i).gameID == gameID)
            return i;
    return -1;
}

void TableModelGame::queryCategoriesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES);
}

void TableModelGame::queryCategoriesField(long long int gameID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES, gameID);
}

void TableModelGame::queryDeveloppersField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::DEVELOPPERS);
}

void TableModelGame::queryDeveloppersField(long long int gameID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::DEVELOPPERS, gameID);
}

void TableModelGame::queryPublishersField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PUBLISHERS);
}

void TableModelGame::queryPublishersField(long long int gameID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PUBLISHERS, gameID);
}

void TableModelGame::queryPlatformField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PLATFORM);
}

void TableModelGame::queryPlatformField(long long int gameID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PLATFORM, gameID);
}

void TableModelGame::queryServicesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERVICES);
}

void TableModelGame::queryServicesField(long long int gameID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERVICES, gameID);
}

void TableModelGame::querySensitiveContentField()
{
    // Getting the Sensitive Content Information for each game.
    QString statement = QString(
        "SELECT\n"
        "   ItemID,\n"
        "   '%2 ' || \"%3\" || ', %4 ' || \"%5\" || ', %6 ' || \"%7\"\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   ItemID;")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(tr("Explicit Content"))
            .arg("ExplicitContent")
            .arg(tr("Violence Content"))
            .arg("ViolenceContent")
            .arg(tr("Bad Language"))
            .arg("BadLanguage");
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

    if (m_query.exec(statement))
    {
        while (m_query.next())
        {
            long long int gameID = m_query.value(0).toLongLong();
            QString sensitiveContent = m_query.value(1).toString();

            for (int i = 0; i < size(); i++)
            {
                if (m_data.at(i).gameID == gameID)
                {
                    m_data[i].sensitiveContent = sensitiveContent;
                    break;
                }
            }
        }
        m_query.clear();
    }
    else
        std::cerr << QString("Failed to query %1 utility interface table.\n\t%2")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
}

void TableModelGame::querySensitiveContentField(long long int gameID)
{
    // Updating the field sensitive content of the game (gameID).
    QString statement = QString(
        "SELECT\n"
        "   ItemID,\n"
        "   '%2 ' || \"%3\" || ', %4 ' || \"%5\" || ', %6 ' || \"%7\"\n"
        "FROM\n"
        "   \"%1\"\n"
        "WHERE\n"
        "   ItemID = %8;")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(tr("Explicit Content"))
            .arg("ExplicitContent")
            .arg(tr("Violence Content"))
            .arg("ViolenceContent")
            .arg(tr("Bad Language"))
            .arg("BadLanguage")
            .arg(gameID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findGamePos(gameID);
        if (pos >= 0 && pos < rowCount() && m_query.next())
        {
            m_data[pos].sensitiveContent = m_query.value(1).toString();
            emit dataChanged(index(pos, Game::SENSITIVE_CONTENT), index(pos, Game::SENSITIVE_CONTENT));
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Sensitive Content of the game %1 in the table %2.\n\t%3")
            .arg(gameID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

QString TableModelGame::url(const QModelIndex& index) const
{
    if (index.row() >= 0 && index.row() < size())
        return m_data.at(index.row()).url;
    return QString();
}

void TableModelGame::setUrl(const QModelIndex& index, const QString& url)
{
    // Set the url of a game.
    if (index.isValid() && index.row() > 0 && index.row() < size())
    {
        long long int gameID = m_data.at(index.row()).gameID;

        QString statement = QString(
            "UPDATE \"%1\"\n"
            "SET Url = \"%2\"\n"
            "WHERE GameID = %3;")
                .arg(m_tableName)
                .arg(url)
                .arg(m_data.at(index.row()).gameID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
        {
            m_data[index.row()].url = url;
            emit listEdited();
        }
        else
        {
#ifndef NDEBUG
            std::cerr << QString("Failed to set the url to the table \"%1\".\n\t%2")
                .arg(m_tableName)
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
#endif
        }
    }
}

TableModel_UtilityInterface* TableModelGame::utilityInterface()
{
    if (m_isTableCreated)
        return m_interface;
    return nullptr;
}