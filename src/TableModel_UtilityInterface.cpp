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

#include "TableModel_UtilityInterface.h"

#include <iostream>

#include <QSqlError>

TableModel_UtilityInterface::TableModel_UtilityInterface(const QString& parentTableName, ListType type, QSqlDatabase& db) :
	m_parentTableName(parentTableName),
	m_listType(type),
	m_db(db),
	m_query(m_db),
	m_isTableReady(true)
{
	createTables();
}

TableModel_UtilityInterface::~TableModel_UtilityInterface()
{
	destroyTables();
}

QString TableModel_UtilityInterface::tableName(UtilityTableName tableName) const
{
	// The name of the interface table is link to the parentTableName.

	switch (tableName)
	{
	case UtilityTableName::CATEGORIES:
		return m_parentTableName + "_Categories";
	case UtilityTableName::DEVELOPPERS:
		return m_parentTableName + "_Developpers";
	case UtilityTableName::PUBLISHERS:
		return m_parentTableName + "_Publishers";
	case UtilityTableName::PLATFORM:
		return m_parentTableName + "_Platform";
	case UtilityTableName::SERVICES:
		return m_parentTableName + "_Services";
	case UtilityTableName::SENSITIVE_CONTENT:
		return m_parentTableName + "_SensitiveContent";
	default:
		return QString();
	}
}

void TableModel_UtilityInterface::createTables()
{
	if (m_listType == ListType::GAMELIST)
		createGameTables();
}

void TableModel_UtilityInterface::destroyTables()
{
	if (m_listType == ListType::GAMELIST)
		destroyGameTables();
}

void TableModel_UtilityInterface::printTableCreationError(const QString& tableName, const QString& messageError)
{
	std::cerr << QString("Failed to create the %1 table.\n\t%2")
		.arg(tableName)
		.arg(messageError)
		.toLocal8Bit().constData()
		<< std::endl;
}

void TableModel_UtilityInterface::destroyTableByName(const QString& tableName)
{
	m_query.clear();

	QString statement = QString(
		"DROP TABLE IF EXISTS \"%1\";").arg(tableName);

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
		std::cerr << QString("Failed to destroy the %1 table.\n\t%2")
			.arg(tableName)
			.arg(m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
}

void TableModel_UtilityInterface::rowRemoved(const QList<long long int>& itemsID)
{
	if (m_listType == ListType::GAMELIST)
		gameRowRemoved(itemsID);
}

void TableModel_UtilityInterface::updateItemUtility(long long int itemID, UtilityTableName tableName, const QList<long long int>& dataList)
{
	// Updating all the utility of a category of the SQL table TableModel.

	// First, removing all the utility of the itemID.
	m_query.clear();

	QString statement = QString(
		"DELETE FROM \"%1\"\n"
		"WHERE\n"
		"	ItemID = %2;")
			.arg(this->tableName(tableName))
			.arg(itemID);
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		std::cerr << QString("Failed to remove all the utility of the table %1 where ItemID is equal to %2.\n\t%3")
			.arg(this->tableName(tableName))
			.arg(itemID)
			.arg(m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
		return;
	}

	// Then, insert the data list into the utility table for the item itemID.
	m_query.clear();

	if (dataList.size() > 0)
	{
		statement = QString(
			"INSERT INTO \"%1\" (ItemID, UtilityID)\n"
			"VALUES\n")
				.arg(this->tableName(tableName));
		
		bool first = true;

		for (int i = 0; i < dataList.size(); i++)
		{
			if (!first)
				statement += ",\n";
			else
				first = false;
			
			statement += QString("	(%1, %2)").arg(itemID).arg(dataList.at(i));
		}
		statement += ";";
	}

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
		std::cerr << QString("failed to insert data innto %1.\n\t%2")
			.arg(this->tableName(tableName))
			.arg(m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
}