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

class TableModel_UtilityInterface;
class UtilitySensitiveContentEditor;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent = nullptr);
    explicit TableModel(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent = nullptr);
    virtual ~TableModel();

    // Data interface.
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool appendRows(int count = 1);
    void deleteRows(const QModelIndexList& indexList);

    // Data information.
    int size() const;
    QString tableName() const;
    QString rowTableName() const;
    ListType listType() const;

    // Allow access to the user of the utilityInterface.
    TableModel_UtilityInterface* utilityInterface();

    // Getting the itemID from a index.
    long long int itemID(const QModelIndex& index) const;

    // Headers column's name.
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void updateQuery();

    QVariant retrieveData() const;
    bool setItemsData(const QVariant& data);

protected:
    void createTable();
    void deleteSqlTable();
    void utilityChanged(long long int itemID, UtilityTableName tableName);

    // Games specific member functions.
    void gameCreateTable();
    int gameColumnCount() const;
    int gameRowCount() const;
    QVariant gameData(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool gameSetData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool gameInsertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool gameRemoveRows(int row, int count, const QModelIndex& parent = QModelIndex());
    QVariant gameHeaderData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    long long int gameItemID(const QModelIndex& index) const;
    template<typename T>
    bool gameUpdateField(QSqlQuery& query, const QString& columnName, int rowNB, T value);
    void gameUpdateQuery();
    void gameQueryUtilityField(UtilityTableName tableName);
    void gameQueryUtilityField(long long int gameID, UtilityTableName tableName);
    void gameQueryCategoriesField();
    void gameQueryCategoriesField(long long int gameID);
    void gameQueryDeveloppersField();
    void gameQueryDeveloppersField(long long int gameID);
    void gameQueryPublishersField();
    void gameQueryPublishersField(long long int gameID);
    void gameQueryPlatformField();
    void gameQueryPlatformField(long long int gameID);
    void gameQueryServicesField();
    void gameQueryServicesField(long long int gameID);
    void gameQuerySensitiveContentField();
    void gameQuerySensitiveContentField(long long int gameID);
    QVariant gameRetrieveData() const;
    bool setGameItemsData(const QVariant& data);
    void gameUtilityChanged(long long int gameID, UtilityTableName tableName);
    int findGamePos(long long int gameID) const;

private:
    QString checkingIfNameFree(const QString& name, int n = -1) const;

    QSqlDatabase& m_db;
    SqlUtilityTable& m_utilityTable;
    TableModel_UtilityInterface* m_utilityInterface;
    QSqlQuery m_query;
    QString m_tableName;
    ListType m_listType;
    QList<GameItem>* m_gameListData;
    bool m_isTableCreated, m_isTableChanged;
    int m_listCount;
};

#endif // GAMESORTING_TABLEMODEL_H_