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

#ifndef GAMESORTING_UTILITYINTERFACEEDITOR_MODEL_H_
#define GAMESORTING_UTILITYINTERFACEEDITOR_MODEL_H_

#include <QAbstractListModel>
#include <QList>
#include <QSqlQuery>
#include <QSqlDatabase>

#include "DataStruct.h"
#include "SqlUtilityTable.h"

class TableModel;
class TableModel_UtilityInterface;

class UtilityInterfaceEditorModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit UtilityInterfaceEditorModel(
        UtilityTableName utilityTableName,
        long long int itemID,
        TableModel* tableModel,
        TableModel_UtilityInterface* dataInterface,
        SqlUtilityTable& utilityData,
        QSqlDatabase& db,
        QObject* parent = nullptr);
    explicit UtilityInterfaceEditorModel(
        UtilityTableName utilityTableName,
        TableModel* tableModel,
        TableModel_UtilityInterface* dataInterface,
        SqlUtilityTable& utilityData,
        QSqlDatabase& db,
        QObject* parent = nullptr);
    virtual ~UtilityInterfaceEditorModel();

    // Interface between the view and the model.
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    // Apply the change made in the model into the utilityInterfaceTable.
    void applyChange();
    // Return the selected utilities.
    QList<long long int> getSelectedUtilities() const;

private:
    void retrieveUtilityData();
    bool isUtilityIDChecked(long long int utilityID) const;
    void removeCheckedUtilityID(long long int utilityID);

    QSqlDatabase& m_db;
    QSqlQuery m_query;

    // Variables to communicate with the data.
    // It's used to retrive all the utility (categories, platform, etc).
    UtilityTableName m_utilityTableName;
    long long int m_itemID;
    TableModel* m_tableModel;
    TableModel_UtilityInterface* m_dataInterface;
    SqlUtilityTable& m_utilityData;
    bool m_isSortingEnabled;
    Qt::SortOrder m_sortOrder;

    // Data
    QList<ItemUtilityData> m_utilityListData;
    QList<long long int> m_checkedIDList;
};

#endif