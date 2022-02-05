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
	case UtilityTableName::DIRECTOR:
		return "Director";
	case UtilityTableName::ACTORS:
		return "Actors";
	case UtilityTableName::PRODUCTION:
		return "Production";
	case UtilityTableName::MUSIC:
		return "Music";
	case UtilityTableName::AUTHORS:
		return "Authors";
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
	else if (m_type == ListType::MOVIESLIST)
	{
		m_isTableReady = true;
		createMoviesTables();
	}
	else if (m_type == ListType::COMMONLIST)
	{
		m_isTableReady = true;
		createCommonTables();
	}
	else if (m_type == ListType::BOOKSLIST)
	{
		m_isTableReady = true;
		createBooksTables();
	}
	else
		m_isTableReady = false;
}

void SqlUtilityTable::destroyTables()
{
	if (m_type == ListType::GAMELIST)
		destroyGameTables();
	else if (m_type == ListType::MOVIESLIST)
		destroyMoviesTables();
	else if (m_type == ListType::COMMONLIST)
		destroyCommonTables();
	else if (m_type == ListType::BOOKSLIST)
		destroyBooksTables();
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

QList<ItemUtilityData> SqlUtilityTable::retrieveTableData(UtilityTableName tableName, bool sort, Qt::SortOrder order, const QString& searchPattern) const
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
		"%4"
		"ORDER BY\n"
		"	%2 %3;")
			.arg(this->tableName(tableName));

	if (sort)
		statement = statement.arg("Name", (order == Qt::AscendingOrder ? "ASC" : "DESC"));
	else
		statement = statement.arg("OrderID", "ASC");
	
	if (!searchPattern.isEmpty())
	{
		QString searchStatement = QString(
			"WHERE\n"
			"	Name LIKE \"%%1%\"\n")
			.arg(searchPattern);
		statement = statement.arg(searchStatement);
	}
	else
		statement = statement.arg("");
	
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
		else if (m_type == ListType::MOVIESLIST)
			return queryMoviesData();
		else if (m_type == ListType::COMMONLIST)
			return queryCommonsData();
		else if (m_type == ListType::BOOKSLIST)
			return queryBooksData();
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
	else if (variant.canConvert<Movie::SaveUtilityData>())
	{
		newList(ListType::MOVIESLIST);
		return setMoviesData(variant);
	}
	else if (variant.canConvert<Common::SaveUtilityData>())
	{
		newList(ListType::COMMONLIST);
		return setCommonsData(variant);
	}
	else if (variant.canConvert<Books::SaveUtilityData>())
	{
		newList(ListType::BOOKSLIST);
		return setBooksData(variant);
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

bool SqlUtilityTable::setStandardData(UtilityTableName tName, const QList<ItemUtilityData>& data)
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