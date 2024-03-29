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

#ifndef GAMESORTING_TABLEMODELCOMMON_H_
#define GAMESORTING_TABLEMODELCOMMON_H_

#include "TableModel.h"
#include <QItemSelection>

class TableModelCommon_UtilityInterface;

class TableModelCommon : public TableModel
{
    Q_OBJECT
public:
    TableModelCommon(const QString& tableName, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent = nullptr);
    TableModelCommon(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QObject* parent = nullptr);
    virtual ~TableModelCommon();

    // Data interface
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& data, int role = Qt::EditRole) override;
    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    virtual void appendRows(int count = 1) override;
    virtual void appendRows(const QModelIndexList& indexList, int count = 1) override;
    virtual void appendRows(const QModelIndexList& indexList, const QStringList& commonList) override;
    virtual void deleteRows(const QModelIndexList& indexList) override;
    virtual int size() const override;
    virtual ListType listType() const override;
    virtual long long int itemID(const QModelIndex& index) const override;

    virtual void updateQuery() override;
    virtual QVariant retrieveData() const override;
    virtual bool setItemData(const QVariant& data) override;

    QString url(const QModelIndex& index) const;
    void setUrl(const QModelIndex& index, const QString& url);

    virtual TableModel_UtilityInterface* utilityInterface();

    QItemSelection moveItemsUp(const QModelIndexList& indexList);
    QItemSelection moveItemsDown(const QModelIndexList& indexList);
    QItemSelection moveItemsTo(const QModelIndexList& indexList, int to);

    void copyToClipboard(QModelIndexList indexList);

protected:
    virtual void createTable() override;
    virtual void deleteTable() override;
    virtual void utilityChanged(long long int itemID, UtilityTableName tableName) override;

private:
    template<typename T>
    bool updateField(const QString& columnName, int rowNB, T value);

    void queryUtilityField(UtilityTableName tableName);
    void queryUtilityField(UtilityTableName tableName, long long int commonID);
    void querySeriesField();
    void querySeriesField(long long int commonID);
    void queryCategoriesField();
    void queryCategoriesField(long long int commonID);
    void queryAuthorsField();
    void queryAuthorsField(long long int commonID);
    void querySensitiveContentField();
    void querySensitiveContentField(long long int commonID);
    int findCommonPos(long long int commonID) const;
    void sortUtility(int column);

    void updateCommonPos(int from = -1);

    int retrieveMaxPos();
    void retrieveInsertedRows(int row, int count = 1);

    TableModelCommon_UtilityInterface* m_interface;
    QList<CommonItem> m_data;
};

#endif // GAMESORTING_TABLEMODELCOMMON_H_