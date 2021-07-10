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

void SqlUtilityTable::createGameTables()
{
	// Creating all the Game utility table.
	createCategoriesTable();
	createDeveloppersTable();
	createPublishersTable();
	createPlatformTable();
	createServicesTable();
}

void SqlUtilityTable::destroyGameTables()
{
	destroyTableByName(tableName(UtilityTableName::CATEGORIES));
	destroyTableByName(tableName(UtilityTableName::DEVELOPPERS));
	destroyTableByName(tableName(UtilityTableName::PUBLISHERS));
	destroyTableByName(tableName(UtilityTableName::PLATFORM));
	destroyTableByName(tableName(UtilityTableName::SERVICES));
	m_type = ListType::UNKNOWN;
	m_isTableReady = false;
}

void SqlUtilityTable::createCategoriesTable()
{
	standardTableCreation(UtilityTableName::CATEGORIES);
}

void SqlUtilityTable::createDeveloppersTable()
{
	standardTableCreation(UtilityTableName::DEVELOPPERS);
}

void SqlUtilityTable::createPublishersTable()
{
	standardTableCreation(UtilityTableName::PUBLISHERS);
}

void SqlUtilityTable::createPlatformTable()
{
	standardTableCreation(UtilityTableName::PLATFORM);
}

void SqlUtilityTable::createServicesTable()
{
	standardTableCreation(UtilityTableName::SERVICES);
}

QVariant SqlUtilityTable::queryGameData() const
{
	Game::SaveUtilityData data = {};
	data.categories = retrieveTableData(UtilityTableName::CATEGORIES);
	data.developpers = retrieveTableData(UtilityTableName::DEVELOPPERS);
	data.publishers = retrieveTableData(UtilityTableName::PUBLISHERS);
	data.platform = retrieveTableData(UtilityTableName::PLATFORM);
	data.services = retrieveTableData(UtilityTableName::SERVICES);

	return QVariant::fromValue(data);
}

bool SqlUtilityTable::setGameStandardData(UtilityTableName tName, const QList<ItemUtilityData>& data)
{
	// Convenient member function to set the data into the SQL Table.
	QString statement = QString(
		"INSERT INTO \"%1\" (\"%1ID\", OrderID, Name)\n"
		"VALUES")
		.arg(tableName(tName));
	
	for (long long int i = 0; i < data.size(); i+=10)
	{
		QString strData;
		for (long long int j = i; j < i+10 && j < data.size(); j++)
		{
			strData +=
				QString("\n\t(%1, %2, \"%3\"),")
					.arg(data.at(j).utilityID)
					.arg(data.at(j).order)
					.arg(data.at(j).name);
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
				std::cerr << QString("Failed to insert data into %1.\n\t%2")
					.arg(tableName(tName))
					.arg(m_query.lastError().text())
					.toLocal8Bit().constData()
					<< std::endl;
#endif
				m_query.clear();
				return false;
			}
			m_query.clear();
		}
	}

	return true;
}

bool SqlUtilityTable::setGameData(const QVariant& variant)
{
	// Set the data into the SQL tables.
	Game::SaveUtilityData data = qvariant_cast<Game::SaveUtilityData>(variant);

	if (!setGameStandardData(UtilityTableName::CATEGORIES, data.categories) ||
		!setGameStandardData(UtilityTableName::DEVELOPPERS, data.developpers) ||
		!setGameStandardData(UtilityTableName::PUBLISHERS, data.publishers) ||
		!setGameStandardData(UtilityTableName::PLATFORM, data.platform) ||
		!setGameStandardData(UtilityTableName::SERVICES, data.services))
		return false;
	
	return true;
}