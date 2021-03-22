#include "TableModel.h"
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
            "   Categories INTEGER,\n"
            "   Developpers INTEGER,\n"
            "   Publishers INTEGER,\n"
            "   Platform INTEGER,\n"
            "   Services INTEGER,\n"
            "   SensitiveContent INTEGER,\n"
            "   Url TEXT,\n"
            "   Rate INTEGER);")
                .arg(m_tableName);

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
            m_isTableCreated = true;
        else
            std::cerr << "Failed to create table " << m_tableName.toLocal8Bit().constData() << "\n\t"
                << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
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
        case 0:
            return m_gameListData->at(index.row()).name;
        case 1:
            return m_gameListData->at(index.row()).categories;
        case 2:
            return m_gameListData->at(index.row()).developpers;
        case 3:
            return m_gameListData->at(index.row()).publishers;
        case 4:
            return m_gameListData->at(index.row()).platform;
        case 5:
            return m_gameListData->at(index.row()).services;
        case 6:
            return m_gameListData->at(index.row()).sensitiveContent;
        case 7:
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
        case 0:
        {
            if (value.canConvert<QString>())
            {
                QString gameName = value.toString();
                (*m_gameListData)[index.row()].name = gameName;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<const QString&>(query, "Name", index.row(), gameName);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
                return result;
            }
            else
                return false;
        } break;

        case 1:
        {
            if (value.canConvert<int>())
            {
                int categories = value.toInt();
                (*m_gameListData)[index.row()].categories = categories;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Categories", index.row(), categories);
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
                (*m_gameListData)[index.row()].developpers = developpers;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Developpers", index.row(), developpers);
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
                (*m_gameListData)[index.row()].publishers = publishers;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Publishers", index.row(), publishers);
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
                (*m_gameListData)[index.row()].platform = platform;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Platform", index.row(), platform);
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
                (*m_gameListData)[index.row()].services = services;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Services", index.row(), services);
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
                int sensitiveContent = value.toInt();
                (*m_gameListData)[index.row()].sensitiveContent = sensitiveContent;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "SensitiveContent", index.row(), sensitiveContent);
                if (result)
                    emit dataChanged(index, index, {Qt::EditRole});
            }
        } break;

        case 7:
        {
            if (value.canConvert<int>())
            {
                int rate = value.toInt();
                (*m_gameListData)[index.row()].rate = rate;

                QSqlQuery query(m_db);
                bool result = gameUpdateField<int>(query, "Rate", index.row(), rate);
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
            "   Categories,\n"
            "   Developpers,\n"
            "   Publishers,\n"
            "   Platform,\n"
            "   Services,\n"
            "   SensitiveContent,\n"
            "   Url,\n"
            "   Rate )\n"
            "VALUES")
                .arg(m_tableName);

        for (int i = 0; i < count; i++)
        {
            statement += 
                "\n   (NULL, \"New Game\", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),";
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
                "   Categories,\n"
                "   Publishers,\n"
                "   Platform,\n"
                "   Services,\n"
                "   SensitiveContent,\n"
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
                        game.categories = query.value(3).toInt();
                        game.developpers = query.value(4).toInt();
                        game.publishers = query.value(5).toInt();
                        game.platform = query.value(6).toInt();
                        game.services = query.value(7).toInt();
                        game.sensitiveContent = query.value(8).toInt();
                        game.url = query.value(9).toString();
                        game.rate = query.value(10).toInt();
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
            "DELETE FROM %1\n"
            "WHERE GameID ")
            .arg(m_tableName);
        
        if (count == 1)
            statement += QString("= %1;")
                .arg(m_gameListData->at(row).gameID);
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ", ";
                statement += QString::number(m_gameListData->at(i).gameID);
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
    m_query.clear();

    QString statement = QString(
        "SELECT\n"
        "   GameID,\n"
        "   GamePos,\n"
        "   Name,\n"
        "   Categories,\n"
        "   Developpers,\n"
        "   Publishers,\n"
        "   Platform,\n"
        "   Services,\n"
        "   SensitiveContent,\n"
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
            game.categories = m_query.value(3).toInt();
            game.developpers = m_query.value(4).toInt();
            game.publishers = m_query.value(5).toInt();
            game.platform = m_query.value(6).toInt();
            game.services = m_query.value(7).toInt();
            game.sensitiveContent = m_query.value(8).toInt();
            game.url = m_query.value(9).toString();
            game.rate = m_query.value(10).toInt();
            m_gameListData->append(game);
        }

        if (m_gameListData->size() > 0)
        {
            m_listCount = m_gameListData->size();
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