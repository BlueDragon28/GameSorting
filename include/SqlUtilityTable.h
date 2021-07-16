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
	QList<ItemUtilityData> retrieveTableData(UtilityTableName tableName, bool sort = false, Qt::SortOrder order = Qt::AscendingOrder) const;

	QVariant data() const;
	bool setData(const QVariant& data);

signals:
	void utilityEdited();

private:
	void createTables();
	void destroyTables();
	void destroyTableByName(const QString& tableName);
	void standardTableCreation(UtilityTableName tName);

	// Games
	void createGameTables();
	void destroyGameTables();
	void createCategoriesTable();
	void createDeveloppersTable();
	void createPublishersTable();
	void createPlatformTable();
	void createServicesTable();
	QVariant queryGameData() const;
	bool setGameData(const QVariant& data);
	bool setGameStandardData(UtilityTableName tName, const QList<ItemUtilityData>& data);

	static void errorMessageCreatingTable(const QString& tableName, const QString& queryError);

	ListType m_type;
	QSqlDatabase& m_db;
	QSqlQuery m_query;
	bool m_isTableReady;
};

#endif // GAMESORTING_SQLUTILITYTABLE_H_