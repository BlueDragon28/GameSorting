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
	createSeriesTable();
	createCategoriesTable();
	createDeveloppersTable();
	createPublishersTable();
	createPlatformTable();
	createServicesTable();
}

void SqlUtilityTable::destroyGameTables()
{
	destroyTableByName(tableName(UtilityTableName::SERIES));
	destroyTableByName(tableName(UtilityTableName::CATEGORIES));
	destroyTableByName(tableName(UtilityTableName::DEVELOPPERS));
	destroyTableByName(tableName(UtilityTableName::PUBLISHERS));
	destroyTableByName(tableName(UtilityTableName::PLATFORM));
	destroyTableByName(tableName(UtilityTableName::SERVICES));
	m_type = ListType::UNKNOWN;
	m_isTableReady = false;
}

void SqlUtilityTable::createSeriesTable()
{
	standardTableCreation(UtilityTableName::SERIES);
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
	data.series = retrieveTableData(UtilityTableName::SERIES);
	data.categories = retrieveTableData(UtilityTableName::CATEGORIES);
	data.developpers = retrieveTableData(UtilityTableName::DEVELOPPERS);
	data.publishers = retrieveTableData(UtilityTableName::PUBLISHERS);
	data.platform = retrieveTableData(UtilityTableName::PLATFORM);
	data.services = retrieveTableData(UtilityTableName::SERVICES);

	return QVariant::fromValue(data);
}

bool SqlUtilityTable::setGameData(const QVariant& variant)
{
	// Set the data into the SQL tables.
	Game::SaveUtilityData data = qvariant_cast<Game::SaveUtilityData>(variant);

	if (!setStandardData(UtilityTableName::SERIES, data.series) ||
		!setStandardData(UtilityTableName::CATEGORIES, data.categories) ||
		!setStandardData(UtilityTableName::DEVELOPPERS, data.developpers) ||
		!setStandardData(UtilityTableName::PUBLISHERS, data.publishers) ||
		!setStandardData(UtilityTableName::PLATFORM, data.platform) ||
		!setStandardData(UtilityTableName::SERVICES, data.services))
		return false;
	
	return true;
}