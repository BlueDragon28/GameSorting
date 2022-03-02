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

#ifndef GAMESORTING_UTILITYLISTMODEL_H_
#define GAMESORTING_UTILITYLISTMODEL_H_

#include "DataStruct.h"
#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QItemSelection>

class SqlUtilityTable;

class UtilityListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit UtilityListModel(SqlUtilityTable* utility, UtilityTableName tableName, QSqlDatabase& db, QObject* parent = nullptr);
    virtual ~UtilityListModel();

    // Interface between the view and the model
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& data, int role = Qt::EditRole) override;
    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    void queryTable();
    void appendRow();
    void deleteIndexs(const QModelIndexList& indexList);
    QItemSelection moveItemUp(const QModelIndexList& indexList);
    QItemSelection moveItemDown(const QModelIndexList& indexList);
    QItemSelection moveItemTo(const QModelIndexList& indexList, int to);

private:
    void updateOrder(int from = -1);

    SqlUtilityTable* m_utility;
    UtilityTableName m_tableName;
    QList<ItemUtilityData> m_data;
    QSqlDatabase m_db;
    QSqlQuery m_query;
};

#endif // GAMESORTING_UTILITYLISTMODEL_H_