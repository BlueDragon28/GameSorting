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

void TableModel_UtilityInterface::createGameTables()
{
	createCategoriesTable();
	createDeveloppersTable();
	createPublishersTable();
	createPlatformTable();
	createServicesTable();
	createSensitiveContentTable();
}

void TableModel_UtilityInterface::destroyGameTables()
{
	destroyTableByName(tableName(UtilityTableName::CATEGORIES));
	destroyTableByName(tableName(UtilityTableName::DEVELOPPERS));
	destroyTableByName(tableName(UtilityTableName::PUBLISHERS));
	destroyTableByName(tableName(UtilityTableName::PLATFORM));
	destroyTableByName(tableName(UtilityTableName::SERVICES));
	destroyTableByName(tableName(UtilityTableName::SENSITIVE_CONTENT));
}

void TableModel_UtilityInterface::createCategoriesTable()
{
	// Creating the Categories interface table.
	if (!m_isTableReady) return;

	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	ItemID INTEGER,\n"
		"	UtilityID INTEGER);").arg(tableName(UtilityTableName::CATEGORIES));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		printTableCreationError(tableName(UtilityTableName::CATEGORIES), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void TableModel_UtilityInterface::createDeveloppersTable()
{
	// Creating the Developpers interface table
	if (!m_isTableReady) return;

	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	ItemID INTEGER,\n"
		"	UtilityID INTEGER);").arg(tableName(UtilityTableName::DEVELOPPERS));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		printTableCreationError(tableName(UtilityTableName::DEVELOPPERS), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void TableModel_UtilityInterface::createPublishersTable()
{
	// Creating the Publishers interface table
	if (!m_isTableReady) return;

	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	ItemID INTEGER,\n"
		"	UtilityID INTEGER);").arg(tableName(UtilityTableName::PUBLISHERS));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		printTableCreationError(tableName(UtilityTableName::PUBLISHERS), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void TableModel_UtilityInterface::createPlatformTable()
{
	// Creating the Platform interface table
	if (!m_isTableReady) return;

	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	ItemID INTEGER,\n"
		"	UtilityID INTEGER);").arg(tableName(UtilityTableName::PLATFORM));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		printTableCreationError(tableName(UtilityTableName::PLATFORM), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void TableModel_UtilityInterface::createServicesTable()
{
	// Creating the Services interface table
	if (!m_isTableReady) return;

	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	ItemID INTEGER,\n"
		"	UtilityID INTEGER);").arg(tableName(UtilityTableName::SERVICES));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		printTableCreationError(tableName(UtilityTableName::SERVICES), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void TableModel_UtilityInterface::createSensitiveContentTable()
{
	// Creating the Sensitive Content interface table.
	if (!m_isTableReady) return;

	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	SensitiveContentID INTEGER PRIMARY KEY,\n"
		"	ItemID INTEGER,\n"
		"	ExplicitContent INTEGER,\n"
		"	ViolenceContent INTEGER,\n"
		"	BadLanguage INTEGER);")
			.arg(tableName(UtilityTableName::SENSITIVE_CONTENT));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		printTableCreationError(tableName(UtilityTableName::SENSITIVE_CONTENT), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void TableModel_UtilityInterface::gameRowRemoved(const QList<long long int>& itemsID)
{
	// Deleting any column of each utility tables referrencing to the gameID itemID.
	if (!m_isTableReady || itemsID.size() == 0) return;

	// Converting the list of itemsID into a string.
	QString idList;
	for (int i = 0; i < itemsID.size(); i++)
	{
		if (i > 0) 
			idList += ", ";

		idList += QString::number(itemsID.at(i));
	}

	// Deleting all the rows of all the utility table with itemID in it.
	UtilityTableName tablesName[6] =
	{
		UtilityTableName::CATEGORIES,
		UtilityTableName::DEVELOPPERS,
		UtilityTableName::PUBLISHERS,
		UtilityTableName::PLATFORM,
		UtilityTableName::SERVICES,
		UtilityTableName::SENSITIVE_CONTENT
	};

	for (int i = 0; i < 6; i++)
	{
		QString statement = QString(
			"DELETE FROM \"%1\"\n"
			"WHERE\n")
				.arg(tableName(tablesName[i]));
		
		if (itemsID.size() == 1)
			statement += QString("	ItemID = %1;").arg(idList);
		else
			statement += QString("	ItemID in (%1);").arg(idList);
		
#ifndef NDEBUG
		std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

		if (!m_query.exec(statement))
			std::cerr << QString("Failed to delete the utility interface rows where ItemID = %1 in the utility interface table %2.\n\t%3")
				.arg(idList)
				.arg(tableName(tablesName[i]))
				.arg(m_query.lastError().text())
				.toLocal8Bit().constData()
				<< std::endl;
	}
}