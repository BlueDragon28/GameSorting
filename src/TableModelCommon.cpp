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

#include "TableModelCommon.h"
#include "TableModelCommon_UtilityInterface.h"
#include <QSqlError>
#include <QApplication>
#include <QClipboard>
#include <iostream>
#include <algorithm>

#define COMMON_TABLE_COUNT 6
#define NUMBER_COMMON_TABLE_COLUMN_COUNT 5

template<typename T>
bool TableModelCommon::updateField(const QString& columnName, int rowNB, T value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = %3\n"
        "WHERE\n"
        "   CommonID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_data.at(rowNB).commonID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to update field %1 of CommonID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).commonID)
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
bool TableModelCommon::updateField(const QString& columnName, int rowNB, const QString& value)
{
    // Helper member function to help update field on SQLite side.
    QString statement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   \"%2\" = \"%3\"\n"
        "WHERE\n"
        "   CommonID = %4;")
            .arg(m_tableName, columnName)
            .arg(value)
            .arg(m_data.at(rowNB).commonID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to update field %1 of GameID: %2 of the table %3.\n\t%4")
            .arg(columnName)
            .arg(m_data.at(rowNB).commonID)
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

TableModelCommon::TableModelCommon(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(tableName, db, utilityTable, parent),
    m_interface(nullptr)
{
    createTable();
    m_interface = new TableModelCommon_UtilityInterface(rawTableName(), m_db);
    connect(m_interface, &TableModelCommon_UtilityInterface::interfaceChanged, this, &TableModelCommon::utilityChanged);
    connect(m_interface, &TableModelCommon_UtilityInterface::interfaceChanged, this, &TableModelCommon::listEdited);
}

TableModelCommon::TableModelCommon(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    TableModel(db, utilityTable, parent),
    m_interface(nullptr)
{
    setItemData(data);
    connect(m_interface, &TableModelCommon_UtilityInterface::interfaceChanged, this, &TableModelCommon::utilityChanged);
    connect(m_interface, &TableModelCommon_UtilityInterface::interfaceChanged, this, &TableModelCommon::listEdited);
}

TableModelCommon::~TableModelCommon()
{
    deleteTable();
    if (m_interface)
        delete m_interface;
}

int TableModelCommon::columnCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return COMMON_TABLE_COUNT;
    else
        return 0;
}

int TableModelCommon::rowCount(const QModelIndex& parent) const
{
    if (m_isTableCreated)
        return m_data.size();
    else
        return 0;
}

QVariant TableModelCommon::data(const QModelIndex& index, int role) const
{
    // Returning the common table into the view.
    if ((role == Qt::EditRole || role == Qt::DisplayRole) && m_isTableCreated)
    {
        if (index.column() >= 0 && index.column() < columnCount() &&
            index.row() >= 0 && index.row() < rowCount())
        {
            // Returning to the tableView the data from the list.
            switch (index.column())
            {
            case Common::NAME:  
                return m_data.at(index.row()).name;
            case Common::SERIES:
                return m_data.at(index.row()).series;
            case Common::CATEGORIES:
                return m_data.at(index.row()).categories;
            case Common::AUTHORS:
                return m_data.at(index.row()).authors;
            case Common::SENSITIVE_CONTENT:
                return QVariant::fromValue(m_data.at(index.row()).sensitiveContent);
            case Common::RATE:
                return m_data.at(index.row()).rate;
            }
        }
    }
    
    return QVariant();
}

bool TableModelCommon::setData(const QModelIndex& index, const QVariant& data, int role)
{
    // Setting the field data.
    // Each column is different, so we working on each column independently.
    if (role == Qt::EditRole && m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
    {
        switch (index.column())
        {
        case Common::NAME:
        {
            if (data.canConvert<QString>())
            {
                QString commonName = data.toString();
                m_data[index.row()].name = commonName;

                bool result = updateField<const QString&>("Name", index.row(), commonName);
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

        case Common::RATE:
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
        }
        }
    }

    return true;
}

bool TableModelCommon::insertRows(int row, int count, const QModelIndex& parent)
{
    // Inserting new rows into the table.
    if (row >= 0 && row <= rowCount() &&
        count > 0 && m_isTableCreated)
    {
        int commonPos;
        if (m_sortingColumnID >= 0)
            // Retrieve max pos.
            commonPos = retrieveMaxPos()+1;
        else
            // Insert the row where the user want it if sorting is not enable.
            commonPos = row;

        // Executing the sql statement for inserting new rows.
        QString statement = QString(
            "INSERT INTO \"%1\" (\n"
            "   CommonPos,\n"
            "   Name,\n"
            "   Url,\n"
            "   Rate )\n"
            "VALUES")
                .arg(m_tableName);

        for (int i = 0; i < count; i++)
        {
            statement += QString(
                "\n   (%1, \"New Common\", NULL, NULL),")
                .arg(commonPos++);
        }
        statement[statement.size() - 1] = ';';

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        // Then, querying the new inserted data and add it to the m_listData
        if (m_query.exec(statement))
        {
            m_query.clear();
            retrieveInsertedRows(row, count);
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

bool TableModelCommon::removeRows(int row, int count, const QModelIndex& parent)
{
        // Removing rows from the table.
    if (row >= 0 && row < rowCount() &&
        row + (count - 1) < rowCount() && m_isTableCreated)
    {
        // Defining the SQL statement.
        QString statement  = QString(
            "DELETE FROM \"%1\"\n"
            "WHERE CommonID ")
            .arg(m_tableName);
        QList<long long int> itemsID(count);
        
        if (count == 1)
        {
            statement += QString("= %1;")
                .arg(m_data.at(row).commonID);
                itemsID[0] = m_data.at(row).commonID;
        }
        else
        {
            statement += "IN (";
            for (int i = 0; i < count; i++)
            {
                if (i > 0)
                    statement += ", ";
                statement += QString::number(m_data.at(row+i).commonID);
                itemsID[i] = m_data.at(row+i).commonID;
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

Qt::ItemFlags TableModelCommon::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant TableModelCommon::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        // Setting the headers' name.
        switch (section)
        {
        case Common::NAME:
            return "Name";
        case Common::SERIES:
            return "Series";
        case Common::CATEGORIES:
            return "Categories";
        case Common::AUTHORS:
            return "Authors";
        case Common::SENSITIVE_CONTENT: 
            return "Sensitive Content";
        case Common::RATE:
            return "Rate";
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void TableModelCommon::appendRows(int count)
{
    appendRows(QModelIndexList(), count);
}

void TableModelCommon::appendRows(const QModelIndexList& indexList, int count)
{
    QModelIndexList indexListCopy(indexList);
    if (!indexListCopy.isEmpty() && m_sortingColumnID < 0)
    {
        std::sort(indexListCopy.begin(), indexListCopy.end(),
            [](const QModelIndex& index1, const QModelIndex& index2) -> bool
            {
                return index1.row() > index2.row();
            });
    }

    if (count > 0 && m_isTableCreated)
    {
        if (indexListCopy.isEmpty() || m_sortingColumnID >= 0)
            insertRows(rowCount(), count);
        else
            insertRows(indexListCopy.at(0).row()+1, count);
    }
}

void TableModelCommon::appendRows(const QModelIndexList& indexList, const QStringList& commonList)
{
    // Insert list (commonList) into the common list.
    if (commonList.isEmpty())
        return;
    
    // Sorting the selected index with higher number first.
    QModelIndexList indexListCopy(indexList);
    if (!indexListCopy.isEmpty() && m_sortingColumnID < 0)
    {
        std::sort(indexListCopy.begin(), indexListCopy.end(),
            [](const QModelIndex& index1, const QModelIndex& index2) -> bool
            {
                return index1.row() > index2.row();
            });
    }

    // Choose where to add the common item.
    int commonPos;
    if (indexListCopy.isEmpty() || m_sortingColumnID >= 0)
        commonPos = retrieveMaxPos()+1;
    else
        commonPos = indexListCopy.at(0).row()+1;
    
    if (commonPos > rowCount())
        commonPos = rowCount();
    
    if (commonPos < 0 || commonPos > rowCount())
        return;
    
    // Insert the common item.
    QString statement = QString(
        "INSERT INTO \"%1\" (\n"
        "   CommonPos,\n"
        "   Name,\n"
        "   Url,\n"
        "   Rate )\n"
        "VALUES")
            .arg(m_tableName);

    for (int i = 0; i < commonList.size(); i++)
    {
        statement += QString(
            "\n   (%1, \"%2\", NULL, NULL),")
            .arg(commonPos+1).arg(commonList.at(i));
    }
    statement[statement.size() - 1] = ';';

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << '\n' << std::endl;
#endif

    if (m_query.exec(statement))
    {
        m_query.clear();
        retrieveInsertedRows(commonPos, commonList.size());
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to insert row of table %1\n\t%2")
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData() << '\n' << std::endl;
#endif
}

void TableModelCommon::deleteRows(const QModelIndexList& indexList)
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

int TableModelCommon::size() const
{
    return rowCount();
}

ListType TableModelCommon::listType() const
{
    return ListType::COMMONLIST;
}

long long int TableModelCommon::itemID(const QModelIndex& index) const
{
    if (m_isTableCreated &&
        index.column() >= 0 && index.column() < columnCount() &&
        index.row() >= 0 && index.row() < rowCount())
        return m_data.at(index.row()).commonID;
    return -1;
}

void TableModelCommon::updateQuery()
{
    // Retrieve the entrire common data of the table and 
    // and put it into the view.
    if (size() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, size()-1);
        endRemoveRows();
    }

    // Getting the new data from the table.
    QString statement = QString(
        "SELECT\n"
        "   CommonID,\n"
        "   CommonPos,\n"
        "   \"%1\".Name as cName,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "%4"
        "ORDER BY\n"
        "   %2 %3;")
            .arg(m_tableName);
    
    // Sorting view.
    if (m_sortingColumnID == Common::NAME)
    {
        statement = statement.arg("cName");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Common::RATE)
    {
        statement = statement.arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg("CommonPos").arg("ASC");

    // Filtering the view.
    if (m_listFilter.column == Game::NAME)
    {
        QString where = QString(
            "WHERE\n"
            "   cName LIKE \"%%1%\"\n")
            .arg(m_listFilter.pattern);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column >= Common::SERIES &&
        m_listFilter.column <= Common::AUTHORS)
    {
        UtilityTableName tName;
        if (m_listFilter.column == Common::SERIES)
            tName = UtilityTableName::SERIES;
        else if (m_listFilter.column == Common::CATEGORIES)
            tName = UtilityTableName::CATEGORIES;
        else if (m_listFilter.column == Common::AUTHORS)
            tName = UtilityTableName::AUTHORS;

        QString where = QString(
            "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
            "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".CommonID\n"
            "WHERE\n"
            "   \"%2\".\"%2ID\" IN (%4)\n"
            "GROUP BY\n"
            "   CommonID\n")
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
    else if (m_listFilter.column == Common::RATE)
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

    if(m_query.exec(statement))
    {
        // Putting the queried data into a QList.
        m_data.clear();
        
        while (m_query.next())
        {
            CommonItem common = {};
            common.commonID = m_query.value(0).toLongLong();
            common.commonPos = m_query.value(1).toLongLong();
            common.name = m_query.value(2).toString();
            common.url = m_query.value(3).toString();
            common.rate = m_query.value(4).toInt();
            m_data.append(common);
        }

        if (size() > 0)
        {
            // Quering the util table and set it into the specifics rows.
            querySeriesField();
            queryCategoriesField();
            queryAuthorsField();
            querySensitiveContentField();

            if (m_sortingColumnID >= Common::SERIES && m_sortingColumnID <= Common::SENSITIVE_CONTENT)
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

QVariant TableModelCommon::retrieveData() const
{
    // Return the data of the table and the utility interface data.
    Common::SaveDataTable data = {};
    data.tableName = m_tableName;

    // Retrieve common data.
    QSqlQuery query(m_db);

    QString statement = QString(
        "SELECT\n"
        "   CommonID,\n"
        "   CommonPos,\n"
        "   Name,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   CommonID ASC;")
            .arg(m_tableName);
    
    if (!query.exec(statement))
        return QVariant();
    
    while (query.next())
    {
        Common::SaveItem common = {};
        common.commonID = query.value(0).toLongLong();
        common.commonPos = query.value(1).toLongLong();
        common.name = query.value(2).toString();
        common.url = query.value(3).toString();
        common.rate = query.value(4).toInt();
        data.commonList.append(common);
    }

    QVariant utilityInterface = m_interface->data();
    if (!utilityInterface.canConvert<Common::SaveUtilityInterfaceData>())
        return QVariant();
    
    data.interface = qvariant_cast<Common::SaveUtilityInterfaceData>(utilityInterface);

    data.columnSort = (signed char)m_sortingColumnID;
    data.sortOrder = (unsigned char)(m_sortingOrder == Qt::AscendingOrder ? 0 : 1);

    return QVariant::fromValue(data);
}

bool TableModelCommon::setItemData(const QVariant& variant)
{
    // Set the data into the TableModel SQL Tables.
    Common::SaveDataTable data = qvariant_cast<Common::SaveDataTable>(variant);

    // Set the table name and create the SQL tables.
    m_tableName = data.tableName;
    if (m_tableName.isEmpty())
        return false;
    createTable();

    // Set the common list.
    QString statement = QString(
        "INSERT INTO \"%1\" (CommonID, CommonPos, Name, Url, Rate)\n"
        "VALUES")
        .arg(m_tableName);
    
    for (long long int i = 0; i < data.commonList.size(); i+=10)
    {
        QString strData;
        for (long long int j = i; j < i+10 && j < data.commonList.size(); j++)
        {
            strData +=
                QString("\n\t(%1, %2, \"%3\", \"%4\", %5),")
                    .arg(data.commonList.at(j).commonID)
                    .arg(data.commonList.at(j).commonPos)
                    .arg(data.commonList.at(j).name)
                    .arg(data.commonList.at(j).url)
                    .arg(data.commonList.at(j).rate);
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
    m_interface = new TableModelCommon_UtilityInterface(m_tableName, m_db, QVariant::fromValue(data.interface));
    if (!m_interface->isTableReady())
    {
        m_isTableCreated = false;
        return false;
    }

    // Then, query the whole table.
    updateQuery();

    return true;
}

void TableModelCommon::createTable()
{
    // Create the Common SQL table.
    QString statement = QString(
        "CREATE TABLE \"%1\" (\n"
        "   CommonID INTEGER PRIMARY KEY,\n"
        "   CommonPos INTEGER,\n"
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
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
    m_query.clear();
}

void TableModelCommon::deleteTable()
{
    // Delete the SQL Utility Interface
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

void TableModelCommon::utilityChanged(long long int commonID, UtilityTableName tableName)
{
    // This member function is called when the utility interface if changed.
    if (commonID >= 0 && size() > 0 && m_isTableCreated)
    {
        if (tableName == UtilityTableName::SERIES)
            querySeriesField(commonID);
        else if (tableName == UtilityTableName::CATEGORIES)
            queryCategoriesField(commonID);
        else if (tableName == UtilityTableName::AUTHORS)
            queryAuthorsField(commonID);
        else if (tableName == UtilityTableName::SENSITIVE_CONTENT)
            querySensitiveContentField(commonID);
    }
}

void TableModelCommon::queryUtilityField(UtilityTableName tableName)
{
    // Standard interface to query the utility data except the sensitive data.
    if (!m_isTableCreated)
        return;

    QString statement = QString(
        "SELECT\n"
        "   \"%1\".CommonID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".\"UtilityID\"\n"
        "INNER JOIN \"%3\" ON \"%3\".\"ItemID\" = \"%1\".CommonID\n"
        "%6"
        "GROUP BY\n"
        "   \"%1\".CommonID\n"
        "ORDER BY\n"
        "   \"%1\".%4 %5;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName));
    
    // Sorting order.
    if (m_sortingColumnID == Common::NAME)
    {
        statement = statement.arg("Name");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Common::RATE)
    {
        statement = statement.arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg("CommonPos").arg("ASC");

    // Filtering the view.
    if (m_listFilter.column == Common::NAME)
    {
        QString where = QString(
            "WHERE\n"
            "   \"%1\".Name LIKE \"%%2%\"\n")
            .arg(m_tableName, m_listFilter.pattern);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column >= Common::SERIES &&
        m_listFilter.column <= Common::AUTHORS)
    {
        UtilityTableName tName;
        if (m_listFilter.column == Common::SERIES)
            tName = UtilityTableName::SERIES;
        if (m_listFilter.column == Common::CATEGORIES)
            tName = UtilityTableName::CATEGORIES;
        else if (m_listFilter.column == Common::AUTHORS)
            tName = UtilityTableName::AUTHORS;
        
        QString commonsID;
        for (int i = 0; i < m_data.size(); i++)
        {
            if (i > 0)
                commonsID += ',';
            commonsID += QString::number(m_data.at(i).commonID);
        }
        
        QString where = QString(
            "WHERE\n"
            "   \"%1\".CommonID IN (%2)\n")
            .arg(m_tableName, commonsID);
        statement = statement.arg(where);
    }
    else if (m_listFilter.column == Common::RATE)
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
            long long int commonID = m_query.value(0).toLongLong();
            QString utilityName = m_query.value(1).toString();

            for (int i = 0; i < rowCount(); i++)
            {
                if (m_data.at(i).commonID == commonID)
                {
                    if (tableName == UtilityTableName::SERIES)
                        m_data[i].series = utilityName;
                    else if (tableName == UtilityTableName::CATEGORIES)
                        m_data[i].categories = utilityName;
                    else if (tableName == UtilityTableName::AUTHORS)
                        m_data[i].authors = utilityName;
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

void TableModelCommon::queryUtilityField(UtilityTableName tableName, long long int commonID)
{
    // Stardard interface to update the utility field of the game (gameID).
    if (!m_isTableCreated)
        return;

    QString statement = QString(
        "SELECT\n"
        "   \"%1\".CommonID,\n"
        "   GROUP_CONCAT(\"%2\".Name, \", \")\n"
        "FROM\n"
        "   \"%1\"\n"
        "INNER JOIN \"%2\" ON \"%2\".\"%2ID\" = \"%3\".UtilityID\n"
        "INNER JOIN \"%3\" ON \"%3\".ItemID = \"%1\".CommonID\n"
        "WHERE\n"
        "   \"%1\".CommonID = %4;")
            .arg(m_tableName)
            .arg(m_utilityTable.tableName(tableName))
            .arg(m_interface->tableName(tableName))
            .arg(commonID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findCommonPos(commonID);
        if (pos >= 0 && pos <= rowCount())
        {
            QString utilityName;
            if (m_query.next())
                utilityName = m_query.value(1).toString();

            if (tableName == UtilityTableName::SERIES)
                m_data[pos].series = utilityName;
            else if (tableName == UtilityTableName::CATEGORIES)
                m_data[pos].categories = utilityName;
            else if (tableName == UtilityTableName::AUTHORS)
                m_data[pos].authors = utilityName;
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Categories of the game %1 in the table %2.\n\t%3")
            .arg(commonID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

int TableModelCommon::findCommonPos(long long int commonID) const
{
    for (int i = 0; i < size(); i++)
        if (m_data.at(i).commonID == commonID)
            return i;
    return -1;
}

void TableModelCommon::querySeriesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERIES);
}

void TableModelCommon::querySeriesField(long long int commonID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::SERIES, commonID);
}

void TableModelCommon::queryCategoriesField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES);
}

void TableModelCommon::queryCategoriesField(long long int commonID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::CATEGORIES, commonID);
}

void TableModelCommon::queryAuthorsField()
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::AUTHORS);
}

void TableModelCommon::queryAuthorsField(long long int commonID)
{
    if (m_isTableCreated)
        queryUtilityField(UtilityTableName::AUTHORS, commonID);
}

void TableModelCommon::querySensitiveContentField()
{
    // Getting the Sensitive Content Information for each common items.
    QString statement = QString(
        "SELECT\n"
        "   ItemID,\n"
        "   ExplicitContent,\n"
        "   ViolenceContent,\n"
        "   BadLanguage,\n"
        "   \"%2\".CommonID\n"
        "FROM\n"
        "   \"%1\", \"%2\"\n"
        "ORDER BY\n"
        "   \"%3\".%4 %5;")
            .arg(m_interface->tableName(UtilityTableName::SENSITIVE_CONTENT))
            .arg(m_tableName);

    // Sorting order
    if (m_sortingColumnID == Common::NAME)
    {
        statement = statement.arg(m_tableName).arg("Name");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else if (m_sortingColumnID == Common::RATE)
    {
        statement = statement.arg(m_tableName).arg("Rate");
        SORTING_ORDER(m_sortingOrder, statement)
    }
    else
        statement = statement.arg(m_tableName).arg("CommonPos").arg("ASC");
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl;
#endif

    if (m_query.exec(statement))
    {
        while (m_query.next())
        {
            long long int commonID = m_query.value(0).toLongLong();
            SensitiveContent sensData = {};
            sensData.explicitContent = m_query.value(1).toInt();
            sensData.violenceContent = m_query.value(2).toInt();
            sensData.badLanguageContent = m_query.value(3).toInt();

            for (int i = 0; i < size(); i++)
            {
                if (m_data.at(i).commonID == commonID)
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

void TableModelCommon::querySensitiveContentField(long long int commonID)
{
    // Updating the field sensitive content of the game (gameID).
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
            .arg(commonID);

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        // Then, apply the retrieved field into the view.
        int pos = findCommonPos(commonID);
        if (pos >= 0 && pos < rowCount() && m_query.next())
        {
            SensitiveContent sensData = {};
            sensData.explicitContent = m_query.value(1).toInt();
            sensData.violenceContent = m_query.value(2).toInt();
            sensData.badLanguageContent = m_query.value(3).toInt();
            m_data[pos].sensitiveContent = sensData;
            emit dataChanged(index(pos, Common::SENSITIVE_CONTENT), index(pos, Common::SENSITIVE_CONTENT));
        }
        m_query.clear();
    }
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to query Sensitive Content of the common item %1 in the table %2.\n\t%3")
            .arg(commonID)
            .arg(m_tableName)
            .arg(m_query.lastError().text()).toLocal8Bit().constData()
            << std::endl;
#endif
}

QString TableModelCommon::url(const QModelIndex& index) const
{
    if (index.row() >= 0 && index.row() < size())
        return m_data.at(index.row()).url;
    return QString();
}

void TableModelCommon::setUrl(const QModelIndex& index, const QString& url)
{
    // Set the url of the common item.
    if (index.isValid() && index.row() >= 0 && index.row() < size())
    {
        long long int commonID = m_data.at(index.row()).commonID;

        QString statement = QString(
            "UPDATE \"%1\"\n"
            "SET Url = \"%2\"\n"
            "WHERE CommonID = %3;")
                .arg(m_tableName)
                .arg(url)
                .arg(m_data.at(index.row()).commonID);

#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
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

TableModel_UtilityInterface* TableModelCommon::utilityInterface()
{
    if (m_isTableCreated)
        return m_interface;
    return nullptr;
}

void TableModelCommon::updateCommonPos(int from)
{
    // Update the GamePos SQL column, used to apply order in the view.
    if (from < 0)
        from = 0;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET CommonPos = %2\n"
        "WHERE CommonID = %3;");
    
    for (int i = from; i < size(); i++)
    {
        if (m_data.at(i).commonPos != i)
        {
            QString statement = baseStatement
                .arg(m_tableName)
                .arg(i)
                .arg(m_data.at(i).commonID);

#ifndef NDEBUG
            std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

            if (m_query.exec(statement))
            {
                m_data[i].commonPos = i;
                m_query.clear();
            }
            else
            {
                std::cerr << QString("Failed to update game position of game %1 of the table %2.\n\t%3")
                    .arg(m_data.at(i).commonID)
                    .arg(m_tableName)
                    .arg(m_query.lastError().text())
                    .toLocal8Bit().constData()
                    << std::endl;
                m_query.clear();
            }
        }
    }
}

QItemSelection TableModelCommon::moveItemsUp(const QModelIndexList& indexList)
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
        "   CommonPos = %2\n"
        "WHERE CommonID = %3;");
    
    foreach (const QModelIndex& index, indexListCpy)
    {
        // If index is equal to 0, ignore
        if (index.row() == 0)
            continue;

        QString statement = baseStatement
            .arg(m_tableName)
            .arg(m_data.at(index.row()).commonPos-1)
            .arg(m_data.at(index.row()).commonID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement))
        {
            // Store the common item and delete it from the common list.
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
                this->index(index.row()-1, NUMBER_COMMON_TABLE_COLUMN_COUNT)));

            updateCommonPos(index.row()-1);
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

QItemSelection TableModelCommon::moveItemsDown(const QModelIndexList& indexList)
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
        "   CommonPos = %2\n"
        "WHERE CommonID = %3;");
    
    foreach (const QModelIndex& index, indexListCpy)
    {
        // If index is equal to size() - 1, ignore.
        if (index.row() == size()-1)
            continue;
        
        QString statement = baseStatement
            .arg(m_tableName)
            .arg(m_data.at(index.row()).commonPos+1)
            .arg(m_data.at(index.row()).commonID);
        
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;        // If index is equal to 0, ignore
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
                this->index(index.row()+1, NUMBER_COMMON_TABLE_COLUMN_COUNT)));

            updateCommonPos(index.row());
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

QItemSelection TableModelCommon::moveItemsTo(const QModelIndexList& indexList, int to)
{
    // Move the selected items from the view to the position (to).
    // Sorting the list to be able to remove items from the highest index to the lowest index.
    QModelIndexList indexListCpy(indexList);
    std::sort(indexListCpy.begin(), indexListCpy.end(), 
        [](const QModelIndex& index1, const QModelIndex& index2) -> bool
        {
            return index1.row() < index2.row();
        });

    // Removing items and add the valid index to the movingCommon list.
    QList<CommonItem> movingCommon;
    for (int i = indexListCpy.size()-1; i >= 0; i--)
    {
        if (indexListCpy.at(i).row() < 0 || indexListCpy.at(i).row() >= size() ||
            indexListCpy.at(i).column() < 0 || indexListCpy.at(i).column() >= columnCount())
            continue;
        
        movingCommon.prepend(m_data.at(indexListCpy.at(i).row()));
        beginRemoveRows(QModelIndex(), indexListCpy.at(i).row(), indexListCpy.at(i).row());
        m_data.remove(indexListCpy.at(i).row(), 1);
        endRemoveRows();
    }

    // The moved items will be store in this list to update the selection model of the view.
    QItemSelection selectedIndex;

    QString baseStatement = QString(
        "UPDATE \"%1\"\n"
        "SET\n"
        "   CommonPos = %2\n"
        "WHERE CommonID = %3;");
    
    // Moving the items.
    int i = to;
    for (CommonItem& item : movingCommon)
    {
        item.commonPos = i;

        QString statement = baseStatement
            .arg(m_tableName)
            .arg(i)
            .arg(item.commonID);

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
            updateCommonPos(indexListCpy.at(0).row());
        else
            updateCommonPos(to);
        selectedIndex.append(QItemSelectionRange(
            index(to, 0),
            index(i-1, NUMBER_COMMON_TABLE_COLUMN_COUNT)));
        emit listEdited();
    }

    // Return the list of the moved index.
    return selectedIndex;
}

void TableModelCommon::sortUtility(int column)
{
    // Sorting common items by there utilities.
    if (column == Common::NAME || column == Common::RATE)
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
        [column, compareString, compareSens] (const CommonItem& item1, const CommonItem& item2) -> bool
        {
            if (column == Common::SERIES)
                return compareString(item1.series, item2.series);
            else if (column == Common::CATEGORIES)
                return compareString(item1.categories, item2.categories);
            else if (column == Common::AUTHORS)
                return compareString(item1.authors, item2.authors);
            else if (column == Common::SENSITIVE_CONTENT)
                return compareSens(item1.sensitiveContent, item2.sensitiveContent);
            return false;
        };
    
    std::stable_sort(m_data.begin(), m_data.end(), sortTemplate);
}

void TableModelCommon::copyToClipboard(QModelIndexList indexList)
{
    if (indexList.size() == 0)
        return;

    QString commonNames;

    if (indexList.size() == 1)
        commonNames = m_data.at(indexList.at(0).row()).name;
    else
    {
        foreach(const QModelIndex& index, indexList)
            commonNames += ':' + m_data.at(index.row()).name;
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(commonNames);
}

int TableModelCommon::retrieveMaxPos()
{
    // Retrieve max pos.
    int maxPos = 0;
    QString posStatement = QString(
        "SELECT\n"
        "   MAX(CommonPos)\n"
        "FROM\n"
        "   \"%1\";").arg(m_tableName);

#ifndef NDEBUG
    std::cout << posStatement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(posStatement))
        if (m_query.next())
            maxPos = m_query.value(0).toInt();
#ifndef NDEBUG
    else
        std::cerr << QString("Failed to get max position on the table %1.\n\t%2")
            .arg(m_tableName, m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
    m_query.clear();
    return maxPos;
}

void TableModelCommon::retrieveInsertedRows(int row, int count)
{
    // Retrieve the inserted data into the SQL table.
    QString statement = QString(
        "SELECT\n"
        "   CommonID,\n"
        "   CommonPos,\n"
        "   Name,\n"
        "   Url,\n"
        "   Rate\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   CommonID DESC\n"
        "LIMIT\n"
        "   %2;")
            .arg(m_tableName)
            .arg(count);
        
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << '\n' << std::endl;
#endif

    if (m_query.exec(statement))
    {
        if (m_sortingColumnID >= 0)
        {
            if (count == 1)
                beginInsertRows(QModelIndex(), rowCount(), rowCount());
            else
                beginInsertRows(QModelIndex(), rowCount(), rowCount()+count-1);
        }
        else
        {
            if (count == 1)
                beginInsertRows(QModelIndex(), row, row);
            else
                beginInsertRows(QModelIndex(), row, row+count-1);
        }

        QList<CommonItem> commonList;
        while(m_query.next())
        {
            CommonItem common = {};
            common.commonID = m_query.value(0).toLongLong();
            common.commonPos = m_query.value(1).toLongLong();
            common.name = m_query.value(2).toString();
            common.url = m_query.value(3).toString();
            common.rate = m_query.value(4).toInt();
            commonList.prepend(common);
        }
        if (m_sortingColumnID >= 0)
            m_data.append(commonList.cbegin(), commonList.cend());
        else
        {
            for (int i = 0; i < commonList.size(); i++)
                m_data.insert(row+i, commonList.at(i));
            updateCommonPos(row+commonList.size());
        }

        endInsertRows();
    }
    else
        updateQuery();
    
    // Emit the signal listEdited, this signal is used to tell that the list has been edited.
    emit listEdited();
    m_query.clear();
}