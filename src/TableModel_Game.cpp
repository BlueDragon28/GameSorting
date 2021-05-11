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

#include "TableModel.h"
#include "TableModel_UtilityInterface.h"
#include <QSqlError>
#include <iostream>

#define GAME_TABLE_COLUMN_COUNT 8

template<typename T>
bool TableModel::gameUpdateField(QSqlQuery& query, const QString& columnName, int rowNB, T value)
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
            .arg(m_gameListData->at(rowNB).gameID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!query.exec(statement))
    {
        std::cout << QString("Failed to update field %1 of GameID: %2.\n\t%3")
            .arg(columnName)
            .arg(m_gameListData->at(rowNB).gameID)
            .arg(query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
        return false;
    }
    m_query.clear();

    return true;
}

template<>
bool TableModel::gameUpdateField(QSqlQuery& query, const QString& columnName, int rowNB, const QString& value)
{
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = \"%3\"\n"
        "WHERE\n"
        "   GameID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_gameListData->at(rowNB).gameID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!query.exec(statement))
    {
        std::cout << QString("Failed to update field %1 of GameID : %2.\n\t%%3")
            .arg(columnName)
            .arg(m_gameListData->at(rowNB).gameID)
            .arg(query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
        return false;
    }
    m_query.clear();

    return true;
}

void TableModel::gameCreateTable()
{
    // Create the game SQL table with all the columns.
    if (m_db.isOpen())
    {
        QString statement = QString(
            "CREATE TABLE \"%1\" (\n"
            "   GameID INTEGER PRIMARY KEY,\n"
            "   GamePos INTEGER,\n"
            "   Name TEXT DEFAULT \"New Game\",\n"
            "   SensitiveContent INTEGER,\n"
            "   Url TEXT,\n"
            "   Rate INTEGER DEFAULT 5);")
                .arg(m_tableName);

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
            m_isTableCreated = true;
        else
            std::cerr << "Failed to create table " << m_tableName.toLocal8Bit().constData() << "\n\t"
                << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
        m_query.clear();
    }
}

int TableModel::gameColumnCount() const
{
    return GAME_TABLE_COLUMN_COUNT;
}

int TableModel::gameRowCount() const
{
    return m_listCount;
}

QVariant TableModel::gameData(const QModelIndex& index, int role) const
{
    // Returning the game table data into the view.
    if (index.column() >= 0 && index.column() < GAME_TABLE_COLUMN_COUNT &&
        index.row() >= 0 && index.row() < m_listCount)
    {
        // Returning to the tableView the data from the list.
        switch (index.column())
        {
        case Game::NAME:
            return m_gameListData->at(index.row()).name;
        case Game::CATEGORIES:
            return m_gameListData->at(index.row()).categories;
        case Game::DEVELOPPERS:
            return m_gameListData->at(index.row()).developpers;
        case Game::PUBLISHERS:
            return m_gameListData->at(index.row()).publishers;
        case Game::PLATFORMS:
            return m_gameListData->at(index.row()).platform;
        case Game::SERVICES:
            return m_gameListData->at(index.row()).services;
        case Game::SENSITIVE_CONTENT:
            return m_gameListData->at(index.row()).sensitiveContent;
        case Game::RATE:
            return m_gameListData->at(index.row()).rate;
        }
    }

    return QVariant();
}

bool TableModel::gameSetData(const QModelIndex& index, const QVariant& value, int role)
{
    // Setting field data.
    // Each column is different, so we working on each column independently.
    if (role == Qt::EditRole && 
        index.column() >= 0 && index.column() <= GAME_TABLE_COLUMN_COUNT &&
        index.row() >= 0 && index.row() < m_gameListData->size())
    {
        switch (index.column())
        {
        case Game::NAME:
        {
            if (value.canConvert<QString>())
            {
                QString gameName = value.toString();
                (*m_gameListData)[index.row()].name = gameName;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<const QString&>(query, "Name", index.row(), gameName);
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
            if (value.canConvert<int>())
            {
                int rate = value.toInt();
                (*m_gameListData)[index.row()].rate = rate;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Rate", index.row(), rate);
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

bool TableModel::gameInsertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting new rows into the table.
    if (row >= 0 && row <= m_listCount &&
        count > 0)
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
                "   \"%1\";")
                    .arg(m_tableName);
                
#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (query.exec(statement))
            {
                if (query.last())
                {
                    if (count == 1)
                        beginInsertRows(QModelIndex(), m_listCount, m_listCount);
                    else
                        beginInsertRows(QModelIndex(), m_listCount, m_listCount + (count - 1));

                    QList<GameItem> gameList;
                    int i = 0;
                    do
                    {
                        GameItem game = {};
                        game.gameID = query.value(0).toLongLong();
                        game.gamePos = query.value(1).toLongLong();
                        game.name = query.value(2).toString();
                        game.url = query.value(3).toString();
                        game.rate = query.value(4).toInt();
                        gameList.prepend(game);
                        i++;
                    } while (query.previous() && i < count);
                    m_gameListData->append(gameList.cbegin(), gameList.cend());
                    m_listCount = m_gameListData->size();

                    endInsertRows();
                }
                else
                    updateQuery();
            }
            else
                updateQuery();
            
            // Emit the signal listEdited, this signal is used to tell that the list has been edited.
            emit listEdited();
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

bool TableModel::gameRemoveRows(int row, int count, const QModelIndex& parent)
{
    // Removing rows from the table.
    if (row >= 0 && row < m_listCount &&
        row + (count - 1) < m_listCount)
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
                .arg(m_gameListData->at(row).gameID);
                itemsID[0] = m_gameListData->at(row).gameID;
        }
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ", ";
                statement += QString::number(m_gameListData->at(row+i).gameID);
                itemsID[i] = m_gameListData->at(row+i).gameID;
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
            
            m_gameListData->remove(row, count);
            m_utilityInterface->rowRemoved(itemsID);

            endRemoveRows();

            emit listEdited();
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

QVariant TableModel::gameHeaderData(int section, Qt::Orientation orientation, int role) const
{
    // Setting the headers' name.
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
        return "Sensitive Content";
    case 7:
        return "Rate";
    default:
        return QVariant();
    }
}

long long int TableModel::gameItemID(const QModelIndex& index) const
{
    if (index.isValid() && index.row() < m_listCount)
        return m_gameListData->at(index.row()).gameID;
    
    return -1;
}

void TableModel::gameUpdateQuery()
{
    // Retrieve the entrire game data of the table and 
    // and put it into the view.
    if (m_listCount > 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_gameListData->size()-1);
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
        m_gameListData->clear();
        
        while (m_query.next())
        {
            GameItem game = {};
            game.gameID = m_query.value(0).toLongLong();
            game.gamePos = m_query.value(1).toLongLong();
            game.name = m_query.value(2).toString();
            game.url = m_query.value(3).toString();
            game.rate = m_query.value(4).toInt();
            m_gameListData->append(game);
        }

        if (m_gameListData->size() > 0)
        {
            m_listCount = m_gameListData->size();

            // Quering the util table and set it into the specifics rows.
            gameQueryCategoriesField();
            gameQueryDeveloppersField();
            gameQueryPublishersField();
            gameQueryPlatformField();
            gameQueryServicesField();
            gameQuerySensitiveContentField();

            beginInsertRows(QModelIndex(), 0, m_gameListData->size()-1);
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

void TableModel::gameQueryUtilityField(UtilityTableName tableName)
{
    // Standard interface to query the utility data except the sensitive data.
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
            .arg(m_utilityInterface->tableName(tableName));
        
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
                if (m_gameListData->at(i).gameID == gameID)
                {
                    if (tableName == UtilityTableName::CATEGORIES)
                        (*m_gameListData)[i].categories = utilityName;
                    else if (tableName == UtilityTableName::DEVELOPPERS)
                        (*m_gameListData)[i].developpers = utilityName;
                    else if (tableName == UtilityTableName::PUBLISHERS)
                        (*m_gameListData)[i].publishers = utilityName;
                    else if (tableName == UtilityTableName::PLATFORM)
                        (*m_gameListData)[i].platform = utilityName;
                    else if (tableName == UtilityTableName::SERVICES)
                        (*m_gameListData)[i].services = utilityName;
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

void TableModel::gameQueryUtilityField(long long int gameID, UtilityTableName tableName)
{
    // Stardard interface to update the utility field of the game (gameID).
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
            .arg(m_utilityInterface->tableName(tableName))
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
                (*m_gameListData)[pos].categories = utilityName;
            else if (tableName == UtilityTableName::DEVELOPPERS)
                (*m_gameListData)[pos].developpers = utilityName;
            else if (tableName == UtilityTableName::PUBLISHERS)
                (*m_gameListData)[pos].publishers = utilityName;
            else if (tableName == UtilityTableName::PLATFORM)
                (*m_gameListData)[pos].platform = utilityName;
            else if (tableName == UtilityTableName::SERVICES)
                (*m_gameListData)[pos].services = utilityName;
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

void TableModel::gameQueryCategoriesField()
{
    // Gettings all the categories of a game by concatenate all the field
    // of Categories Utility table.
    gameQueryUtilityField(UtilityTableName::CATEGORIES);
}

void TableModel::gameQueryCategoriesField(long long int gameID)
{
    // Updating the field category of the game (gameID).
    gameQueryUtilityField(gameID, UtilityTableName::CATEGORIES);
}

void TableModel::gameQueryDeveloppersField()
{
    // Gettings all the Developpers of a game by concatenate all the field
    // of Developpers Utility table.
    gameQueryUtilityField(UtilityTableName::DEVELOPPERS);
}

void TableModel::gameQueryDeveloppersField(long long int gameID)
{
    // Updating the field developpers of the game (gameID).
    gameQueryUtilityField(gameID, UtilityTableName::DEVELOPPERS);
}

void TableModel::gameQueryPublishersField()
{
    // Gettings all the Publishers of a game by concatenate all the field
    // of Publishers Utility table.
    gameQueryUtilityField(UtilityTableName::PUBLISHERS);
}

void TableModel::gameQueryPublishersField(long long int gameID)
{
    // Updating the field publishers of the game (gameID).
    gameQueryUtilityField(gameID, UtilityTableName::PUBLISHERS);
}

void TableModel::gameQueryPlatformField()
{
    // Gettings all the Platform of a game by concatenate all the field
    // of Platform Utility table.
    gameQueryUtilityField(UtilityTableName::PLATFORM);
}

void TableModel::gameQueryPlatformField(long long int gameID)
{
    // Updating the field platform of the game (gameID).
    gameQueryUtilityField(gameID, UtilityTableName::PLATFORM);
}

void TableModel::gameQueryServicesField()
{
    // Gettings all the Platform of a game by concatenate all the field
    // of Platform Utility table.
    gameQueryUtilityField(UtilityTableName::SERVICES);
}

void TableModel::gameQueryServicesField(long long int gameID)
{
    // Updating the field platform of the game (gameID).
    gameQueryUtilityField(gameID, UtilityTableName::SERVICES);
}

void TableModel::gameQuerySensitiveContentField()
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
            .arg(m_utilityInterface->tableName(UtilityTableName::SENSITIVE_CONTENT))
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

            for (int i = 0; i < m_listCount; i++)
            {
                if (m_gameListData->at(i).gameID == gameID)
                {
                    (*m_gameListData)[i].sensitiveContent = sensitiveContent;
                    break;
                }
            }
        }
        m_query.clear();
    }
    else
        std::cerr << QString("Failed to query %1 utility interface table.\n\t%2")
            .arg(m_utilityInterface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
}

void TableModel::gameQuerySensitiveContentField(long long int gameID)
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
            .arg(m_utilityInterface->tableName(UtilityTableName::SENSITIVE_CONTENT))
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
            (*m_gameListData)[pos].sensitiveContent = m_query.value(1).toString();
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

QVariant TableModel::gameRetrieveData() const
{
    // Return the data of the table (game list) and the utility interface data.
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

    QVariant utilityInterface = m_utilityInterface->data();
    if (!utilityInterface.canConvert<Game::SaveUtilityInterfaceData>())
        return QVariant();
    
    data.interface = qvariant_cast<Game::SaveUtilityInterfaceData>(utilityInterface);

    return QVariant::fromValue(data);
}

bool TableModel::setGameItemsData(const QVariant& variant)
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
    if (m_utilityInterface)
    {
        delete m_utilityInterface;
        m_utilityInterface = nullptr;
    }
    m_utilityInterface = new TableModel_UtilityInterface(m_tableName, m_listType, m_db, QVariant::fromValue(data.interface));
    if (!m_utilityInterface->isTableReady())
    {
        m_isTableCreated = false;
        return false;
    }
    connect(m_utilityInterface, &TableModel_UtilityInterface::interfaceChanged, this, &TableModel::utilityChanged);

    // Then, query the whole table.
    gameUpdateQuery();

    return true;
}

void TableModel::gameUtilityChanged(long long int gameID, UtilityTableName tableName)
{
    std::cout << QString("GameID: %1, rowCount(): %2.").arg(gameID).arg(rowCount()).toLocal8Bit().constData() << std::endl;
    // Update an utility interface (tableName) of a specific game (gameID).
    if (gameID >= 0 && gameID <= rowCount() && rowCount() > 0)
    {
        if (tableName == UtilityTableName::CATEGORIES)
            gameQueryCategoriesField(gameID);
        else if (tableName == UtilityTableName::DEVELOPPERS)
            gameQueryDeveloppersField(gameID);
        else if (tableName == UtilityTableName::PUBLISHERS)
            gameQueryPublishersField(gameID);
        else if (tableName == UtilityTableName::PLATFORM)
            gameQueryPlatformField(gameID);
        else if (tableName == UtilityTableName::SERVICES)
            gameQueryServicesField(gameID);
        else if (tableName == UtilityTableName::SENSITIVE_CONTENT)
            gameQuerySensitiveContentField(gameID);
    }
}

int TableModel::findGamePos(long long int gameID) const
{
    for (int i = 0; i < m_gameListData->size(); i++)
    {
        if (m_gameListData->at(i).gameID == gameID)
            return i;
    }
    return -1;
}