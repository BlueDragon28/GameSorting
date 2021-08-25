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

#include "TableModelMovies.h"
#include "TableModelMovies_UtilityInterface.h"
#include <QSqlError>
#include <iostream>
#include <algorithm>

#define MOVIES_TABLE_COLUMN_COUNT 9
#define NUMBER_MOVIES_TABLE_COLUMN_COUNT 8

template<typename T>
bool TableModelMovies::updateField(const QString& columnName, int rowNB, T value)
{
    // Helper member functin to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = %3\n"
        "WHERE\n"
        "   MovieID = %4")
        .arg(m_tableName, columnName)
        .arg(value)
        .arg(m_data.at(rowNB).movieID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
        std::cerr << QString("Failed to update field %1 of MovieID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).movieID)
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
        m_query.clear();
        return false;
    }
    m_query.clear();
    return true;
}

template<>
bool TableModelMovies::updateField(const QString& columnName, int rowNB, const QString& value)
{
    // Helper member functin to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = \"%3\"\n"
        "WHERE\n"
        "   MovieID = %4")
        .arg(m_tableName, columnName)
        .arg(value)
        .arg(m_data.at(rowNB).movieID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
        std::cerr << QString("Failed to update field %1 of MovieID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).movieID)
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
        m_query.clear();
        return false;
    }
    m_query.clear();
    return true;
}

TableModelMovies::TableModelMovies(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(tableName, db, utilityTable, parent),
    m_interface(nullptr)
{
    createTable();
    m_interface = new TableModelMovies_UtilityInterface(rawTableName(), m_db);
    connect(m_interface, &TableModelMovies_UtilityInterface::interfaceChanged, this, &TableModelMovies::utilityChanged);
    connect(m_interface, &TableModelMovies_UtilityInterface::interfaceChanged, this, &TableModelMovies::listEdited);
}

TableModelMovies::TableModelMovies(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(db, utilityTable, parent),
    m_interface(nullptr)
{
    setItemData(data);
    connect(m_interface, &TableModelMovies_UtilityInterface::interfaceChanged, this, &TableModelMovies::utilityChanged);
    connect(m_interface, &TableModelMovies_UtilityInterface::interfaceChanged, this, &TableModelMovies::listEdited);
}

TableModelMovies::~TableModelMovies()
{
    deleteTable();
    if (m_interface)
        delete m_interface;
}

int TableModelMovies::columnCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return MOVIES_TABLE_COLUMN_COUNT;
    else
        return 0;
}

int TableModelMovies::rowCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return m_data.size();
    else 
        return 0;
}

QVariant TableModelMovies::data(const QModelIndex& index, int role) const
{
    // returning the game table data into the view.
    if ((role == Qt::EditRole || role == Qt::DisplayRole) && m_isTableCreated)
    {
        if (index.column() >= 0 && index.column() < columnCount() &&
            index.row() >= 0 && index.row() < rowCount())
        {
            // Returning to the tableView the data from list.
            switch (index.column())
            {
            case Movie::NAME:
                return m_data.at(index.row()).name;
            case Movie::CATEGORIES:
                return m_data.at(index.row()).categories;
            case Movie::DIRECTORS:
                return m_data.at(index.row()).directors;
            case Movie::ACTORS:
                return m_data.at(index.row()).actors;
            case Movie::PRODUCTIONS:
                return m_data.at(index.row()).productions;
            case Movie::MUSIC:
                return m_data.at(index.row()).music;
            case Movie::SERVICES:
                return m_data.at(index.row()).services;
            case Movie::SENSITIVE_CONTENT:
                return QVariant::fromValue(m_data.at(index.row()).sensitiveContent);
            case Movie::RATE:
                return m_data.at(index.row()).rate;
            }
        }
    }

    return QVariant();
}

bool TableModelMovies::setData(const QModelIndex& index, const QVariant& data, int role)
{
    // Setting field data.
    // Each column is different, so we working on each column independently.
    if (role == Qt::EditRole && m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
    {
        switch (index.column())
        {
        case Movie::NAME:
        {
            if (data.canConvert<QString>())
            {
                QString movieName = data.toString();
                m_data[index.row()].name = movieName;

                bool result = updateField<const QString&>("Name", index.row(), movieName);
                if (result)
                {
                    emit dataChanged(index, index, {Qt::EditRole});
                    emit listEdited();
                }
                return result;
            }
        } break;
        case Movie::RATE:
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
        } break;
        }
    }

    return true;
}

bool TableModelMovies::insertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting new rows into the table.
    if (row >= 0 && row <= rowCount() &&
        count > 0 && m_isTableCreated)
    {
        // MoviePos  statement
        QString posStatement = QString(
            "SELECT\n"
            "   MAX(MoviePos)\n"
            "FROM\n"
            "   \"%1\";").arg(m_tableName);

#ifndef NDEBUG
        std::cout << posStatement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        int maxPos = 0;
        if (m_query.exec(posStatement))
        {
            if (m_query.next())
            {
                maxPos = m_query.value(0).toInt();
                m_query.clear();
            }
        }
        else
        {
            std::cerr << QString("Failed to get max position on the table %1.\n\t%2")
                .arg(m_tableName, m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            m_query.clear();
        }

        // Executing the sql statement for inserting new rows.
        QString statement = QString(
            "INSERT INTO \"%1\" (\n"
            "   MoviePos,\n"
            "   Name,\n"
            "   Url,\n"
            "   Rate )\n"
            "VALUES")
                .arg(m_tableName);
        
        for (int i = 0; i < count; i++)
        {
            if (i > 0)
                statement += ',';

            statement += QString(
                "\n\t(%1, \"New Movie\", NULL, NULL)")
                .arg(++maxPos);
        }
        statement += ';';

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        // Then, querying the new inserted data and add it to the m_listData
        if (m_query.exec(statement))
        {
            statement = QString(
                "SELECT\n"
                "   MovieID,\n"
                "   MoviePos,\n"
                "   Name,\n"
                "   Url,\n"
                "   Rate\n"
                "FROM\n"
                "   \"%1\"\n"
                "ORDER BY\n"
                "   MovieID DESC\n"
                "LIMIT\n"
                "   %2;")
                    .arg(m_tableName)
                    .arg(count);
            
#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (m_query.exec(statement))
            {
                if (count == 1)
                    beginInsertRows(QModelIndex(), rowCount(), rowCount());
                else
                    beginInsertRows(QModelIndex(), rowCount(), rowCount() + (count - 1));
                
                QList<MovieItem> movieList;
                while (m_query.next())
                {
                    MovieItem movie = {};
                    movie.movieID = m_query.value(0).toLongLong();
                    movie.moviePos = m_query.value(1).toLongLong();
                    movie.name = m_query.value(2).toString();
                    movie.url = m_query.value(3).toString();
                    movie.rate = m_query.value(4).toInt();
                    movieList.prepend(movie);
                }
                m_data.append(movieList);

                endInsertRows();
            }
            else
                updateQuery();

            // Emit the signal lsitEdited, this signal is used to tell that the list has been edited.
            emit listEdited();
            m_query.clear();
        }
        else
        {
#ifndef NDEBUG
            std::cerr << QString("Failed to insert row of table %1\n\t%2")
                .arg(m_tableName)
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
#endif
            return false;
        }
    }

    return true;
}

bool TableModelMovies::removeRows(int row, int count, const QModelIndex& parent)
{
    // Removing rows from the table.
    if (row >= 0 && row < rowCount() &
        row + (count - 1) < rowCount() && m_isTableCreated)
    {
        // Defining the SQL statement.
        QString statement = QString(
            "DELETE FROM \"%1\"\n"
            "WHERE MovieID ")
            .arg(m_tableName);
        QList<long long int> itemsID(count);

        if (count == 1)
        {
            statement += QString("= %1;")
                .arg(m_data.at(row).movieID);
            itemsID[0] = m_data.at(row).movieID;
        }
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ';';
                statement += QString::number(m_data.at(row+i).movieID);
                itemsID[i] = m_data.at(row+i).movieID;
            }
        }

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif
        
        // Execute the SQL statement.
        // If the statement success, then
        // removing the data from the list
        // and updating the view.
        if (m_query.exec(statement))
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
                m_query.lastError().text().toLocal8Bit().constData() << std::endl;
#endif
            return false;
        }
    }

    return true;
}

Qt::ItemFlags TableModelMovies::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant TableModelMovies::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        // Setting the headers' name.
        switch (section)
        {
        case Movie::NAME:
            return "Name";
        case Movie::CATEGORIES:
            return "Categories";
        case Movie::DIRECTORS:
            return "Directors";
        case Movie::ACTORS:
            return "Actors";
        case Movie::PRODUCTIONS:
            return "Production";
        case Movie::MUSIC:
            return "Music";
        case Movie::SERVICES:
            return "Services";
        case Movie::SENSITIVE_CONTENT:
            return "Sensitive content";
        case Movie::RATE:
            return "Rate";
        }
    }
    return QVariant();
}

void TableModelMovies::appendRows(int count)
{
    if (count > 0 && m_isTableCreated)
        insertRows(rowCount(), count);
}

void TableModelMovies::deleteRows(const QModelIndexList& indexList)
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

int TableModelMovies::size() const
{
    return rowCount();
}

ListType TableModelMovies::listType() const
{
    return ListType::MOVIESLIST;
}

long long int TableModelMovies::itemID(const QModelIndex& index) const
{
    if (m_isTableCreated &&
        index.column() >= 0 && index.row() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
        return m_data.at(index.row()).movieID;
    return -1;
}

void TableModelMovies::updateQuery()
{
    // Retrieve the entrire game data of the table and 
    // and put it into the view.
    if (size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, size()-1);
        endRemoveRows();
    }

    QString statement = QString(
        "SELECT\n"
        "   MovieID,\n"
        "   MoviePos,\n"
        "   \"%1\".Name as mName,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "%4"
        "ORDER BY\n"
        "   %2 %3;")
            .arg(m_tableName);
    
    // Sorting view.
    if (m_sortingColumnID == Movie::NAME)
    {
        statement = statement.arg("mName");
        SORTING_ORDER(m_sortingOrder, statement);
    }
    else if (m_sortingColumnID == Movie::RATE)
    {
        statement = statement.arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement);
    }
    else
        statement = statement.arg("MoviePos", "ASC");
    
    // Filtering the view.
    if (m_listFilter.column == Movie::NAME)
    {
        QString where = QString(
            "WHERE\n"
            "   mName LIKE \"%%1%\"\n")
            .arg(m_listFilter.pattern);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column >= Movie::CATEGORIES &&
        m_listFilter.column <= Movie::SERVICES)
    {
        UtilityTableName tName;
        if (m_listFilter.column == Movie::CATEGORIES)
            tName = UtilityTableName::CATEGORIES;
        else if (m_listFilter.column == Movie::DIRECTORS)
            tName = UtilityTableName::DIRECTOR;
        else if (m_listFilter.column == Movie::ACTORS)
            tName = UtilityTableName::ACTORS;
        else if (m_listFilter.column == Movie::PRODUCTIONS)
            tName = UtilityTableName::PRODUCTION;
        else if (m_listFilter.column == Movie::MUSIC)
            tName = UtilityTableName::MUSIC;
        else if (m_listFilter.column == Movie::SERVICES)
            tName = UtilityTableName::SERVICES;
        
        QString where = QString(
            "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
            "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".MovieID\n"
            "WHERE\n"
            "   \"%2\".\"%2ID\" IN (%4)\n"
            "GROUP BY\n"
            "   MovieID\n")
            .arg(m_tableName, m_utilityTable.tableName(tName), m_interface->tableName(tName));

        QString utilList;
        for (int i = 0; i < m_listFilter.utilityList.size(); i++)
        {
            if (i > 0)
                utilList += ',';
            utilList += QString::number(m_listFilter.utilityList.at(i));
        }
        statement = statement.arg(where.arg(utilList));
    }
    else if (m_listFilter.column == Movie::RATE)
    {
        QString where = QString(
            "WHERE\n"
            "   Rate = %1\n")
                .arg(m_listFilter.rate);
        statement = statement.arg(where);
    }
    else
        statement = statement.arg("");

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Putting the queried data into a QList.
        m_data.clear();

        while (m_query.next())
        {
            MovieItem movie = {};
            movie.movieID = m_query.value(0).toLongLong();
            movie.moviePos = m_query.value(1).toLongLong();
            movie.name = m_query.value(2).toString();
            movie.url = m_query.value(3).toString();
            movie.rate = m_query.value(4).toInt();
            m_data.append(movie);
        }

        if (size() > 0)
        {
            // Quering the util table and set it into the specifics rows.
            queryCategoriesField();
            queryDirectorsField();
            queryActorsField();
            queryProductionsField();
            queryMusicField();
            queryServicesField();
            querySensitiveContentField();

            if (m_sortingColumnID > 0 && m_sortingColumnID < 8)
                sortUtility(m_sortingColumnID);
            
            beginInsertRows(QModelIndex(), 0, size()-1);
            endInsertRows();
        }
    }
    else
        std::cerr << "Failed to update cell values of table " << m_tableName.toLocal8Bit().constData() << "\n\t"
            << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
}

QVariant TableModelMovies::retrieveData() const
{
    // Return the data of the table and the utility interface data.
    Movie::SaveDataTable data = {};
    data.tableName = m_tableName;

    // Retrieve movie data
    QSqlQuery query(m_db);

    QString statement = QString(
        "SELECT\n"
        "   MovieID,\n"
        "   MoviePos,\n"
        "   Name,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   MovieID ASC;")
            .arg(m_tableName);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif
    
    if (!query.exec(statement))
    {
        std::cerr << QString("Failed to query table %1\n\t%2")
            .arg(m_tableName, query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
        return QVariant();
    }

    while (query.next())
    {
        Movie::SaveItem movie = {};
        movie.movieID = query.value(0).toLongLong();
        movie.moviePos = query.value(1).toLongLong();
        movie.name = query.value(2).toString();
        movie.url = query.value(3).toString();
        movie.rate = query.value(4).toInt();
        data.movieList.append(movie);
    }

    QVariant utilityInterface = m_interface->data();
    if (utilityInterface.canConvert<Movie::SaveUtilityInterfaceData>())
    {
        std::cerr << "Cannot convert utility QVariant to Movie::SaveUtilityInterfaceData." << std::endl;
        return QVariant();
    }

    data.interface = qvariant_cast<Movie::SaveUtilityInterfaceData>(utilityInterface);

    data.columnSort = (signed char)m_sortingColumnID;
    data.sortOrder = (signed char)(m_sortingOrder == Qt::AscendingOrder ? 0 : 1);

    return QVariant::fromValue(data);
}

bool TableModelMovies::setItemData(const QVariant& variant)
{
    // Set the data into the TableModel SQL Tables.
    if (!variant.isValid() || !variant.canConvert<Movie::SaveDataTable>())
        return false;
    
    Movie::SaveDataTable data = qvariant_cast<Movie::SaveDataTable>(variant);

    m_tableName = data.tableName;
    if (m_tableName.isEmpty())
        return false;
    createTable();
    
    // Set the game list
    QString statement = QString(
        "INSERT INTO \"%1\" (MovieID, MoviePos, Name, Url, Rate)\n"
        "VALUES")
            .arg(m_tableName);
    
    for (long long int i = 0; i < data.movieList.size(); i+=10)
    {
        QString strData;
        for (long long int j = i; j < i+10 && j < data.movieList.size(); j++)
        {
            if (j > i)
                strData += ',';
            
            strData +=
                QString("\n\t(%1, %2, \"%3\", \"%4\", %5)")
                    .arg(data.movieList.at(j).movieID)
                    .arg(data.movieList.at(j).moviePos)
                    .arg(data.movieList.at(j).name)
                    .arg(data.movieList.at(j).url)
                    .arg(data.movieList.at(j).rate);
        }
        if (strData.size() > 0)
        {
            strData += ';';

#ifndef NDEBUG
            std::cout << (statement + strData).toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (!m_query.exec(statement + strData))
            {
                std::cerr << QString("Failed to exec statement for setting data into the table %1.\n\t%2")
                    .arg(m_tableName, m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
                m_query.clear();
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
    m_interface = new TableModelMovies_UtilityInterface(m_tableName, m_db, QVariant::fromValue(data.interface));
    if (!m_interface->isTableReady())
        return false;

    // Then, query the whole table.
    updateQuery();

    return true;
}

void TableModelMovies::createTable()
{
    // Create the Movies SQL table
    QString statement = QString(
        "CREATE TABLE \"%1\" (\n"
        "   MovieID INTEGER PRIMARY KEY,\n"
        "   MoviePos INTEGER,\n"
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
    else
    {
        std::cerr << QString("Failed to create the table %1.\n\t%2")
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
    }
    m_query.clear();
}

void TableModelMovies::deleteTable()
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
        std::cerr << QString("Failed to delete table %1.\n\t%2")
            .arg(m_tableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
    }
    m_query.clear();
}

void TableModelMovies::utilityChanged(long long int movieID, UtilityTableName tableName)
{
    // This member function is called when the utility interface is changed.
    if (movieID >= 0 && size() > 0 && m_isTableCreated)
    {
        if (tableName == UtilityTableName::CATEGORIES)
            queryCategoriesField();
        else if (tableName == UtilityTableName::DIRECTOR)
            queryDirectorsField();
        else if (tableName == UtilityTableName::ACTORS)
            queryActorsField();
        else if (tableName == UtilityTableName::PRODUCTION)
            queryProductionsField();
        else if (tableName == UtilityTableName::MUSIC)
            queryMusicField();
        else if (tableName == UtilityTableName::SERVICES)
            queryServicesField();
        else if (tableName == UtilityTableName::SENSITIVE_CONTENT)
            querySensitiveContentField();
    }
}

void TableModelMovies::queryUtilityField(UtilityTableName tableName)
{
    // Standard interface to query the utility data except the sensitive data.
    if (!m_isTableCreated)
        return;
    
    QString statement = QString(
        "SELECT\n"
        "   \"%1\".MovieID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
        "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".MovieID\n"
        "%6"
        "GROUP BY\n"
        "   \"%1\".MovieID\n"
        "ORDER BY\n"
        "   \"%1\".%4 %5;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName));
    
    // Sorting order
    if (m_sortingColumnID == Movie::NAME)
    {
        statement = statement.arg("Name");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Movie::RATE)
    {
        statement = statement.arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg("MoviePos").arg("ASC");
    
    // Filtering the view.
    if (m_listFilter.column == Movie::NAME)
    {
        QString where = QString(
            "WHERE\n"
            "   \"%1\".Name LIKE \"%%2\"\n")
                .arg(m_tableName, m_listFilter.pattern);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column >= Movie::CATEGORIES &&
        m_listFilter.column <= Movie::SERVICES)
    {
        UtilityTableName tName;
        if (m_listFilter.column == Movie::CATEGORIES)
            tName = UtilityTableName::CATEGORIES;
        else if (m_listFilter.column == Movie::DIRECTORS)
            tName = UtilityTableName::DIRECTOR;
        else if (m_listFilter.column == Movie::ACTORS)
            tName = UtilityTableName::ACTORS;
        else if (m_listFilter.column == Movie::PRODUCTIONS)
            tName = UtilityTableName::PRODUCTION;
        else if (m_listFilter.column == Movie::MUSIC)
            tName = UtilityTableName::MUSIC;
        else if (m_listFilter.column == Movie::SERVICES)
            tName = UtilityTableName::SERVICES;
        
        QString moviesID;
        for (int i = 0; i < m_data.size(); i++)
        {
            if (i > 0)
                moviesID += ',';
            moviesID += QString::number(m_data.at(i).movieID);
        }

        QString where = QString(
            "WHERE\n"
            "   \"%1\".MovieID IN (%2)\n")
                .arg(m_tableName, moviesID);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column == Movie::RATE)
    {
        QString where = QString(
            "WHERE\n"
            "   \"%1\".Rate = %2\n")
            .arg(m_tableName)
            .arg(m_listFilter.rate);
        statement = statement.arg(where);
    }
    else
        statement = statement.arg("");

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Apply the queried data into model data.
        while (m_query.next())
        {
            long long int movieID = m_query.value(0).toLongLong();
            QString utilityName = m_query.value(1).toString();

            for (int i = 0; i < m_data.size(); i++)
            {
                if (m_data.at(i).movieID == movieID)
                {
                    if (tableName == UtilityTableName::CATEGORIES)
                        m_data[i].categories = utilityName;
                    else if (tableName == UtilityTableName::DIRECTOR)
                        m_data[i].directors = utilityName;
                    else if (tableName == UtilityTableName::ACTORS)
                        m_data[i].actors = utilityName;
                    else if (tableName == UtilityTableName::PRODUCTION)
                        m_data[i].productions = utilityName;
                    else if (tableName == UtilityTableName::MUSIC)
                        m_data[i].music = utilityName;
                    else if (tableName == UtilityTableName::SERVICES)
                        m_data[i].services = utilityName;
                    break;
                }
            }
        }
    }
    else
    {
        std::cerr << QString("Failed to query Utilities of the %1 table.\n\t%2")
            .arg(m_tableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
    }
    m_query.clear();
}

void TableModelMovies::queryUtilityField(UtilityTableName tableName, long long int movieID)
{
    // Standard interface to update the utility field of the movie (movieID).
    if (!m_isTableCreated)
        return;
    
    QString statement = QString(
        "SELECT\n"
        "   \"%1\".MovieID,\n"
        "   GROUP_CONCACT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
        "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".MovieID\n"
        "WHERE\n"
        "   \"%1\".MovieID = %4;")
            .arg(m_tableName, m_utilityTable.tableName(tableName), m_interface->tableName(tableName))
            .arg(movieID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrived field into the view.
        int pos = findMoviePos(movieID);
        if (pos >= 0 && pos <= size())
        {
            QString utilityName;
            if (m_query.next())
                utilityName = m_query.value(1).toString();
            
            if (tableName == UtilityTableName::CATEGORIES)
                m_data[pos].categories = utilityName;
            else if (tableName == UtilityTableName::DIRECTOR)
                m_data[pos].directors = utilityName;
            else if (tableName == UtilityTableName::ACTORS)
                m_data[pos].actors = utilityName;
            else if (tableName == UtilityTableName::PRODUCTION)
                m_data[pos].productions = utilityName;
            else if (tableName == UtilityTableName::MUSIC)
                m_data[pos].music = utilityName;
            else if (tableName == UtilityTableName::SERVICES)
                m_data[pos].services = utilityName;
        }
    }
    else
        std::cerr << QString("Fialed to query Utilities of the game %1 in the table %2.\n\t%3")
            .arg(movieID)
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
    m_query.clear();
}

int TableModelMovies::findMoviePos(long long int movieID) const
{
    for (int i = 0; i < size(); i++)
        if (m_data.at(i).movieID == movieID)
            return i;
    return -1;
}

void TableModelMovies::queryCategoriesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES);
}

void TableModelMovies::queryCategoriesField(long long int movieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES, movieID);
}

void TableModelMovies::queryDirectorsField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::DIRECTOR);
}

void TableModelMovies::queryDirectorsField(long long int movieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::DIRECTOR, movieID);
}

void TableModelMovies::queryActorsField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::ACTORS);
}

void TableModelMovies::queryActorsField(long long int movieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::ACTORS, movieID);
}

void TableModelMovies::queryProductionsField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PRODUCTION);
}

void TableModelMovies::queryProductionsField(long long int movieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PRODUCTION, movieID);
}

void TableModelMovies::queryMusicField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::MUSIC);
}

void TableModelMovies::queryMusicField(long long int movieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::MUSIC, movieID);
}

void TableModelMovies::queryServicesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERVICES);
}

void TableModelMovies::queryServicesField(long long int movieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERVICES, movieID);
}

void TableModelMovies::querySensitiveContentField()
{
    // Getting the Sensitive Content Information for each movie.
    QString statement = QString(
        "SELECT\n"
        "   ItemID,\n"
        "   ExplicitContent,\n"
        "   ViolenceContent,\n"
        "   BadLanguage,\n"
        "   \"%2\".MovieID\n"
        "FROM\n"
        "   \"%1\", \"%2\"\n"
        "ORDER BY\n"
        "   \"%3\".%4 %5;")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(m_tableName);

    // Sorting order
    if (m_sortingColumnID == 0)
    {
        statement = statement.arg(m_tableName).arg("Name");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == 7)
    {
        statement = statement.arg(m_tableName).arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg(m_tableName).arg("MovieID").arg("ASC");
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

    if (m_query.exec(statement))
    {
        while (m_query.next())
        {
            long long int movieID = m_query.value(0).toLongLong();
            SensitiveContent sensData = {};
            sensData.explicitContent = m_query.value(1).toInt();
            sensData.violenceContent = m_query.value(2).toInt();
            sensData.badLanguageContent = m_query.value(3).toInt();

            for (int i = 0; i < size(); i++)
            {
                if (m_data.at(i).movieID == movieID)
                {
                    m_data[i].sensitiveContent = sensData;
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

void TableModelMovies::querySensitiveContentField(long long int movieID)
{
    // Updating the field sensitive content of the movie (movieID).
    QString statement = QString(
        "SELECT\n"
        "   ItemID,\n"
        "   ExplicitContent,\n"
        "   ViolenceContent,\n"
        "   BadLanguage\n"
        "FROM\n"
        "   \"%1\"\n"
        "WHERE\n"
        "   ItemID = %2;")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(movieID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findMoviePos(movieID);
        if (pos >= 0 && pos < rowCount() && m_query.next())
        {
            SensitiveContent sensData = {};
            sensData.explicitContent = m_query.value(1).toInt();
            sensData.violenceContent = m_query.value(2).toInt();
            sensData.badLanguageContent = m_query.value(3).toInt();
            m_data[pos].sensitiveContent = sensData;
            emit dataChanged(index(pos, Game::SENSITIVE_CONTENT), index(pos, Game::SENSITIVE_CONTENT));
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Sensitive Content of the movie %1 in the table %2.\n\t%3")
            .arg(movieID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

QString TableModelMovies::url(const QModelIndex& index) const
{
    if (index.row() >= 0 && index.row() < size())
        return m_data.at(index.row()).url;
    return QString();
}

void TableModelMovies::setUrl(const QModelIndex& index, const QString& url)
{
    // Set the url of a movie.
    if (index.isValid() && index.row() > 0 && index.row() < size())
    {
        long long int movieID = m_data.at(index.row()).movieID;

        QString statement = QString(
            "UPDATE \"%1\"\n"
            "SET Url = \"%2\"\n"
            "WHERE MovieID = %3;")
                .arg(m_tableName)
                .arg(url)
                .arg(m_data.at(index.row()).movieID);
        
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

TableModel_UtilityInterface* TableModelMovies::utilityInterface()
{
    if (m_isTableCreated)
        return m_interface;
    return nullptr;
}

void TableModelMovies::updateMoviePos(int from)
{
    // Update the MoviePos SQL column, used to apply order in the view.
    if (from < 0)
        from = 0;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET MoviePos = %2\n"
        "WHERE MovieID = %3;");
    
    for (int i = from; i < size(); i++)
    {
        if (m_data.at(i).moviePos != i)
        {
            QString statement = baseStatement
                .arg(m_tableName)
                .arg(i)
                .arg(m_data.at(i).movieID);

#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (m_query.exec(statement))
            {
                m_data[i].moviePos = i;
                m_query.clear();
            }
            else
            {
                std::cerr << QString("Failed to update game position of movie %1 of the table %2.\n\t%3")
                    .arg(m_data.at(i).movieID)
                    .arg(m_tableName)
                    .arg(m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
                m_query.clear();
            }
        }
    }
}

QItemSelection TableModelMovies::moveItemsUp(const QModelIndexList& indexList)
{
    // Move the selected items in the view up by one row.
    QModelIndexList indexListCpy(indexList);
    std::sort(indexListCpy.begin(), indexListCpy.end(),
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() < index2.row();
        });
    
    QItemSelection selectedIndex;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   MoviePos = %2\n"
        "WHERE MovieID = %3;");
    
    foreach (const QModelIndex& index, indexListCpy)
    {
        // If index is equal to 0, ignore
        if (index.row() == 0)
            continue;
        
        QString statement = baseStatement
            .arg(m_tableName)
            .arg(m_data.at(index.row()).moviePos-1)
            .arg(m_data.at(index.row()).movieID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
        {
            // Store the movie and delete it from the movies list.
            beginRemoveRows(QModelIndex(), index.row(), index.row());
            endRemoveRows();
            // Move the item ot the new position.
            m_data.move(index.row(), index.row()-1);
            beginInsertRows(QModelIndex(), index.row()-1, index.row()-1);
            endInsertRows();
            emit listEdited();

            // Store the index of the moved intem into a list to be selected in the view.
            selectedIndex.append(QItemSelectionRange(
                this->index(index.row()-1, 0),
                this->index(index.row()-1, NUMBER_MOVIES_TABLE_COLUMN_COUNT)));

            updateMoviePos(index.row()-1);
            m_query.clear();
        }
        else
        {
            std::cerr << QString("Error: failed to move up items in the table %1.\n\t%2")
                .arg(m_tableName, m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            m_query.clear();
        }
    }

    return selectedIndex;
}

QItemSelection TableModelMovies::moveItemsDown(const QModelIndexList& indexList)
{
    // Move the selected items in the view down by one row.
    QModelIndexList indexListCpy(indexList);
    std::sort(indexListCpy.begin(), indexListCpy.end(),
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() > index2.row();
        });
    
    QItemSelection selectedIndex;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   MoviePos = %2\n"
        "WHERE MovieID = %3;");

    foreach (const QModelIndex& index, indexListCpy)
    {
        // If index is equal to 0, ignore
        if (index.row() == size()-1)
            continue;
        
        QString statement = baseStatement
            .arg(m_tableName)
            .arg(m_data.at(index.row()).moviePos+1)
            .arg(m_data.at(index.row()).movieID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
        {
            // Store the movie and delete it from the movies list.
            beginRemoveRows(QModelIndex(), index.row(), index.row());
            endRemoveRows();
            // Move the item ot the new position.
            m_data.move(index.row(), index.row()+1);
            beginInsertRows(QModelIndex(), index.row()+1, index.row()+1);
            endInsertRows();
            emit listEdited();

            // Store the index of the moved intem into a list to be selected in the view.
            selectedIndex.append(QItemSelectionRange(
                this->index(index.row()+1, 0),
                this->index(index.row()+1, NUMBER_MOVIES_TABLE_COLUMN_COUNT)));

            updateMoviePos(index.row());
            m_query.clear();
        }
        else
        {
            std::cerr << QString("Error: failed to move up items in the table %1.\n\t%2")
                .arg(m_tableName, m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            m_query.clear();
        }
    }

    return selectedIndex;
}

QItemSelection TableModelMovies::moveItemsTo(const QModelIndexList& indexList, int to)
{
    // Move the selected items from the view to the position (to).
    // Sorting the list to be able to remove items from the highest index to the lowest index. 
    QModelIndexList indexListCpy(indexList);
    std::sort(indexListCpy.begin(), indexListCpy.end(),
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() < index2.row();
        });
    
    // Removing items and add the valid index to the movingMovies list.
    QList<MovieItem> movingMovies;
    for (int i = indexListCpy.size()-1; i >= 0; i--)
    {
        if (indexListCpy.at(i).row() < 0 || indexListCpy.at(i).row() >= size() ||
            indexListCpy.at(i).column() < 0 || indexListCpy.at(i).column() >= columnCount())
            continue;
        
        movingMovies.prepend(m_data.at(indexListCpy.at(i).row()));
        beginRemoveRows(QModelIndex(), indexListCpy.at(i).row(), indexListCpy.at(i).row());
        m_data.remove(indexListCpy.at(i).row(), 1);
        endRemoveRows();
    }

    // The moved items will be store in this list to update the selection model of the view.
    QItemSelection selectedIndex;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   MoviePos = %2\n"
        "WHERE MovieID = %3;");
    
    // Moving items
    int i = to;
    for (MovieItem& item: movingMovies)
    {
        item.moviePos = i;

        QString statement = baseStatement
            .arg(m_tableName)
            .arg(i)
            .arg(item.movieID);
    
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
        {
            m_data.insert(i, item);
            i++;
            m_query.clear();
        }
        else
        {
            std::cerr << QString("Error: failed to replace items of table %1.\n\t%2")
                .arg(m_tableName, m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl << std::endl;
            m_query.clear();
        }
    }

    // If any items has been moved, update the view.
    if (i > to)
    {
        beginInsertRows(QModelIndex(), to, i-1);
        endInsertRows();
        if (indexListCpy.at(0).row() < to)
            updateMoviePos(indexListCpy.at(0).row());
        else
            updateMoviePos(to);
        selectedIndex.append(QItemSelectionRange(
            index(to, 0),
            index(i-1, NUMBER_MOVIES_TABLE_COLUMN_COUNT)));
        emit listEdited();
    }

    // Return the list of the moved index.
    return selectedIndex;
}

void TableModelMovies::sortUtility(int column)
{
    // Sorting movies by there utilities.
    if (column == Movie::NAME | column == Movie::RATE)
        return;
    
    auto compareUtil =
        [this] (const QString& str1, const QString& str2) -> bool
        {
            if (this->m_sortingOrder == Qt::AscendingOrder)
                return str1.compare(str2) < 0;
            else
                return str1.compare(str2) > 0;
        };
    
    auto compareSens =
        [this] (const SensitiveContent& data1, const SensitiveContent& data2) -> bool
        {
            if (this->m_sortingOrder == Qt::AscendingOrder)
            {
                if (data1.explicitContent < data2.explicitContent)
                    return true;
                else if (data1.explicitContent == data2.explicitContent)
                {
                    if (data1.violenceContent < data2.violenceContent)
                        return true;
                    else if (data1.violenceContent == data2.violenceContent)
                    {
                        if (data1.badLanguageContent < data2.badLanguageContent)
                            return true;
                    }
                }
            }
            else
            {
                if (data1.explicitContent > data2.explicitContent)
                    return true;
                else if (data1.explicitContent == data2.explicitContent)
                {
                    if (data1.violenceContent > data2.violenceContent)
                        return true;
                    else if (data1.violenceContent == data2.violenceContent)
                    {
                        if (data1.badLanguageContent > data2.badLanguageContent)
                            return true;
                    }
                }
            }

            return false;
        };

    auto sortTemplate =
        [column, compareUtil, compareSens] (const MovieItem& item1, const MovieItem& item2) -> bool
        {
            if (column == Movie::CATEGORIES)
                return compareUtil(item1.categories, item2.categories);
            else if (column == Movie::DIRECTORS)
                return compareUtil(item1.directors, item2.directors);
            else if (column == Movie::ACTORS)
                return compareUtil(item1.actors, item2.actors);
            else if (column == Movie::PRODUCTIONS)
                return compareUtil(item1.productions, item2.productions);
            else if (column == Movie::MUSIC)
                return compareUtil(item1.music, item2.music);
            else if (column == Movie::SERVICES)
                return compareUtil(item1.services, item2.services);
            else if (column == Movie::SENSITIVE_CONTENT)
                return compareSens(item1.sensitiveContent, item2.sensitiveContent);
            return false;
        };
    
    std::stable_sort(m_data.begin(), m_data.end(), sortTemplate);
}