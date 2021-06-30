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

#include "TableModelGame_UtilityInterface.h"

#include <iostream>

#include <QSqlError>

TableModelGame_UtilityInterface::TableModelGame_UtilityInterface(const QString& parentTableName, QSqlDatabase& db) :
	TableModel_UtilityInterface(parentTableName, db)
{
	createTables();
}

TableModelGame_UtilityInterface::TableModelGame_UtilityInterface(const QString& parentTableName, QSqlDatabase& db, const QVariant& data) :
	TableModelGame_UtilityInterface(parentTableName, db)
{
	setData(data);
}

TableModelGame_UtilityInterface::~TableModelGame_UtilityInterface()
{
	destroyTables();
}

QString TableModelGame_UtilityInterface::tableName(UtilityTableName tableName) const
{
	// Return the table name of a specific utility.
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

void TableModelGame_UtilityInterface::newParentName(const QString& newParentName)
{
	// Replace the parent table name of the interface table with the new parent table name.
	if (!m_isTableReady)
		return;
	
	// Storing the current table name into variable.
	QString catCurName = tableName(UtilityTableName::CATEGORIES);
	QString devCurName = tableName(UtilityTableName::DEVELOPPERS);
	QString pubCurName = tableName(UtilityTableName::PUBLISHERS);
	QString platCurName = tableName(UtilityTableName::PLATFORM);
	QString servCurName = tableName(UtilityTableName::SERVICES);
	QString sensCurName = tableName(UtilityTableName::SENSITIVE_CONTENT);
	
	// Changing m_parentTableName to the new parent name.
	m_parentTableName = newParentName;

	// Get the new table name.
	QString catNewName = tableName(UtilityTableName::CATEGORIES);
	QString devNewName = tableName(UtilityTableName::DEVELOPPERS);
	QString pubNewName = tableName(UtilityTableName::PUBLISHERS);
	QString platNewName = tableName(UtilityTableName::PLATFORM);
	QString servNewName = tableName(UtilityTableName::SERVICES);
	QString sensNewName = tableName(UtilityTableName::SENSITIVE_CONTENT);

	// Renaming the tables.
	renameTable(catCurName, catNewName);
	renameTable(devCurName, devNewName);
	renameTable(pubCurName, pubNewName);
	renameTable(platCurName, platNewName);
	renameTable(servCurName, servNewName);
	renameTable(sensCurName, sensNewName);
}

void TableModelGame_UtilityInterface::rowRemoved(const QList<long long int>& gamesID)
{
	// Deleting any row of each utility tables referrencing a gameID inside gamesID.
	if (!m_isTableReady || gamesID.size() == 0)
		return;

	QString idList;
	for (int i = 0; i < gamesID.size(); i++)
	{
		if (i > 0)
			idList += ", ";
		idList += QString::number(gamesID.at(i));
	}

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
		
		if (gamesID.size() == 1)
			statement += QString("	ItemID = %1;").arg(idList);
		else
			statement += QString("	ItemID IN (%1);").arg(idList);
		
#ifndef NDEBUG
		std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

		if (!m_query.exec(statement))
			std::cerr << QString("Failed to delete gameID (%1) from the table %2.\n\t%3")
				.arg(idList, tableName(tablesName[i]), m_query.lastError().text())
				.toLocal8Bit().constData()
				<< std::endl;
		m_query.clear();
	}
}

void TableModelGame_UtilityInterface::updateItemUtility(long long int gameID, UtilityTableName tableName, const QVariant& data)
{
	// Updating all the utility of a category of the SQL table TableModel.
	if (!m_isTableReady || !data.isValid() && (data.canConvert<QList<long long int>>() || data.canConvert<Game::SensitiveContentData>()))
		return;

	// First, removing all the utility of the itemID.
	QString statement = QString(
		"DELETE FROM \"%1\"\n"
		"WHERE\n"
		"	ItemID = %2;")
			.arg(this->tableName(tableName))
			.arg(gameID);
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		std::cerr << QString("Failed to remove all the utility of the table %1 where ItemID is equal to %2.\n\t%3")
			.arg(this->tableName(tableName))
			.arg(gameID)
			.arg(m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
		return;
	}

	// If the data is an utility
	if (data.canConvert<QList<long long int>>())
	{
		QList<long long int> utilitiesID = qvariant_cast<QList<long long int>>(data);

		if (utilitiesID.size() > 0)
		{
			statement = QString(
				"INSERT INTO \"%1\" (ItemID, UtilityID)\n"
				"VALUES")
					.arg(this->tableName(tableName));
				
			for (int i = 0; i < utilitiesID.size(); i++)
			{
				if (i > 0)
					statement += ',';
				statement += QString("\n	(%1, %2)").arg(gameID).arg(utilitiesID.at(i));
			}
			statement += ';';

#ifndef NDEBUG
			std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

			if (m_query.exec(statement))
				emit interfaceChanged(gameID, tableName);
			else
				std::cerr << QString("Failed to insert data into %1.\n\t%2")
					.arg(this->tableName(tableName), m_query.lastError().text())
					.toLocal8Bit().constData()
					<< std::endl;
			m_query.clear();
		}
		else
			emit interfaceChanged(gameID, tableName);
	}
	// If the data is sensitive content.
	else if (data.canConvert<Game::SensitiveContentData>())
	{
		Game::SensitiveContentData sensData = qvariant_cast<Game::SensitiveContentData>(data);

		QString statement = QString(
			"INSERT INTO \"%1\" (ItemID, ExplicitContent, ViolenceContent, BadLanguage)\n"
			"VALUES\n"
			"	(%2, %3, %4, %5);")
				.arg(this->tableName(UtilityTableName::SENSITIVE_CONTENT))
				.arg(gameID)
				.arg(sensData.explicitContent)
				.arg(sensData.violenceContent)
				.arg(sensData.badLanguageContent);
		
#ifndef NDEBUG
		std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

		if (m_query.exec(statement))
			emit interfaceChanged(gameID, tableName);
		else
			std::cerr << QString("Failed to insert data into %1.\n\t%2")
				.arg(this->tableName(tableName), m_query.lastError().text())
				.toLocal8Bit().constData()
				<< std::endl;
		m_query.clear();
	}
}

ListType TableModelGame_UtilityInterface::listType() const
{
	return ListType::GAMELIST;
}

QVariant TableModelGame_UtilityInterface::data() const
{
	// Retrieve the data of the utility interface to be save into a file.
	if (!m_isTableReady)
		return QVariant();

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
	
	UtilityTableName tablesName[5] =
	{
		UtilityTableName::CATEGORIES,
		UtilityTableName::DEVELOPPERS,
		UtilityTableName::PUBLISHERS,
		UtilityTableName::PLATFORM,
		UtilityTableName::SERVICES
	};

	// Utility interface (Categories, Developpers, Publishers, Platform and Services).
	for (int i = 0; i < 5; i++)
	{
		if (!query.exec(statement.arg(tableName(tablesName[i]))))
			return QVariant();
		
		while (query.next())
		{
			Game::SaveUtilityInterfaceItem item = {};
			item.gameID = query.value(0).toLongLong();
			item.utilityID = query.value(1).toLongLong();
			if (tablesName[i] == UtilityTableName::CATEGORIES)
				data.categories.append(item);
			else if (tablesName[i] == UtilityTableName::DEVELOPPERS)
				data.developpers.append(item);
			else if (tablesName[i] == UtilityTableName::PUBLISHERS)
				data.pubishers.append(item);
			else if (tablesName[i] == UtilityTableName::PLATFORM)
				data.platform.append(item);
			else if (tablesName[i] == UtilityTableName::SERVICES)
				data.services.append(item);
		}
		query.clear();
	}

	// Sensitive content
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

bool TableModelGame_UtilityInterface::setData(const QVariant& variant)
{
	// Apply the data into the SQL table.
	if (!variant.isValid())
		return false;
	
	Game::SaveUtilityInterfaceData data = qvariant_cast<Game::SaveUtilityInterfaceData>(variant);

	UtilityTableName tablesName[5] = 
	{
		UtilityTableName::CATEGORIES,
		UtilityTableName::DEVELOPPERS,
		UtilityTableName::PUBLISHERS,
		UtilityTableName::PLATFORM,
		UtilityTableName::SERVICES
	};

	// Apply the utility into the SQL tables.
	QString statement = QString(
		"INSERT INTO \"%1\" (ItemID, UtilityID)\n"
		"VALUES");

	for (int i = 0; i < 5; i++)
	{
		QList<Game::SaveUtilityInterfaceItem>* pItem;
		if (tablesName[i] == UtilityTableName::CATEGORIES)
			pItem = &data.categories;
		else if (tablesName[i] == UtilityTableName::DEVELOPPERS)
			pItem = &data.developpers;
		else if (tablesName[i] == UtilityTableName::PUBLISHERS)
			pItem = &data.pubishers;
		else if (tablesName[i] == UtilityTableName::PLATFORM)
			pItem = &data.platform;
		else if (tablesName[i] == UtilityTableName::SERVICES)
			pItem = &data.services;
		
		for (long long int j = 0; j < pItem->size(); j+=10)
		{
			QString strData;
			for (long long int k = j; k < j+10 && k < pItem->size(); k++)
			{
				if (k > j)
					strData += ',';
				
				strData += QString("\n\t(%1, %2)")
					.arg(pItem->at(k).gameID)
					.arg(pItem->at(k).utilityID);
			}
			
			if (strData.size() > 0)
			{
				strData += ';';

#ifndef NDEBUG
				std::cout << (statement.arg(tableName(tablesName[i])) + strData).toLocal8Bit().constData() << std::endl << std::endl;
#endif

				if (!m_query.exec(statement.arg(tableName(tablesName[i])) + strData))
				{
					std::cerr << QString("Failed to set data into %1.\n\t%2")
						.arg(tableName(tablesName[i]), m_query.lastError().text())
						.toLocal8Bit().constData()
						<< std::endl;
					return false;
				}
				m_query.clear();
			}
		}
	}

	// Apply the Sensitive Content data into the SQL table.
	statement = QString(
		"INSERT INTO \"%1\" (SensitiveContentID, ItemID, ExplicitContent, ViolenceContent, BadLanguage)\n"
		"VALUES")
			.arg(tableName(UtilityTableName::SENSITIVE_CONTENT));

	for (long long int i = 0; i < data.sensitiveContent.size(); i+=10)
	{
		QString strData;
		for (long long int j = i; j < i+10 && j < data.sensitiveContent.size(); j++)
		{
			if (j > i)
				strData += ',';
			
			strData += QString("\n\t(%1, %2, %3, %4, %5)")
				.arg(data.sensitiveContent.at(j).SensitiveContentID)
				.arg(data.sensitiveContent.at(j).gameID)
				.arg(data.sensitiveContent.at(j).explicitContent)
				.arg(data.sensitiveContent.at(j).violenceContent)
				.arg(data.sensitiveContent.at(j).badLanguageContent);
		}
		if (strData.size() > 0)
		{
			strData += ';';

#ifndef NDEBUG
			std::cout << (statement + strData).toLocal8Bit().constData() << std::endl << std::endl;
#endif

			if (!m_query.exec(statement + strData))
			{
				std::cerr << QString("Failed to insert data into %1.\n\t%2")
					.arg(tableName(UtilityTableName::SENSITIVE_CONTENT), m_query.lastError().text())
					.toLocal8Bit().constData() << std::endl;
				return false;
			}
			m_query.clear();
		}
	}

	return true;
}

void TableModelGame_UtilityInterface::createTables()
{
	// Create the utilities interface tables (Categories, Developpers, Publishers, Platform, Services and Sensitive Content)
	QString statement = QString(
		"CREATE TABLE \"%1\" (\n"
		"	ItemID INTEGER,\n"
		"	UtilityID INTEGER);\n");
	
	// Create all the standard interface.
	UtilityTableName tablesName[5] =
	{
		UtilityTableName::CATEGORIES,
		UtilityTableName::DEVELOPPERS,
		UtilityTableName::PUBLISHERS,
		UtilityTableName::PLATFORM,
		UtilityTableName::SERVICES
	};

	for (int i = 0; i < 5; i++)
	{
		if (!m_query.exec(statement.arg(tableName(tablesName[i]))))
		{
			std::cerr << QString("Failed to create table \"%1\"\n\t%2")
				.arg(tableName(tablesName[i]), m_query.lastError().text())
				.toLocal8Bit().constData()
				<< std::endl;
			m_isTableReady = false;
			m_query.clear();
			return;
		}
		m_query.clear();
	}

	// Create the sensitive content data.
	statement = QString(
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
		std::cerr << QString("Failed to create table %1.\n\t%2")
			.arg(tableName(UtilityTableName::SENSITIVE_CONTENT), m_query.lastError().text())
			.toLocal8Bit().constData()
			<< std::endl;
		m_isTableReady = false;
		m_query.clear();
		return;
	}
	m_query.clear();

	m_isTableReady = true;
}

void TableModelGame_UtilityInterface::destroyTables()
{
	destroyTableByName(tableName(UtilityTableName::CATEGORIES));
	destroyTableByName(tableName(UtilityTableName::DEVELOPPERS));
	destroyTableByName(tableName(UtilityTableName::PUBLISHERS));
	destroyTableByName(tableName(UtilityTableName::PLATFORM));
	destroyTableByName(tableName(UtilityTableName::SERVICES));
	destroyTableByName(tableName(UtilityTableName::SENSITIVE_CONTENT));
}