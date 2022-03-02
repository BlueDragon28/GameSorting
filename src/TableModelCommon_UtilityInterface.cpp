/*
* MIT Licence
*
* This file is part of the GameSorting
*
* Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
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

#include "TableModelCommon_UtilityInterface.h"
#include <iostream>
#include <QSqlError>

TableModelCommon_UtilityInterface::TableModelCommon_UtilityInterface(const QString& parentName, QSqlDatabase& db) :
    TableModel_UtilityInterface(parentName, db)
{
    createTables();
}

TableModelCommon_UtilityInterface::TableModelCommon_UtilityInterface(const QString& parentName, QSqlDatabase& db, const QVariant& variant) :
    TableModelCommon_UtilityInterface(parentName, db)
{
    setData(variant);
}

TableModelCommon_UtilityInterface::~TableModelCommon_UtilityInterface()
{
    destroyTables();
}

QString TableModelCommon_UtilityInterface::tableName(UtilityTableName tableName) const
{
    // Return the table name of a specific utility.
    switch (tableName)
    {
	case UtilityTableName::SERIES:
		return m_parentTableName + "_Series";
    case UtilityTableName::CATEGORIES:
        return m_parentTableName + "_Categories";
    case UtilityTableName::AUTHORS:
        return m_parentTableName + "_Authors";
	case UtilityTableName::SENSITIVE_CONTENT:
		return m_parentTableName + "_SensitiveContent";
    default:
        return QString();
    }
}

void TableModelCommon_UtilityInterface::newParentName(const QString& newParentName)
{
    // Replace the parent table name of the interface table with the new parent table name.
    if (!m_isTableReady)
        return;
    
    // Storing the current table name into variables.
	QString serCurName = tableName(UtilityTableName::SERIES);
    QString catCurName = tableName(UtilityTableName::CATEGORIES);
    QString autCurName = tableName(UtilityTableName::AUTHORS);
	QString senCurName = tableName(UtilityTableName::SENSITIVE_CONTENT);

    // Changing m_parentTableName to the new parent name.
    m_parentTableName = newParentName;

    // Get the new table name.
	QString serNewName = tableName(UtilityTableName::SERIES);
    QString catNewName = tableName(UtilityTableName::CATEGORIES);
    QString autNewName = tableName(UtilityTableName::AUTHORS);
	QString senNewName = tableName(UtilityTableName::SENSITIVE_CONTENT);

    // Renaming the tables
	renameTable(serCurName, serNewName);
    renameTable(catCurName, catNewName);
    renameTable(autCurName, autNewName);
	renameTable(senCurName, senNewName);
}

void TableModelCommon_UtilityInterface::rowRemoved(const QList<long long int>& commonIDs)
{
    // Deleting any row of each utility tables referrencing a commonID inside commonIDs.
    if (!m_isTableReady || commonIDs.isEmpty())
        return;
    
    QString idList;
    for (int i = 0; i < commonIDs.size(); i++)
    {
        if (i > 0)
            idList += ", ";
        idList += QString::number(commonIDs.at(i));
    }

    UtilityTableName tablesName[4] =
    {
		UtilityTableName::SERIES,
        UtilityTableName::CATEGORIES,
        UtilityTableName::AUTHORS,
		UtilityTableName::SENSITIVE_CONTENT
    };

    for (int i = 0; i < 4; i++)
    {
        QString statement = QString(
            "DELETE FROM \"%1\"\n"
            "WHERE\n")
                .arg(tableName(tablesName[i]));
        
        if (commonIDs.size() == 1)
            statement += QString("  ItemID = %1;").arg(idList);
        else
            statement += QString("  ItemID IN (%1);").arg(idList);
    
#ifndef NDEBUG
        std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (!m_query.exec(statement))
            std::cerr << QString("Failed to delete commonID (%1) from the table %2.\n\t%3")
                .arg(idList, tableName(tablesName[i]), m_query.lastError().text())
                .toLocal8Bit().constData()
                << std::endl;
        m_query.clear();
    }
}

void TableModelCommon_UtilityInterface::updateItemUtility(long long int commonID, UtilityTableName tableName, const QVariant& data)
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
			.arg(commonID);
	
#ifndef NDEBUG
	std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

	if (!m_query.exec(statement))
	{
		std::cerr << QString("Failed to remove all the utility of the table %1 where ItemID is equal to %2.\n\t%3")
			.arg(this->tableName(tableName))
			.arg(commonID)
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
				statement += QString("\n	(%1, %2)").arg(commonID).arg(utilitiesID.at(i));
			}
			statement += ';';

#ifndef NDEBUG
			std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

			if (m_query.exec(statement))
				emit interfaceChanged(commonID, tableName);
			else
				std::cerr << QString("Failed to insert data into %1.\n\t%2")
					.arg(this->tableName(tableName), m_query.lastError().text())
					.toLocal8Bit().constData()
					<< std::endl;
			m_query.clear();
		}
		else
			emit interfaceChanged(commonID, tableName);
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
				.arg(commonID)
				.arg(sensData.explicitContent)
				.arg(sensData.violenceContent)
				.arg(sensData.badLanguageContent);
		
#ifndef NDEBUG
		std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

		if (m_query.exec(statement))
			emit interfaceChanged(commonID, tableName);
		else
			std::cerr << QString("Failed to insert data into %1.\n\t%2")
				.arg(this->tableName(tableName), m_query.lastError().text())
				.toLocal8Bit().constData()
				<< std::endl;
		m_query.clear();
	}
}

ListType TableModelCommon_UtilityInterface::listType() const
{
    return ListType::COMMONLIST;
}

QVariant TableModelCommon_UtilityInterface::data() const
{
    // Retrieve the data of the utility interface to be save into a file.
	if (!m_isTableReady)
		return QVariant();

	Common::SaveUtilityInterfaceData data = {};

	QSqlQuery query(m_db);
	QString statement = QString(
		"SELECT\n"
		"	ItemID,\n"
		"	UtilityID\n"
		"FROM\n"
		"	\"%1\"\n"
		"ORDER BY\n"
		"	ItemID;");
	
	UtilityTableName tablesName[3] =
	{
		UtilityTableName::SERIES,
		UtilityTableName::CATEGORIES,
		UtilityTableName::AUTHORS
	};

	// Utility interface (Categories, Authors).
	for (int i = 0; i < 3; i++)
	{
		if (!query.exec(statement.arg(tableName(tablesName[i]))))
			return QVariant();
		
		while (query.next())
		{
			Game::SaveUtilityInterfaceItem item = {};
			item.gameID = query.value(0).toLongLong();
			item.utilityID = query.value(1).toLongLong();
			if (tablesName[i] == UtilityTableName::SERIES)
				data.series.append(item);
			if (tablesName[i] == UtilityTableName::CATEGORIES)
				data.categories.append(item);
			else if (tablesName[i] == UtilityTableName::AUTHORS)
				data.authors.append(item);
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

bool TableModelCommon_UtilityInterface::setData(const QVariant& variant)
{
	// Apply the data into the SQL table.
	if (!variant.isValid())
		return false;
	
	Common::SaveUtilityInterfaceData data = qvariant_cast<Common::SaveUtilityInterfaceData>(variant);

	UtilityTableName tablesName[3] = 
	{
		UtilityTableName::SERIES,
		UtilityTableName::CATEGORIES,
		UtilityTableName::AUTHORS
	};

	// Apply the utility into the SQL tables.
	QString statement = QString(
		"INSERT INTO \"%1\" (ItemID, UtilityID)\n"
		"VALUES");

	for (int i = 0; i < 3; i++)
	{
		QList<Game::SaveUtilityInterfaceItem>* pItem;
		if (tablesName[i] == UtilityTableName::SERIES)
			pItem = &data.series;
		else if (tablesName[i] == UtilityTableName::CATEGORIES)
			pItem = &data.categories;
		else if (tablesName[i] == UtilityTableName::AUTHORS)
			pItem = &data.authors;
		
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

void TableModelCommon_UtilityInterface::createTables()
{
    // Create the utilities interface tables (Categories, Authors and Sensitive Content).
    QString statement = QString(
        "CREATE TABLE \"%1\" (\n"
        "   ItemID INTEGER,\n"
        "   UtilityID INTEGER);");
    
    // Create all the standard interface.
    UtilityTableName tablesName[3] = 
    {
		UtilityTableName::SERIES,
        UtilityTableName::CATEGORIES,
        UtilityTableName::AUTHORS
    };

    for (int i = 0; i < 3; i++)
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

    // Create the Sensitive Content data.
    statement = QString(
        "CREATE TABLE \"%1\" (\n"
        "   SensitiveContentID INTEGER PRIMARY KEY,\n"
        "   ItemID INTEGER,\n"
        "   ExplicitContent INTEGER,\n"
        "   ViolenceContent INTEGER,\n"
        "   BadLanguage INTEGER);")
            .arg(tableName(UtilityTableName::SENSITIVE_CONTENT));

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (!m_query.exec(statement))
    {
        std::cerr << QString("Failed to create table %1.\n\t%2")
            .arg(tableName(UtilityTableName::SENSITIVE_CONTENT),
			m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
        m_isTableReady = false;
        m_query.clear();
        return;
    }
    m_query.clear();

    m_isTableReady = true;
}

void TableModelCommon_UtilityInterface::destroyTables()
{
	destroyTableByName(tableName(UtilityTableName::SERIES));
    destroyTableByName(tableName(UtilityTableName::CATEGORIES));
    destroyTableByName(tableName(UtilityTableName::AUTHORS));
	destroyTableByName(tableName(UtilityTableName::SENSITIVE_CONTENT));
}