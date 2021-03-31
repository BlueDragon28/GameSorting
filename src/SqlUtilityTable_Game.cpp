/*
* MIT License
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

void SqlUtilityTable::createGameTables()
{
	// Creating all the Game utility table.
	createCategoriesTable();
	createDeveloppersTable();
	createPublishersTable();
	createPlatformTable();
	createServicesTable();
	createSensitiveContentTable();
}

void SqlUtilityTable::destroyGameTables()
{
	destroyTableByName(tableName(UtilityTableName::CATEGORIES));
	destroyTableByName(tableName(UtilityTableName::DEVELOPPERS));
	destroyTableByName(tableName(UtilityTableName::PUBLISHERS));
	destroyTableByName(tableName(UtilityTableName::PLATFORM));
	destroyTableByName(tableName(UtilityTableName::SERVICES));
	destroyTableByName(tableName(UtilityTableName::SENSITIVE_CONTENT));
}

void SqlUtilityTable::createCategoriesTable()
{
	if (!m_isTableReady) return;

	// Creating the categories table.
	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1ID\" INTEGER PRIMARY KEY,\n"
		"	Name TEXT,\n"
		"	Description TEXT);").arg(tableName(UtilityTableName::CATEGORIES));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(tableName(UtilityTableName::CATEGORIES), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void SqlUtilityTable::createDeveloppersTable()
{
	if (!m_isTableReady) return;

	// Creating the developpers table.
	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1ID\" INTEGER PRIMARY KEY,\n"
		"	Name TEXT);").arg(tableName(UtilityTableName::DEVELOPPERS));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(tableName(UtilityTableName::DEVELOPPERS), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void SqlUtilityTable::createPublishersTable()
{
	if (!m_isTableReady) return;

	// Creating the publishers table.
	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1ID\" INTEGER PRIMARY KEY,\n"
		"	Name TEXT);").arg(tableName(UtilityTableName::PUBLISHERS));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(tableName(UtilityTableName::PUBLISHERS), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void SqlUtilityTable::createPlatformTable()
{
	if (!m_isTableReady) return;

	// Creating the platform table.
	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1ID\" INTEGER PRIMARY KEY,\n"
		"	Name TEXT);").arg(tableName(UtilityTableName::PLATFORM));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(tableName(UtilityTableName::PLATFORM), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void SqlUtilityTable::createServicesTable()
{
	if (!m_isTableReady) return;

	// Creating the services table.
	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1ID\" INTEGER PRIMARY KEY,\n"
		"	Name TEXT);").arg(tableName(UtilityTableName::SERVICES));
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(tableName(UtilityTableName::SERVICES), m_query.lastError().text());
		m_isTableReady = false;
	}
}

void SqlUtilityTable::createSensitiveContentTable()
{
	if (!m_isTableReady) return;

	// Creating the sensitive content table.
	m_query.clear();

	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	\"%1\" INTEGER PRIMARY KEY,\n"
		"	ExplicitContent INTEGER,\n"
		"	ViolenceContent INTEGER,\n"
		"	BadLanguage INTEGER);").arg(tableName(UtilityTableName::SENSITIVE_CONTENT));

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		errorMessageCreatingTable(tableName(UtilityTableName::SENSITIVE_CONTENT), m_query.lastError().text());
		m_isTableReady = false;
	}
}