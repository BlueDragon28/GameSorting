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

#ifndef GAMESORTING_SQLUTILITYTABLE_H_
#define GAMESORTING_SQLUTILITYTABLE_H_

#include "DataStruct.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QVariant>

class SqlUtilityTable : public QObject
{
	Q_OBJECT
public:
	SqlUtilityTable(ListType type, QSqlDatabase& db);
	virtual ~SqlUtilityTable();

	void newList(ListType type);
	static QString tableName(UtilityTableName tableName);
	QList<ItemUtilityData> retrieveTableData(UtilityTableName tableName, bool sort = false, Qt::SortOrder order = Qt::AscendingOrder, const QString& searchPattern = QString()) const;

	QVariant data() const;
	bool setData(const QVariant& data);

	long long int addItem(UtilityTableName tableName, const QString& name);

signals:
	void utilityEdited();

private:
	void createTables();
	void destroyTables();
	void destroyTableByName(const QString& tableName);
	void standardTableCreation(UtilityTableName tName);
	bool setStandardData(UtilityTableName tName, const QList<ItemUtilityData>& data);

	// Games
	void createGameTables();
	void destroyGameTables();
	void createSeriesTable();
	void createCategoriesTable();
	void createDeveloppersTable();
	void createPublishersTable();
	void createPlatformTable();
	void createServicesTable();
	QVariant queryGameData() const;
	bool setGameData(const QVariant& data);

	// Movies
	void createMoviesTables();
	void destroyMoviesTables();
	void createDirectoryTable();
	void createActorsTable();
	void createProductionsTable();
	void createMusicTable();
	QVariant queryMoviesData() const;
	bool setMoviesData(const QVariant& data);

	// Common
	void createCommonTables();
	void destroyCommonTables();
	void createAuthorsTables();
	QVariant queryCommonsData() const;
	bool setCommonsData(const QVariant& data);

	// Books
	void createBooksTables();
	void destroyBooksTables();
	QVariant queryBooksData() const;
	bool setBooksData(const QVariant& data);

	// Series
	void createSeriesTables();
	void destroySeriesTables();
	QVariant querySeriesData() const;
	bool setSeriesData(const QVariant& data);

	static void errorMessageCreatingTable(const QString& tableName, const QString& queryError);

	ListType m_type;
	QSqlDatabase& m_db;
	QSqlQuery m_query;
	bool m_isTableReady;
};

#endif // GAMESORTING_SQLUTILITYTABLE_H_