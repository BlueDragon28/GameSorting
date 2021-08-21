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

#ifndef GAMESORTING_TABLEMODEL_H_
#define GAMESORTING_TABLEMODEL_H_

#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>

#include "DataStruct.h"
#include "SqlUtilityTable.h"

#define SORTING_ORDER(order, string) \
    switch (order) \
    { \
    case Qt::AscendingOrder: \
    { \
        string = string.arg("ASC"); \
    } break; \
    case Qt::DescendingOrder: \
    { \
        string = string.arg("DESC"); \
    } break; \
    }

class TableModel_UtilityInterface;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent = nullptr);
    TableModel(QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent = nullptr);
    virtual ~TableModel();

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    virtual void appendRows(int count = 1) = 0;
    virtual void deleteRows(const QModelIndexList& indexList) = 0;
    virtual int size() const = 0;
    QString tableName() const;
    void setTableName(const QString& tableName);
    QString rawTableName() const;
    virtual ListType listType() const = 0;
    virtual long long int itemID(const QModelIndex& index) const = 0;

    virtual void updateQuery() = 0;
    virtual QVariant retrieveData() const = 0;
    virtual bool setItemData(const QVariant& data) = 0;
    virtual void setFilter(const ListFilter& filter);
    virtual bool isSortingEnabled() const;
    virtual bool isFilterEnabled() const;

    virtual TableModel_UtilityInterface* utilityInterface() = 0;

signals:
    void listEdited();
    void tableNameChanged(const QString& tableName);
    void sortingChanged(bool value);
    void filterChanged(bool value);

protected:
    virtual void createTable() = 0;
    virtual void deleteTable() = 0;
    virtual void utilityChanged(long long int itemID, UtilityTableName tableName) = 0;
    QString checkingIfNameFree(const QString& name, int n = -1) const;

    QSqlDatabase& m_db;
    SqlUtilityTable& m_utilityTable;
    QSqlQuery m_query;
    QString m_tableName;
    bool m_isTableCreated, m_isTableChanged;
    ListFilter m_listFilter;
    int m_sortingColumnID;
    Qt::SortOrder m_sortingOrder;
};

#endif // GAMESORTING_TABLEMODEL_H_