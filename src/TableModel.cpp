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
#include "UtilityInterfaceEditor.h"
#include "UtilitySensitiveContentEditor.h"
#include "Common.h"
#include <QSqlError>
#include <iostream>
#include <algorithm>

TableModel::TableModel(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    QAbstractTableModel(parent),
    m_db(db),
    m_utilityTable(utilityTable),
    m_query(m_db),
    m_isTableCreated(false),
    m_isTableChanged(false)
{
    m_tableName = checkingIfNameFree(replaceSpaceByUnderscore(replaceMultipleSpaceByOne(removeFirtAndLastSpaces(tableName))));
}

TableModel::TableModel(QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent) :
    QAbstractTableModel(parent),
    m_db(db),
    m_utilityTable(utilityTable),
    m_query(m_db),
    m_isTableCreated(false),
    m_isTableChanged(false)
{}

TableModel::~TableModel()
{
    m_query.clear();
}

QString TableModel::tableName() const
{
    return replaceUnderscoreBySpace(m_tableName);
}

void TableModel::setTableName(const QString& tableName)
{
    // Update the table name, but before, check if the name is free
    if (tableName.isEmpty() || tableName.compare(m_tableName, Qt::CaseInsensitive) == 0 || !m_isTableCreated)
        return;
    
    QString newTableName = checkingIfNameFree(replaceSpaceByUnderscore(replaceMultipleSpaceByOne(removeFirtAndLastSpaces(tableName))));

    // Apply the new name to the SQL table.
    QString statement = QString(
        "ALTER TABLE \"%1\"\n"
        "RENAME TO \"%2\";")
            .arg(m_tableName)
            .arg(newTableName);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to rename the table form %1 to %2.\n\t%3")
            .arg(m_tableName)
            .arg(newTableName)
            .arg(m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
#endif
        return;
    }
    m_query.clear();

    m_tableName = newTableName;
    emit listEdited();
    emit tableNameChanged(m_tableName);
}

QString TableModel::rawTableName() const
{
    return m_tableName;
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