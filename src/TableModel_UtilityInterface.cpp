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

TableModel_UtilityInterface::TableModel_UtilityInterface(const QString& parentTableName, QSqlDatabase& db) :
	m_parentTableName(parentTableName),
	m_db(db),
	m_query(m_db),
	m_isTableReady(false)
{}

TableModel_UtilityInterface::~TableModel_UtilityInterface()
{
	m_query.clear();
}

bool TableModel_UtilityInterface::isTableReady() const
{
	return m_isTableReady;
}

void TableModel_UtilityInterface::destroyTableByName(const QString& tableName)
{
	// Destroy a table using it's name.
	if (!m_isTableReady) 
		return;

	QString statement = QString(
		"DROP TABLE IF EXISTS \"%1\";").arg(tableName);

#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
		std::cerr << QString("Failed to destroy table %1.\n\t%2")
			.arg(tableName)
			.arg(m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
	m_query.clear();
}

void TableModel_UtilityInterface::renameTable(const QString& currentName, const QString& newName)
{
	// Renaming a table.
	if (!m_isTableReady) 
		return;

	QString statement = QString(
		"ALTER TABLE \"%1\"\n"
		"RENAME TO \"%2\";")
			.arg(currentName, newName);
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		std::cerr << QString("Failed to rename table from %1 to %2.\n\t%3")
			.arg(currentName, newName, m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
	}
	m_query.clear();
	return;
}