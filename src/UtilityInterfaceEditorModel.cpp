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

#include "UtilityInterfaceEditorModel.h"
#include "TableModel.h"
#include "TableModel_UtilityInterface.h"

#include <iostream>
#include <QSqlError>
#include <QList>
#include <QStringList>

UtilityInterfaceEditorModel::UtilityInterfaceEditorModel(
    UtilityTableName utilityTableName,
    long long int itemID,
    TableModel* tableModel,
    TableModel_UtilityInterface* dataInterface,
    SqlUtilityTable& utilityData,
    QSqlDatabase& db,
    QObject* parent) :
        QAbstractListModel(parent),
        m_db(db),
        m_query(m_db),
        m_utilityTableName(utilityTableName),
        m_itemID(itemID),
        m_tableModel(tableModel),
        m_dataInterface(dataInterface),
        m_utilityData(utilityData)
{
    retrieveUtilityData();
    retrieveSelectedUtilitiesOnItem();
}

UtilityInterfaceEditorModel::UtilityInterfaceEditorModel(
    UtilityTableName utilityTableName,
    TableModel* tableModel,
    TableModel_UtilityInterface* dataInterface,
    SqlUtilityTable& utilityData,
    QSqlDatabase& db,
    QObject* parent) :
    QAbstractListModel(parent),
    m_db(db),
    m_query(m_db),
    m_utilityTableName(utilityTableName),
    m_itemID(-1),
    m_tableModel(tableModel),
    m_dataInterface(dataInterface),
    m_utilityData(utilityData)
{
    retrieveUtilityData();
}

UtilityInterfaceEditorModel::~UtilityInterfaceEditorModel()
{}

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
            updateUtilitiesStr();
            return false;
        }
        else
        {
            m_checkedIDList.append(m_utilityListData.at(index.row()).utilityID);
            emit dataChanged(index, index, {role});
            updateUtilitiesStr();
            return true;
        }
    }
    
    return true;
}

QVariant UtilityInterfaceEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && role == Qt::DisplayRole && orientation == Qt::Horizontal)
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
    QList<ItemUtilityData> utilityData = m_utilityData.retrieveTableData(m_utilityTableName, m_isSortingEnabled, m_sortOrder, m_strFilter);
    if (utilityData.size() > 0)
    {
        m_utilityListData.resize(utilityData.size());
        for (int i = 0; i < m_utilityListData.size(); i++)
        {
            m_utilityListData[i] = utilityData.at(i);
        }
    }
    updateUtilitiesStr();
}

void UtilityInterfaceEditorModel::applyChange()
{
    // Apply the change made in the model into the SQL utility interface table.
    if (m_itemID < 0)
        return;

    m_query.clear();

    QList<long long int> interfaceData;
    for (int i = 0; i < m_checkedIDList.size(); i++)
        interfaceData.append(m_checkedIDList.at(i));
    
    m_dataInterface->updateItemUtility(m_itemID, m_utilityTableName, QVariant::fromValue(interfaceData));
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

QList<long long int> UtilityInterfaceEditorModel::getSelectedUtilities() const
{
    return m_checkedIDList;
}

void UtilityInterfaceEditorModel::sort(int column, Qt::SortOrder order)
{
    // Sorting the utilies list.
    if ((column == -1 ? false : true) != m_isSortingEnabled || order != m_sortOrder)
    {
        m_isSortingEnabled = column == -1 ? false : true;
        m_sortOrder = order;

        if (m_utilityListData.size() > 0)
        {
            beginRemoveRows(QModelIndex(), 0, m_utilityListData.size()-1);
            m_utilityListData.clear();
            endRemoveRows();
        }

        retrieveUtilityData();

        if (m_utilityListData.size() > 0)
        {
            beginInsertRows(QModelIndex(), 0, m_utilityListData.size()-1);
            endInsertRows();
        }
    }
}

void UtilityInterfaceEditorModel::setFilter(const QString& pattern)
{
    if (m_strFilter.compare(pattern) != 0)
    {
        m_strFilter = pattern;

        if (m_utilityListData.size() > 0)
        {
            beginRemoveRows(QModelIndex(), 0, m_utilityListData.size()-1);
            m_utilityListData.clear();
            endRemoveRows();
        }

        retrieveUtilityData();

        if (m_utilityListData.size() > 0)
        {
            beginInsertRows(QModelIndex(), 0, m_utilityListData.size()-1);
            endInsertRows();
        }
    }
}

void UtilityInterfaceEditorModel::retrieveSelectedUtilitiesOnItem()
{
    // When the user open the dialog on an item, this member function extract the already existing and apply then to the list.
    QString statement = QString(
        "SELECT\n"
        "   UtilityID\n"
        "FROM\n"
        "   \"%1\"\n"
        "WHERE\n"
        "   ItemID = %2;")
        .arg(m_dataInterface->tableName(m_utilityTableName))
        .arg(m_itemID);
    
#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << std::endl << std::endl;
#endif

    if (m_query.exec(statement))
    {
        while (m_query.next())
        {
            long long int utilityID = m_query.value(0).toLongLong();
            // Check if the utility is in the utility list, if not, it means it has been removed.
            foreach (const ItemUtilityData& data, m_utilityListData)
            {
                if (data.utilityID == utilityID)
                {
                    m_checkedIDList.append(utilityID);
                    break;
                }
            }
        }
        m_query.clear();
    }
    else
    {
        std::cout << QString("Failed to extract utilities of item %1 of the table %2.\n\t%3")
            .arg(m_itemID)
            .arg(m_dataInterface->tableName(m_utilityTableName), m_query.lastError().text())
            .toLocal8Bit().constData()
            << std::endl;
        m_query.clear();
    }
}

void UtilityInterfaceEditorModel::updateUtilitiesStr()
{
    // Retrieve a string of the selected utilities.
    QStringList selectedUtilitiesID;

    for (int i = 0; i < m_checkedIDList.size(); i++)
    {
        selectedUtilitiesID += QString::number(m_checkedIDList.at(i));
    }

    if (selectedUtilitiesID.isEmpty())
    {
        emit utilitiesUpdated(QString());
        return;
    }

    QString statement = QString(
        "SELECT\n"
        "   Name\n"
        "FROM\n"
        "   \"%1\"\n"
        "WHERE\n"
        "   \"%1ID\" = %2;")
            .arg(m_utilityData.tableName(m_utilityTableName));

    QString strSelectedUtilities;
    bool result = true;
    foreach (const QString& id, selectedUtilitiesID)
    {
#ifndef NDEBUG
    std::cout << statement.arg(id).toLocal8Bit().constData() << std::endl << std::endl;
#endif

        if (m_query.exec(statement.arg(id)))
        {
            if (m_query.next())
            {
                if (!strSelectedUtilities.isEmpty())
                    strSelectedUtilities += ", ";
                strSelectedUtilities += m_query.value(0).toString();
            }
        }
        else
        {
            std::cerr << "failed to retrieve selected utilies in UtilityInterfaceEditorModel::updateUtilitiesStr() member function." << std::endl;
            result = false;
        }
    }

    if (result)
        emit utilitiesUpdated(strSelectedUtilities);
}