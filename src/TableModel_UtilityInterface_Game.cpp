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
	m_query.clear();
}

void TableModel_UtilityInterface::createDeveloppersTable()
{
	// Creating the Developpers interface table
	if (!m_isTableReady) return;

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
	m_query.clear();
}

void TableModel_UtilityInterface::createPublishersTable()
{
	// Creating the Publishers interface table
	if (!m_isTableReady) return;

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
	m_query.clear();
}

void TableModel_UtilityInterface::createPlatformTable()
{
	// Creating the Platform interface table
	if (!m_isTableReady) return;

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
	m_query.clear();
}

void TableModel_UtilityInterface::createServicesTable()
{
	// Creating the Services interface table
	if (!m_isTableReady) return;

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
	m_query.clear();
}

void TableModel_UtilityInterface::createSensitiveContentTable()
{
	// Creating the Sensitive Content interface table.
	if (!m_isTableReady) return;

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
	m_query.clear();
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
		m_query.clear();
	}
}

QVariant TableModel_UtilityInterface::gameData() const
{
	// Query all the game utility interface.
	Game::SaveUtilityInterfaceData data = {};

	QSqlQuery query(m_db);
	QString statement = QString(
		"SELECT\n"
		"	ItemID,\n"
		"	UtilityID\n"
		"FROM\n"
		"	\"%1\"\n"
		"ORDER BY\n"
		"	ItemID;");
	
	// Categories
	if (!query.exec(statement.arg(tableName(UtilityTableName::CATEGORIES))))
		return QVariant();
	
	while (query.next())
	{
		Game::SaveUtilityInterfaceItem item = {};
		item.gameID = query.value(0).toLongLong();
		item.utilityID = query.value(1).toLongLong();
		data.categories.append(item);
	}
	query.clear();

	// Developpers
	if (!query.exec(statement.arg(tableName(UtilityTableName::DEVELOPPERS))))
		return QVariant();

	while (query.next())
	{
		Game::SaveUtilityInterfaceItem item = {};
		item.gameID = query.value(0).toLongLong();
		item.utilityID = query.value(1).toLongLong();
		data.developpers.append(item);
	}
	query.clear();

	// Publishers
	if (!query.exec(statement.arg(tableName(UtilityTableName::PUBLISHERS))))
		return QVariant();
	
	while (query.next())
	{
		Game::SaveUtilityInterfaceItem item = {};
		item.gameID = query.value(0).toLongLong();
		item.utilityID = query.value(1).toLongLong();
		data.pubishers.append(item);
	}
	query.clear();

	// Platform
	if (!query.exec(statement.arg(tableName(UtilityTableName::PLATFORM))))
		return QVariant();
	
	while (query.next())
	{
		Game::SaveUtilityInterfaceItem item = {};
		item.gameID = query.value(0).toLongLong();
		item.utilityID = query.value(1).toLongLong();
		data.platform.append(item);
	}
	query.clear();

	// Services
	if (!query.exec(statement.arg(tableName(UtilityTableName::SERVICES))))
		return QVariant();
	
	while (query.next())
	{
		Game::SaveUtilityInterfaceItem item = {};
		item.gameID = query.value(0).toLongLong();
		item.utilityID = query.value(1).toLongLong();
		data.services.append(item);
	}
	query.clear();

	// Sensitive Content
	statement = QString(
		"SELECT\n"
		"	SensitiveContentID,\n"
		"	ItemID,\n"
		"	ExplicitContent,\n"
		"	ViolenceContent,\n"
		"	BadLanguage\n"
		"FROM\n"
		"	\"%1\"\n"
		"ORDER BY\n"
		"	SensitiveContentID;")
			.arg(tableName(UtilityTableName::SENSITIVE_CONTENT));
	
	if (!query.exec(statement))
		return QVariant();
	
	while (query.next())
	{
		Game::SaveUtilitySensitiveContentItem item = {};
		item.SensitiveContentID = query.value(0).toLongLong();
		item.gameID = query.value(1).toLongLong();
		item.explicitContent = query.value(2).toInt();
		item.violenceContent = query.value(3).toInt();
		item.badLanguageContent = query.value(4).toInt();
		data.sensitiveContent.append(item);
	}

	return QVariant::fromValue(data);
}

inline bool TableModel_UtilityInterface::setGameStandardData(UtilityTableName tName, const QList<Game::SaveUtilityInterfaceItem>& data)
{
	// Convenient member function to set the Categories, Developpers, Publishers, Platform and Services data.
	QString statement = QString(
		"INSERT INTO \"%1\" (ItemID, UtilityID)\n"
		"VALUES")
			.arg(tableName(tName));
	
	for (long long int i = 0; i < data.size(); i+=10)
	{
		QString strData;
		for (long long int j = i; j < i+10 && j < data.size(); j++)
		{
			strData +=
				QString("\n\t(%1, %2),")
					.arg(data.at(j).gameID)
					.arg(data.at(j).utilityID);
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
				std::cerr << QString("Failed to set data into %1.\n\t%2")
					.arg(tableName(tName))
					.arg(m_query.lastError().text())
					.toLocal8Bit().constData()
					<< std::endl;
#endif
				return false;
			}
			m_query.clear();
		}
	}

	return true;
}

bool TableModel_UtilityInterface::setGameData(const QVariant& variant)
{
	// Apply the data into the SQL table.
	Game::SaveUtilityInterfaceData data = qvariant_cast<Game::SaveUtilityInterfaceData>(variant);

	if (!setGameStandardData(UtilityTableName::CATEGORIES, data.categories) ||
		!setGameStandardData(UtilityTableName::DEVELOPPERS, data.developpers) ||
		!setGameStandardData(UtilityTableName::PUBLISHERS, data.pubishers) ||
		!setGameStandardData(UtilityTableName::PLATFORM, data.platform) ||
		!setGameStandardData(UtilityTableName::SERVICES, data.services))
		return false;

	// Sensitive Content
	QString sensitiveContentStatement = QString(
		"INSERT INTO \"%1\" (SensitiveContentID, ItemID, ExplicitContent, ViolenceContent, BadLanguage)\n"
		"VALUES")
		.arg(tableName(UtilityTableName::SENSITIVE_CONTENT));
	
	for (long long int i = 0; i < data.sensitiveContent.size(); i+=10)
	{
		QString strData;
		for (long long int j = i; j < i+10 && j < data.sensitiveContent.size(); j++)
		{
			strData +=
				QString("\n\t(%1, %2, %3, %4, %5),")
					.arg(data.sensitiveContent.at(j).SensitiveContentID)
					.arg(data.sensitiveContent.at(j).gameID)
					.arg(data.sensitiveContent.at(j).explicitContent)
					.arg(data.sensitiveContent.at(j).violenceContent)
					.arg(data.sensitiveContent.at(j).badLanguageContent);
		}
		if (strData.size() > 0)
		{
			strData[strData.size()-1] = ';';

#ifndef NDEBUG
			std::cout << (sensitiveContentStatement + strData).toLocal8Bit().constData() << std::endl << std::endl;
#endif
		
			if (!m_query.exec(sensitiveContentStatement + strData))
			{
#ifndef NDEBUG
				std::cerr << QString("Failed to set data into %1.\n\t%2")
					.arg(tableName(UtilityTableName::SENSITIVE_CONTENT))
					.arg(m_query.lastError().text())
					.toLocal8Bit().constData()
					<< std::endl;
#endif
				return false;
			}
			m_query.clear();
		}
	}

	return true;
}

void TableModel_UtilityInterface::setGameParentTableNewName(const QString& name)
{
	// Update the name of the game utility interface table name.
	QString oldCategoriesName = tableName(UtilityTableName::CATEGORIES);
	QString oldDeveloppersName = tableName(UtilityTableName::DEVELOPPERS);
	QString oldPublishersName = tableName(UtilityTableName::PUBLISHERS);
	QString oldPlatformName = tableName(UtilityTableName::PLATFORM);
	QString oldServiceName = tableName(UtilityTableName::SERVICES);
	QString oldSensitiveName = tableName(UtilityTableName::SENSITIVE_CONTENT);

	m_parentTableName = name;

	renameTable(oldCategoriesName, tableName(UtilityTableName::CATEGORIES));
	renameTable(oldDeveloppersName, tableName(UtilityTableName::DEVELOPPERS));
	renameTable(oldPublishersName, tableName(UtilityTableName::PUBLISHERS));
	renameTable(oldPlatformName, tableName(UtilityTableName::PLATFORM));
	renameTable(oldServiceName, tableName(UtilityTableName::SERVICES));
	renameTable(oldSensitiveName, tableName(UtilityTableName::SENSITIVE_CONTENT));
}