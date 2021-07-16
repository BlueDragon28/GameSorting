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
	if (!m_db.isOpen() || m_type == ListType::UNKNOWN)
	{
		m_isTableReady = false;
		return;
	}

	createTables();
}

SqlUtilityTable::~SqlUtilityTable()
{
	destroyTables();
	m_query.clear();
}

void SqlUtilityTable::newList(ListType type)
{
	// Destoying all the existing table and recreating them for the new list.
	destroyTables();
	m_type = type;
	if (type != ListType::UNKNOWN)
		createTables();
	else
		m_isTableReady = false;
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
	{
		m_isTableReady = true;
		createGameTables();
	}
	else
		m_isTableReady = false;
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
	m_query.clear();
}

QList<ItemUtilityData> SqlUtilityTable::retrieveTableData(UtilityTableName tableName, bool sort, Qt::SortOrder order) const
{
	// Retrive all the data containing in the table tableName.
	QSqlQuery query(m_db);

	QString statement = QString(
		"SELECT\n"
		"	\"%1ID\",\n"
		"	OrderID,\n"
		"	\"Name\"\n"
		"FROM\n"
		"	\"%1\"\n"
		"ORDER BY\n"
		"	%2 %3;")
			.arg(this->tableName(tableName));

	if (sort)
		statement = statement.arg("Name", (order == Qt::AscendingOrder ? "ASC" : "DESC"));
	else
		statement = statement.arg("OrderID", "ASC");
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (query.exec(statement))
	{
		QList<ItemUtilityData> tableData;
		while (query.next())
		{
			long long int utilityID = query.value(0).toLongLong();
			int order = query.value(1).toInt();
			QString name = query.value(2).toString();
			tableData.append({utilityID, order, name});
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

QVariant SqlUtilityTable::data() const
{
	if (m_isTableReady)
	{
		if (m_type == ListType::GAMELIST)
			return queryGameData();
	}
	return QVariant();
}

bool SqlUtilityTable::setData(const QVariant& variant)
{
	// Set data into the utility tables.
	if (variant.canConvert<Game::SaveUtilityData>())
	{
		newList(ListType::GAMELIST);
		return setGameData(variant);
	}
	
	newList(ListType::UNKNOWN);
	return false;
}

void SqlUtilityTable::standardTableCreation(UtilityTableName tableName)
{
	// This member function is used has a base plan for creating the utility table.
	if (!m_isTableReady) return;

	// Create the table.
	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1ID\" INTEGER PRIMARY KEY,\n"
		"	OrderID INTEGER,\n"
		"	Name TEXT);")
			.arg(this->tableName(tableName));
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(this->tableName(tableName), m_query.lastError().text());
		m_isTableReady = false;
	}
	m_query.clear();
}