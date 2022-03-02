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

#include "TableModelSeries.h"
#include "TableModelSeries_UtilityInterface.h"
#include <QSqlError>
#include <iostream>
#include <algorithm>

#define SERIES_TABLE_COUNT 11
#define NUMBER_SERIES_TABLE_COLUMN_COUNT 10

template<typename T>
bool TableModelSeries::updateField(const QString& columnName, int rowNB, T value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = %3\n"
        "WHERE\n"
        "   SeriesID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_data.at(rowNB).serieID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to update field %1 of SerieID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).serieID)
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
bool TableModelSeries::updateField(const QString& columnName, int rowNB, const QString& value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = \"%3\"\n"
        "WHERE\n"
        "   SeriesID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_data.at(rowNB).serieID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to update field %1 of SerieID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).serieID)
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

TableModelSeries::TableModelSeries(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(tableName, db, utilityTable, parent),
    m_interface(nullptr)
{
    createTable();
    m_interface = new TableModelSeries_UtilityInterface(rawTableName(), db);
    connect(m_interface, &TableModelSeries_UtilityInterface::interfaceChanged, this, &TableModelSeries::utilityChanged);
    connect(m_interface, &TableModelSeries_UtilityInterface::interfaceChanged, this, &TableModelSeries::listEdited);
}

TableModelSeries::TableModelSeries(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(db, utilityTable, parent),
    m_interface(nullptr)
{
    setItemData(data);
    connect(m_interface, &TableModelSeries_UtilityInterface::interfaceChanged, this, &TableModelSeries::utilityChanged);
    connect(m_interface, &TableModelSeries_UtilityInterface::interfaceChanged, this, &TableModelSeries::listEdited);
}

TableModelSeries::~TableModelSeries()
{
    deleteTable();
    if (m_interface)
        delete m_interface;
}

int TableModelSeries::columnCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return SERIES_TABLE_COUNT;
    else
        return 0;
}

int TableModelSeries::rowCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return m_data.size();
    else
        return 0;
}

QVariant TableModelSeries::data(const QModelIndex& index, int role) const
{
    // Returning the series table into the view.
    if ((role == Qt::EditRole || role == Qt::DisplayRole) && m_isTableCreated)
    {
        if (index.column() >= 0 && index.column() < columnCount() &&
            index.row() >= 0 && index.row() < rowCount())
        {
            // Returning to the tableView the data from the list.
            switch (index.column())
            {
            case Series::NAME:
                return m_data.at(index.row()).name;
            case Series::EPISODE:
                return m_data.at(index.row()).episodePos;
            case Series::SEASON:
                return m_data.at(index.row()).seasonPos;
            case Series::CATEGORIES:
                return m_data.at(index.row()).categories;
            case Series::DIRECTORS:
                return m_data.at(index.row()).directors;
            case Series::ACTORS:
                return m_data.at(index.row()).actors;
            case Series::PRODUCTION:
                return m_data.at(index.row()).production;
            case Series::MUSIC:
                return m_data.at(index.row()).music;
            case Series::SERVICES:
                return m_data.at(index.row()).services;
            case Series::SENSITIVE_CONTENT:
                return QVariant::fromValue(m_data.at(index.row()).sensitiveContent);
            case Series::RATE:
                return m_data.at(index.row()).rate;
            }
        }
    }

    return QVariant();
}

bool TableModelSeries::setData(const QModelIndex& index, const QVariant& data, int role)
{
    // Setting the field data.
    // Each column is different, so we working on each column independently.
    if (role == Qt::EditRole && m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
    {
        switch (index.column())
        {
        case Series::NAME:
        {
            if (data.canConvert<QString>())
            {
                QString serieName = data.toString();
                m_data[index.row()].name = serieName;

                bool result = updateField<const QString&>("Name", index.row(), serieName);
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
        case Series::EPISODE:
        {
            if (data.canConvert<int>())
            {
                int episodePos = data.toInt();
                m_data[index.row()].episodePos = episodePos;

                bool result = updateField<int>("Episode", index.row(), episodePos);
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
        case Series::SEASON:
        {
            if (data.canConvert<int>())
            {
                int seasonPos = data.toInt();
                m_data[index.row()].seasonPos = seasonPos;

                bool result = updateField<int>("Season", index.row(), seasonPos);
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
        case Series::RATE:
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

bool TableModelSeries::insertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting new rows into the table.
    if (row >= 0 && row <= rowCount() &&
        count > 0 && m_isTableCreated)
    {
        // BooksPos Statement
        QString posStatement = QString(
            "SELECT\n"
            "   MAX(SeriesPos)\n"
            "FROM\n"
            "   \"%1\";").arg(m_tableName);

#ifndef NDEBUG
        std::cout << posStatement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

        int maxPos;
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
            "   SeriesPos,\n"
            "   Name,\n"
            "   Episode,\n"
            "   Season,\n"
            "   Url,\n"
            "   Rate )\n"
            "VALUES")
                .arg(m_tableName);

        for (int i = 0; i < count; i++)
        {
            statement += QString(
                "\n   (%1, \"New Serie\", NULL, NULL, NULL, NULL),")
                .arg(++maxPos);
        }
        statement[statement.size() - 1] = ';';

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        // Then, querying the new inserted data and add it to the m_listData
        if (m_query.exec(statement))
        {
            m_query.clear();
            statement = QString(
                "SELECT\n"
                "   SeriesID,\n"
                "   SeriesPos,\n"
                "   Name,\n"
                "   Episode,\n"
                "   Season,\n"
                "   Url,\n"
                "   Rate\n"
                "FROM\n"
                "   \"%1\"\n"
                "ORDER BY\n"
                "   SeriesID DESC\n"
                "LIMIT\n"
                "   %2;")
                    .arg(m_tableName)
                    .arg(count);
                
#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

            if (m_query.exec(statement))
            {
                if (count == 1)
                    beginInsertRows(QModelIndex(), rowCount(), rowCount());
                else
                    beginInsertRows(QModelIndex(), rowCount(), rowCount() + (count - 1));

                QList<SeriesItem> seriesList;
                while(m_query.next())
                {
                    SeriesItem serie = {};
                    serie.serieID = m_query.value(0).toLongLong();
                    serie.seriePos = m_query.value(1).toLongLong();
                    serie.name = m_query.value(2).toString();
                    serie.episodePos = m_query.value(3).toInt();
                    serie.seasonPos = m_query.value(4).toInt();
                    serie.url = m_query.value(5).toString();
                    serie.rate = m_query.value(6).toInt();
                    seriesList.prepend(serie);
                }
                m_data.append(seriesList.cbegin(), seriesList.cend());

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
                .arg(m_query.lastError().text()).toLocal8Bit().constData() << std::endl;
#endif
            
            return false;
        }
    }

    return true;
}

bool TableModelSeries::removeRows(int row, int count, const QModelIndex& parent)
{
    // Removing rows from the table.
    if (row >= 0 && row < rowCount() &&
        row + (count - 1) < rowCount() && m_isTableCreated)
    {
        // Defining the SQL statement.
        QString statement  = QString(
            "DELETE FROM \"%1\"\n"
            "WHERE SeriesID ")
            .arg(m_tableName);
        QList<long long int> itemsID(count);
        
        if (count == 1)
        {
            statement += QString("= %1;")
                .arg(m_data.at(row).serieID);
                itemsID[0] = m_data.at(row).serieID;
        }
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ", ";
                statement += QString::number(m_data.at(row+i).serieID);
                itemsID[i] = m_data.at(row+i).serieID;
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

Qt::ItemFlags TableModelSeries::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant TableModelSeries::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        // Setting the header's name.
        switch (section)
        {
        case Series::NAME:
            return "Name";
        case Series::EPISODE:
            return "Episode";
        case Series::SEASON:
            return "Season";
        case Series::CATEGORIES:
            return "Categories";
        case Series::DIRECTORS:
            return "Directors";
        case Series::ACTORS:
            return "Actors";
        case Series::PRODUCTION:
            return "Production";
        case Series::MUSIC:
            return "Music";
        case Series::SERVICES:
            return "Services";
        case Series::SENSITIVE_CONTENT:
            return "Sensitive Content";
        case Series::RATE:
            return "Rate";
        }
    }

    return QVariant();
}

void TableModelSeries::appendRows(int count)
{
    if (count > 0 && m_isTableCreated)
        insertRows(rowCount(), count);
}

void TableModelSeries::deleteRows(const QModelIndexList& indexList)
{
    // Used to delete selected rows in the view.
    QModelIndexList indexListCpy(indexList);
    std::sort(indexListCpy.begin(), indexListCpy.end(),
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() > index2.row();
        });
    
    for (QModelIndexList::const_iterator it = indexListCpy.cbegin();
        it != indexListCpy.cend();
        it++)
        removeRow((*it).row(), QModelIndex());
    
    if (indexList.size() > 0)
        emit listEdited();
}

int TableModelSeries::size() const
{
    return rowCount();
}

ListType TableModelSeries::listType() const
{
    return ListType::SERIESLIST;
}

long long int TableModelSeries::itemID(const QModelIndex& index) const
{
    if (m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
        return m_data.at(index.row()).serieID;
    return -1;
}

void TableModelSeries::updateQuery()
{
    // Retrieve the entrire series data of the table and 
    // and put it into the view.
    if (size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, size()-1);
        endRemoveRows();
    }

    // Getting the new data from the table.
    QString statement = QString(
        "SELECT\n"
        "   SeriesID,\n"
        "   SeriesPos,\n"
        "   \"%1\".Name as sName,\n"
        "   Episode,\n"
        "   Season,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "%4"
        "ORDER BY\n"
        "   %2 %3;")
            .arg(m_tableName);
    
    // Sorting view.
    if (m_sortingColumnID == Series::NAME)
    {
        statement = statement.arg("sName");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::EPISODE)
    {
        statement = statement.arg("Episode");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::SEASON)
    {
        statement = statement.arg("Season");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::RATE)
    {
        statement = statement.arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg("SeriesPos").arg("ASC");

    // Filtering the view.
    if (m_listFilter.column == Series::NAME)
    {
        QString where = QString(
            "WHERE\n"
            "   sName LIKE \"%%1%\"\n")
            .arg(m_listFilter.pattern);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column >= Series::CATEGORIES &&
        m_listFilter.column <= Series::SERVICES)
    {
        UtilityTableName tName;
        if (m_listFilter.column == Series::CATEGORIES)
            tName = UtilityTableName::CATEGORIES;
        else if (m_listFilter.column == Series::DIRECTORS)
            tName = UtilityTableName::DIRECTOR;
        else if (m_listFilter.column == Series::ACTORS)
            tName = UtilityTableName::ACTORS;
        else if (m_listFilter.column == Series::PRODUCTION)
            tName = UtilityTableName::PRODUCTION;
        else if (m_listFilter.column == Series::MUSIC)
            tName = UtilityTableName::MUSIC;
        else if (m_listFilter.column == Series::SERVICES)
            tName = UtilityTableName::SERVICES;

        QString where = QString(
            "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
            "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".SeriesID\n"
            "WHERE\n"
            "   \"%2\".\"%2ID\" IN (%4)\n"
            "GROUP BY\n"
            "   SeriesID\n")
            .arg(m_tableName, m_utilityTable.tableName(tName), m_interface->tableName(tName));
        
        QString utilList;
        for (int i = 0; i < m_listFilter.utilityList.size(); i++)
        {
            if (i > 0)
                utilList += ',';
            
            utilList += QString::number(m_listFilter.utilityList.at(i));
        }
        where = where.arg(utilList);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column == Series::RATE)
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
    std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

    if(m_query.exec(statement))
    {
        // Putting the queried data into the QList.
        m_data.clear();
        
        while (m_query.next())
        {
            SeriesItem serie = {};
            serie.serieID = m_query.value(0).toLongLong();
            serie.seriePos = m_query.value(1).toLongLong();
            serie.name = m_query.value(2).toString();
            serie.episodePos = m_query.value(3).toInt();
            serie.seasonPos = m_query.value(4).toInt();
            serie.url = m_query.value(5).toString();
            serie.rate = m_query.value(6).toInt();
            m_data.append(serie);
        }

        if (size() > 0)
        {
            // Quering the util table and set it into the specifics rows.
            queryCategoriesField();
            queryDirectorsField();
            queryActorsField();
            queryProductionField();
            queryMusicField();
            queryServicesField();
            querySensitiveContentField();

            if (m_sortingColumnID >= Series::CATEGORIES && m_sortingColumnID <= Series::SENSITIVE_CONTENT)
                sortUtility(m_sortingColumnID);

            beginInsertRows(QModelIndex(), 0, size()-1);
            endInsertRows();
        }
    }
#ifndef NDEBUG
    else
        std::cerr << "Failed to update cell values of table " << m_tableName.toLocal8Bit().constData() << "\n\t"
            << m_query.lastError().text().toLocal8Bit().constData() << std::endl;
#endif
}

QVariant TableModelSeries::retrieveData() const
{
    // Return the data of the table and the utility interface data.
    Series::SaveDataTable data = {};
    data.tableName = m_tableName;

    // Retrieve series data.
    QSqlQuery query(m_db);

    QString statement = QString(
        "SELECT\n"
        "   SeriesID,\n"
        "   SeriesPos,\n"
        "   Name,\n"
        "   Episode,\n"
        "   Season,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   SeriesID ASC;")
            .arg(m_tableName);
    
    if (!query.exec(statement))
        return QVariant();
    
    while (query.next())
    {
        Series::SaveItem serie = {};
        serie.serieID = query.value(0).toLongLong();
        serie.seriePos = query.value(1).toLongLong();
        serie.name = query.value(2).toString();
        serie.episodePos = query.value(3).toInt();
        serie.seasonPos = query.value(4).toInt();
        serie.url = query.value(5).toString();
        serie.rate = query.value(6).toInt();
        data.serieList.append(serie);
    }

    QVariant utilityInterface = m_interface->data();
    if (!utilityInterface.canConvert<Series::SaveUtilityInterfaceData>())
        return QVariant();
    
    data.interface = qvariant_cast<Series::SaveUtilityInterfaceData>(utilityInterface);

    data.columnSort = (signed char)m_sortingColumnID;
    data.sortOrder = (unsigned char)(m_sortingOrder == Qt::AscendingOrder ? 0 : 1);

    return QVariant::fromValue(data);
}

bool TableModelSeries::setItemData(const QVariant& variant)
{
    // Set the data into the TableModel SQL Tables.
    Series::SaveDataTable data = qvariant_cast<Series::SaveDataTable>(variant);

    // Set the table name and create the SQL tables.
    m_tableName = data.tableName;
    if (m_tableName.isEmpty())
        return false;
    createTable();

    // Set the series list.
    QString statement = QString(
        "INSERT INTO \"%1\" (SeriesID, SeriesPos, Name, Episode, Season, Url, Rate)\n"
        "VALUES")
        .arg(m_tableName);
    
    for (long long int i = 0; i < data.serieList.size(); i+=10)
    {
        QString strData;
        for (long long int j = i; j < i+10 && j < data.serieList.size(); j++)
        {
            strData +=
                QString("\n\t(%1, %2, \"%3\", %4, %5, \"%6\", %7),")
                    .arg(data.serieList.at(j).serieID)
                    .arg(data.serieList.at(j).seriePos)
                    .arg(data.serieList.at(j).name)
                    .arg(data.serieList.at(j).episodePos)
                    .arg(data.serieList.at(j).seasonPos)
                    .arg(data.serieList.at(j).url)
                    .arg(data.serieList.at(j).rate);
        }
        if (strData.size() > 0)
        {
            strData[strData.size()-1] = ';';

#ifndef NDEBUG
            std::cout << (statement + strData).toLocal8Bit().constData() << "\n" << std::endl;
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
    m_interface = new TableModelSeries_UtilityInterface(m_tableName, m_db, QVariant::fromValue(data.interface));
    if (!m_interface->isTableReady())
    {
        m_isTableCreated = false;
        return false;
    }

    // Then, query the whole table.
    updateQuery();

    return true;
}

void TableModelSeries::createTable()
{
    // Creating the Series SQL table.
    QString statement = QString(
        "CREATE TABLE \"%1\" (\n"
        "   SeriesID INTEGER PRIMARY KEY,\n"
        "   SeriesPos INTEGER,\n"
        "   Name TEXT,\n"
        "   Episode INTEGER,\n"
        "   Season INTEGER,\n"
        "   SensitiveContent INTEGER,\n"
        "   Url TEXT,\n"
        "   Rate INTEGER);")
            .arg(m_tableName);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

    if (m_query.exec(statement))
        m_isTableCreated = true;
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to create the table %1.\n\t%2")
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
    m_query.clear();
}

void TableModelSeries::deleteTable()
{
    // Delete the SQL table.
    if (!m_isTableCreated)
        return;
    
    QString statement = QString(
        "DROP TABLE IF EXISTS \"%1\";")
            .arg(m_tableName);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
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

void TableModelSeries::utilityChanged(long long int serieID, UtilityTableName tableName)
{
    // This member function is called when the utility interface is changed.
    if (serieID >= 0 && size() > 0  && m_isTableCreated)
    {
        if (tableName == UtilityTableName::CATEGORIES)
            queryCategoriesField(serieID);
        else if (tableName == UtilityTableName::DIRECTOR)
            queryDirectorsField(serieID);
        else if (tableName == UtilityTableName::ACTORS)
            queryActorsField(serieID);
        else if (tableName == UtilityTableName::PRODUCTION)
            queryProductionField(serieID);
        else if (tableName == UtilityTableName::MUSIC)
            queryMusicField(serieID);
        else if (tableName == UtilityTableName::SERVICES)
            queryServicesField(serieID);
        else if (tableName == UtilityTableName::SENSITIVE_CONTENT)
            querySensitiveContentField(serieID);
    }
}

void TableModelSeries::queryUtilityField(UtilityTableName tableName)
{
    // Standard interface to query the utility data except the sensitive data.
    if (!m_isTableCreated)
        return;

    QString statement = QString(
        "SELECT\n"
        "   \"%1\".SeriesID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".\"UtilityID\"\n"
        "INNER JOIN \"%3\" ON \"%3\".\"ItemID\" = \"%1\".SeriesID\n"
        "%6"
        "GROUP BY\n"
        "   \"%1\".SeriesID\n"
        "ORDER BY\n"
        "   \"%1\".%4 %5;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName));
    
    // Sorting order.
    if (m_sortingColumnID == Series::NAME)
    {
        statement = statement.arg("Name");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::EPISODE)
    {
        statement = statement.arg("Episode");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::SEASON)
    {
        statement = statement.arg("Season");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::RATE)
    {
        statement = statement.arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg("SeriesPos").arg("ASC");

    // Filtering the view.
    if (m_listFilter.column == Series::NAME)
    {
        QString where = QString(
            "WHERE\n"
            "   \"%1\".Name LIKE \"%%2%\"\n")
            .arg(m_tableName, m_listFilter.pattern);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column >= Series::CATEGORIES &&
        m_listFilter.column <= Series::SERVICES)
    {
        UtilityTableName tName;
        if (m_listFilter.column == Series::CATEGORIES)
            tName = UtilityTableName::CATEGORIES;
        else if (m_listFilter.column == Series::DIRECTORS)
            tName = UtilityTableName::DIRECTOR;
        else if (m_listFilter.column == Series::ACTORS)
            tName = UtilityTableName::ACTORS;
        else if (m_listFilter.column == Series::PRODUCTION)
            tName = UtilityTableName::PRODUCTION;
        else if (m_listFilter.column == Series::MUSIC)
            tName = UtilityTableName::MUSIC;
        else if (m_listFilter.column == Series::SERVICES)
            tName = UtilityTableName::SERVICES;
        
        QString seriesID;
        for (int i = 0; i < m_data.size(); i++)
        {
            if (i > 0)
                seriesID += ',';
            seriesID += QString::number(m_data.at(i).serieID);
        }
        
        QString where = QString(
            "WHERE\n"
            "   \"%1\".SeriesID IN (%2)\n")
            .arg(m_tableName, seriesID);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column == Series::RATE)
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
    std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Apply the queried data into model data.
        while (m_query.next())
        {
            long long int serieID = m_query.value(0).toLongLong();
            QString utilityName = m_query.value(1).toString();

            for (int i = 0; i < rowCount(); i++)
            {
                if (m_data.at(i).serieID == serieID)
                {
                    if (tableName == UtilityTableName::CATEGORIES)
                        m_data[i].categories = utilityName;
                    else if (tableName == UtilityTableName::DIRECTOR)
                        m_data[i].directors = utilityName;
                    else if (tableName == UtilityTableName::ACTORS)
                        m_data[i].actors = utilityName;
                    else if (tableName == UtilityTableName::PRODUCTION)
                        m_data[i].production = utilityName;
                    else if (tableName == UtilityTableName::MUSIC)
                        m_data[i].music = utilityName;
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
        std::cerr << QString("Failed to query Utilities of the %1 table.\n\t%2")
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

void TableModelSeries::queryUtilityField(UtilityTableName tableName, long long int serieID)
{
    // Stardard interface to update the utility field of the serie (serieID).
    if (!m_isTableCreated)
        return;

    QString statement = QString(
        "SELECT\n"
        "   \"%1\".SeriesID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
        "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".SeriesID\n"
        "WHERE\n"
        "   \"%1\".SeriesID = %4;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName))
            .arg(serieID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findSeriePos(serieID);
        if (pos >= 0 && pos <= rowCount())
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
                m_data[pos].production = utilityName;
            else if (tableName == UtilityTableName::MUSIC)
                m_data[pos].music = utilityName;
            else if (tableName == UtilityTableName::SERVICES)
                m_data[pos].services = utilityName;
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Utilities of the serie %1 in the table %2.\n\t%3")
            .arg(serieID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

int TableModelSeries::findSeriePos(long long int serieID) const
{
    for (int i = 0; i < size(); i++)
        if (m_data.at(i).serieID == serieID)
            return i;
    return -1;
}

void TableModelSeries::queryCategoriesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES);
}

void TableModelSeries::queryCategoriesField(long long int serieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES, serieID);
}

void TableModelSeries::queryDirectorsField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::DIRECTOR);
}

void TableModelSeries::queryDirectorsField(long long int serieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::DIRECTOR, serieID);
}

void TableModelSeries::queryActorsField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::ACTORS);
}

void TableModelSeries::queryActorsField(long long int serieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::ACTORS, serieID);
}

void TableModelSeries::queryProductionField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PRODUCTION);
}

void TableModelSeries::queryProductionField(long long int serieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::PRODUCTION, serieID);
}

void TableModelSeries::queryMusicField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::MUSIC);
}

void TableModelSeries::queryMusicField(long long int serieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::MUSIC, serieID);
}

void TableModelSeries::queryServicesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERVICES);
}

void TableModelSeries::queryServicesField(long long int serieID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERVICES, serieID);
}

void TableModelSeries::querySensitiveContentField()
{
    // Getting the Sensitive Content Information for each serie items.
    QString statement = QString(
        "SELECT\n"
        "   ItemID,\n"
        "   ExplicitContent,\n"
        "   ViolenceContent,\n"
        "   BadLanguage,\n"
        "   \"%2\".SeriesID\n"
        "FROM\n"
        "   \"%1\", \"%2\"\n"
        "ORDER BY\n"
        "   \"%3\".%4 %5;")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(m_tableName);

    // Sorting order
    if (m_sortingColumnID == Series::NAME)
    {
        statement = statement.arg(m_tableName).arg("Name");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::EPISODE)
    {
        statement = statement.arg(m_tableName).arg("Episode");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::SEASON)
    {
        statement = statement.arg(m_tableName).arg("Season");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Series::RATE)
    {
        statement = statement.arg(m_tableName).arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg(m_tableName).arg("SeriesPos").arg("ASC");
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

    if (m_query.exec(statement))
    {
        while (m_query.next())
        {
            long long int serieID = m_query.value(0).toLongLong();
            SensitiveContent sensData = {};
            sensData.explicitContent = m_query.value(1).toInt();
            sensData.violenceContent = m_query.value(2).toInt();
            sensData.badLanguageContent = m_query.value(3).toInt();

            for (int i = 0; i < size(); i++)
            {
                if (m_data.at(i).serieID == serieID)
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

void TableModelSeries::querySensitiveContentField(long long int serieID)
{
    // Updating the field sensitive content of the serie (serieID).
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
            .arg(serieID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findSeriePos(serieID);
        if (pos >= 0 && pos < rowCount() && m_query.next())
        {
            SensitiveContent sensData = {};
            sensData.explicitContent = m_query.value(1).toInt();
            sensData.violenceContent = m_query.value(2).toInt();
            sensData.badLanguageContent = m_query.value(3).toInt();
            m_data[pos].sensitiveContent = sensData;
            emit dataChanged(index(pos, Series::SENSITIVE_CONTENT), index(pos, Series::SENSITIVE_CONTENT));
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Sensitive Content of the serie item %1 in the table %2.\n\t%3")
            .arg(serieID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

QString TableModelSeries::url(const QModelIndex& index) const
{
    if (index.row() >= 0 && index.row() < size())
        return m_data.at(index.row()).url;
    return QString();
}

void TableModelSeries::setUrl(const QModelIndex& index, const QString& url)
{
    // Set the url of the serie item.
    if (index.isValid() && index.row() >= 0 && index.row() < size())
    {
        long long int serieID = m_data.at(index.row()).serieID;

        QString statement = QString(
            "UPDATE \"%1\"\n"
            "SET Url = \"%2\"\n"
            "WHERE SeriesID = %3;")
                .arg(m_tableName)
                .arg(url)
                .arg(m_data.at(index.row()).serieID);

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

        if (m_query.exec(statement))
        {
            m_data[index.row()].url = url;
            emit listEdited();
        }
#ifndef NDEBUG
        else
        {
            std::cerr << QString("Failed to set the url to the table \"%1\".\n\t%2")
                .arg(m_tableName, m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
        }
#endif
    }
}

TableModel_UtilityInterface* TableModelSeries::utilityInterface()
{
    if (m_isTableCreated)
        return m_interface;
    return nullptr;
}

void TableModelSeries::updateSeriesPos(int from)
{
    // Update the SeriesPos SQL column, used to apply order in the view.
    if (from < 0)
        from = 0;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET SeriesPos = %2\n"
        "WHERE SeriesID = %3;");
    
    for (int i = from; i < size(); i++)
    {
        if (m_data.at(i).seriePos != i)
        {
            QString statement = baseStatement
                .arg(m_tableName)
                .arg(i)
                .arg(m_data.at(i).seriePos);

#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

            if (m_query.exec(statement))
            {
                m_data[i].seriePos = i;
                m_query.clear();
            }
            else
            {
                std::cerr << QString("Failed to update book position of the serie %1 of the table %2.\n\t%3")
                    .arg(m_data.at(i).seriePos)
                    .arg(m_tableName)
                    .arg(m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
                m_query.clear();
            }
        }
    }
}

QItemSelection TableModelSeries::moveItemsUp(const QModelIndexList& indexList)
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
        "   SeriesPos = %2\n"
        "WHERE SeriesID = %3;");
    
    foreach (const QModelIndex& index, indexListCpy)
    {
        // If index is equal to 0, ignore
        if (index.row() == 0)
            continue;

        QString statement = baseStatement
            .arg(m_tableName)
            .arg(m_data.at(index.row()).seriePos-1)
            .arg(m_data.at(index.row()).serieID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;
#endif

        if (m_query.exec(statement))
        {
            // Store the serie item and delete it from the serie list.
            beginRemoveRows(QModelIndex(), index.row(), index.row());
            endRemoveRows();
            // Move the item to the new position
            m_data.move(index.row(), index.row()-1);
            beginInsertRows(QModelIndex(), index.row()-1, index.row()-1);
            endInsertRows();
            emit listEdited();

            // Store the index of the moved item into a list to be selected in the view.
            selectedIndex.append(QItemSelectionRange(
                this->index(index.row()-1, 0),
                this->index(index.row()-1, NUMBER_SERIES_TABLE_COLUMN_COUNT)));

            updateSeriesPos(index.row()-1);
            m_query.clear();
        }
        else
        {
            std::cerr << QString("Error: failed to move up items in the table %1.\n\t%2")
                .arg(m_tableName)
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            m_query.clear();
        }
    }

    return selectedIndex;
}

QItemSelection TableModelSeries::moveItemsDown(const QModelIndexList& indexList)
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
        "   SeriesPos = %2\n"
        "WHERE SeriesID = %3;");
    
    foreach (const QModelIndex& index, indexListCpy)
    {
        // If index is equal to size() - 1, ignore.
        if (index.row() == size()-1)
            continue;
        
        QString statement = baseStatement
            .arg(m_tableName)
            .arg(m_data.at(index.row()).seriePos+1)
            .arg(m_data.at(index.row()).serieID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << "\n" << std::endl;        // If index is equal to 0, ignore
#endif

        if (m_query.exec(statement))
        {
            beginRemoveRows(QModelIndex(), index.row(), index.row());
            endRemoveRows();
            // Move the item to the new position
            m_data.move(index.row(), index.row()+1);
            beginInsertRows(QModelIndex(), index.row()+1, index.row()+1);
            endInsertRows();
            emit listEdited();

            // Store the index of the moved item into a list to be selected in the view.
            selectedIndex.append(QItemSelectionRange(
                this->index(index.row()+1, 0),
                this->index(index.row()+1, NUMBER_SERIES_TABLE_COLUMN_COUNT)));

            updateSeriesPos(index.row());
            m_query.clear();
        }
        else
        {
            std::cerr << QString("Error: failed to move down items in the table %1.\n\t%2")
                .arg(m_tableName)
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            m_query.clear();
        }
    }

    return selectedIndex;
}

QItemSelection TableModelSeries::moveItemsTo(const QModelIndexList& indexList, int to)
{
    // Move the selected items from the view to the position (to).
    // Sorting the list to be able to remove items from the highest index to the lowest index.
    QModelIndexList indexListCpy(indexList);
    std::sort(indexListCpy.begin(), indexListCpy.end(), 
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() < index2.row();
        });

    // Removing items and add the valid index to the movingSeries list.
    QList<SeriesItem> movingSeries;
    for (int i = indexListCpy.size()-1; i >= 0; i--)
    {
        if (indexListCpy.at(i).row() < 0 || indexListCpy.at(i).row() >= size() ||
            indexListCpy.at(i).column() < 0 || indexListCpy.at(i).column() >= columnCount())
            continue;
        
        movingSeries.prepend(m_data.at(indexListCpy.at(i).row()));
        beginRemoveRows(QModelIndex(), indexListCpy.at(i).row(), indexListCpy.at(i).row());
        m_data.remove(indexListCpy.at(i).row(), 1);
        endRemoveRows();
    }

    // The moved items will be store in this list to update the selection model of the view.
    QItemSelection selectedIndex;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   SeriesPos = %2\n"
        "WHERE SeriesID = %3;");
    
    // Moving the items.
    int i = to;
    for (SeriesItem& item : movingSeries)
    {
        item.seriePos = i;

        QString statement = baseStatement
            .arg(m_tableName)
            .arg(i)
            .arg(item.seriePos);

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
                .arg(m_tableName)
                .arg(m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
            m_query.clear();
        }
    }

    // If any items has been moved, update the view.
    if (i > to)
    {
        beginInsertRows(QModelIndex(), to, i-1);
        endInsertRows();
        if (indexListCpy.at(0).row() < to)
            updateSeriesPos(indexListCpy.at(0).row());
        else
            updateSeriesPos(to);
        selectedIndex.append(QItemSelectionRange(
            index(to, 0),
            index(i-1, NUMBER_SERIES_TABLE_COLUMN_COUNT)));
        emit listEdited();
    }

    // Return the list of the moved index.
    return selectedIndex;
}

void TableModelSeries::sortUtility(int column)
{
    // Sorting series items by there utilities.
    if (column == Series::NAME || column == Series::RATE)
        return;

    auto compareString =
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
        [column, compareString, compareSens] (const SeriesItem& item1, const SeriesItem& item2) -> bool
        {
            if (column == Series::CATEGORIES)
                return compareString(item1.categories, item2.categories);
            else if (column == Series::DIRECTORS)
                return compareString(item1.directors, item2.directors);
            else if (column == Series::ACTORS)
                return compareString(item1.actors, item2.actors);
            else if (column == Series::PRODUCTION)
                return compareString(item1.production, item2.production);
            else if (column == Series::MUSIC)
                return compareString(item1.music, item2.music);
            else if (column == Series::SERVICES)
                return compareString(item1.services, item2.services);
            else if (column == Series::SENSITIVE_CONTENT)
                return compareSens(item1.sensitiveContent, item2.sensitiveContent);
            return false;
        };
    
    std::stable_sort(m_data.begin(), m_data.end(), sortTemplate);
}