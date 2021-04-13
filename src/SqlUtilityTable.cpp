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

#include "SqlUtilityTable.h"
#include <iostream>
#include <QSqlError>

SqlUtilityTable::SqlUtilityTable(ListType type, QSqlDatabase& db) :
	m_type(type),
	m_db(db),
	m_query(m_db),
	m_isTableReady(true)
{
	if (!m_db.isOpen())
	{
		m_isTableReady = false;
		return;
	}

	createTables();
}

SqlUtilityTable::~SqlUtilityTable()
{
	destroyTables();
}

QString SqlUtilityTable::tableName(UtilityTableName tableName)
{
	// Return the name of a table.
	switch (tableName)
	{
	case UtilityTableName::CATEGORIES:
		return "Categories";
	case UtilityTableName::DEVELOPPERS:
		return "Developpers";
	case UtilityTableName::PUBLISHERS:
		return "Publishers";
	case UtilityTableName::PLATFORM:
		return "Platform";
	case UtilityTableName::SERVICES:
		return "Services";
	default:
		return QString();
	}
}

void SqlUtilityTable::createTables()
{
	if (m_type == ListType::GAMELIST)
		createGameTables();
}

void SqlUtilityTable::destroyTables()
{
	if (m_type == ListType::GAMELIST)
		destroyGameTables();
}

void SqlUtilityTable::errorMessageCreatingTable(const QString& tableName, const QString& queryError)
{
	std::cerr << QString("Failed to create the %1 table.").arg(tableName).toLocal8Bit().constData() << std::endl << std::endl
		<< queryError.toLocal8Bit().constData() << std::endl;
}

void SqlUtilityTable::destroyTableByName(const QString& tableName)
{
	// A helper function used to delete a table.
	m_query.clear();

	QString statement = QString(
		"DROP TABLE IF EXISTS \"%1\";").arg(tableName);

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		std::cerr << QString("Failed to destroy the %1 table.").arg(tableName).toLocal8Bit().constData() << std::endl << std::endl
			<< QString("\t%1").arg(m_query.lastError().text()).toLocal8Bit().constData() << std::endl << std::endl;
	}
}

QList<ItemUtilityData> SqlUtilityTable::retrieveTableData(UtilityTableName tableName) const
{
	// Retrive all the data containing in the table tableName.
	QSqlQuery query(m_db);

	QString statement = QString(
		"SELECT\n"
		"	\"%1ID\",\n"
		"	\"Name\"\n"
		"FROM\n"
		"	\"%1\";")
			.arg(this->tableName(tableName));
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (query.exec(statement))
	{
		QList<ItemUtilityData> tableData;
		while (query.next())
		{
			long long int utilityID = query.value(0).toLongLong();
			QString name = query.value(1).toString();
			tableData.append({utilityID, name});
		}
		return tableData;
	}
	else
		std::cerr << QString("Failed to retrieve data from %1 table.\n\t")
			.arg(this->tableName(tableName))
			.arg(query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;

	return {};
}