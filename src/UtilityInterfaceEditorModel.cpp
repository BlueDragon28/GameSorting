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

#include "UtilityInterfaceEditorModel.h"
#include "TableModel.h"
#include "TableModel_UtilityInterface.h"

#include <iostream>
#include <QSqlError>
#include <QList>

UtilityInterfaceEditorModel::UtilityInterfaceEditorModel(
    UtilityTableName utilityTableName,
    long long int itemID,
    TableModel* tableModel,
    TableModel_UtilityInterface* dataInterface,
    SqlUtilityTable& utilityData,
    QSqlDatabase& db,
    QObject* parent) :
        m_db(db),
        m_query(m_db),
        QAbstractListModel(parent),
        m_utilityTableName(utilityTableName),
        m_itemID(itemID),
        m_tableModel(tableModel),
        m_dataInterface(dataInterface),
        m_utilityData(utilityData)
{
    retrieveUtilityData();
}

int UtilityInterfaceEditorModel::rowCount(const QModelIndex& parent) const
{
    // Return the number of row in the model.
    return m_utilityListData.size();
}

QVariant UtilityInterfaceEditorModel::data(const QModelIndex& index, int role) const
{
    // Return 
    if ((index.isValid() && index.row() < rowCount()))
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole) 
            return m_utilityListData.at(index.row()).name;
        else if (role == Qt::CheckStateRole)
            return isUtilityIDChecked(m_utilityListData.at(index.row()).utilityID);
    }

    return QVariant();
}

bool UtilityInterfaceEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        //m_utilityListData[index.row()].isChecked = !m_utilityListData.at(index.row()).isChecked;
        if (isUtilityIDChecked(m_utilityListData.at(index.row()).utilityID))
        {
            removeCheckedUtilityID(m_utilityListData.at(index.row()).utilityID);
            emit dataChanged(index, index, {role});
            return false;
        }
        else
        {
            m_checkedIDList.append(m_utilityListData.at(index.row()).utilityID);
            emit dataChanged(index, index, {role});
            return true;
        }
    }
    
    return true;
}

QVariant UtilityInterfaceEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0)
        return "Name";
    return QVariant();
}

Qt::ItemFlags UtilityInterfaceEditorModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsUserCheckable | QAbstractListModel::flags(index);
}

void UtilityInterfaceEditorModel::retrieveUtilityData()
{
    // Retrieving all the data of the utility data for the editing.
    QList<ItemUtilityData> utilityData = m_utilityData.retrieveTableData(m_utilityTableName);
    if (utilityData.size() > 0)
    {
        m_utilityListData.resize(utilityData.size());
        for (int i = 0; i < m_utilityListData.size(); i++)
        {
            m_utilityListData[i] = utilityData.at(i);
        }
    }
}

void UtilityInterfaceEditorModel::applyChange()
{
    // Apply the change made in the model into the SQL utility interface table.
    m_query.clear();

    QList<long long int> interfaceData;
    for (int i = 0; i < m_checkedIDList.size(); i++)
        interfaceData.append(m_checkedIDList.at(i));
    
    m_dataInterface->updateItemUtility(m_itemID, m_utilityTableName, interfaceData);
}

bool UtilityInterfaceEditorModel::isUtilityIDChecked(long long int utilityID) const
{
    // Check if utilityID is in the m_checkedIDList list.
    for (int i = 0; i < m_checkedIDList.size(); i++)
        if (m_checkedIDList.at(i) == utilityID)
            return true;
    return false;
}

void UtilityInterfaceEditorModel::removeCheckedUtilityID(long long int utilityID)
{
    // Remove from the m_checkedIDList list all the element with utilityID.
    for (int i = m_checkedIDList.size()-1; i >= 0; i--)
        if (m_checkedIDList.at(i) == utilityID)
            m_checkedIDList.remove(i, 1);
}