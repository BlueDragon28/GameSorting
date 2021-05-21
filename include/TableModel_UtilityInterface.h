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

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>

class TableModel_UtilityInterface : public QObject
{
	Q_OBJECT
public:
	TableModel_UtilityInterface(const QString& parentTableName, QSqlDatabase& db);
	virtual ~TableModel_UtilityInterface();

	virtual QString tableName(UtilityTableName tableName) const = 0;
	virtual void newParentName(const QString& tableName) = 0;
	virtual bool isTableReady() const;
	virtual void rowRemoved(const QList<long long int>& itemID) = 0;
	virtual void updateItemUtility(long long int itemID, UtilityTableName tableName, const QVariant& data) = 0;
	virtual ListType listType() const = 0;

	virtual QVariant data() const = 0;

signals:
	void interfaceChanged(long long int itemID, UtilityTableName tableName);

protected:
	virtual bool setData(const QVariant& data) = 0;
	virtual void createTables() = 0;
	virtual void destroyTables() = 0;
	virtual void destroyTableByName(const QString& tableName);
	virtual void renameTable(const QString& currentName, const QString& newName);

	QString m_parentTableName;
	QSqlDatabase& m_db;
	QSqlQuery m_query;
	bool m_isTableReady;
};

#endif // GAMESORTING_TABLEMODEL_UTILITYINTERFACE_H_