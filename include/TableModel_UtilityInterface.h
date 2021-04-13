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

#ifndef GAMESORTING_TABLEMODEL_UTILITYINTERFACE_H_
#define GAMESORTING_TABLEMODEL_UTILITYINTERFACE_H_

#include "DataStruct.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class TableModel_UtilityInterface
{
	TableModel_UtilityInterface(const TableModel_UtilityInterface&) = delete;
public:
	TableModel_UtilityInterface(const QString& parentTableName, ListType type, QSqlDatabase& db);
	~TableModel_UtilityInterface();

	QString tableName(UtilityTableName tableName) const;
	void rowRemoved(const QList<long long int>& itemsID);
	void updateItemUtility(long long int itemID, UtilityTableName tableName, const QList<long long int>& dataList);
	void updateItemUtility(long long int itemID, UtilityTableName tableName, int explicitContent, int violenceContent, int languageContent);

private:
	void createTables();
	void destroyTables();
	void destroyTableByName(const QString& tableName);

	// Games
	void createGameTables();
	void destroyGameTables();
	void createCategoriesTable();
	void createDeveloppersTable();
	void createPublishersTable();
	void createPlatformTable();
	void createServicesTable();
	void createSensitiveContentTable();
	void gameRowRemoved(const QList<long long int>& itemsID);

	static void printTableCreationError(const QString& tableName, const QString& messageError);

	QString m_parentTableName;
	ListType m_listType;
	QSqlDatabase& m_db;
	QSqlQuery m_query;
	bool m_isTableReady;
};

#endif // GAMESORTING_TABLEMODEL_UTILITYINTERFACE_H_