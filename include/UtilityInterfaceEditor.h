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

#ifndef GAMESORTING_UTILITYINTERFACEEDITOR_H_
#define GAMESORTING_UTILITYINTERFACEEDITOR_H_

#include <QDialog>
#include <QSqlDatabase>

#include "DataStruct.h"
#include "UtilityInterfaceEditorModel.h"
#include "SqlUtilityTable.h"

class TableModel;
class TableModel_UtilityInterface;
class QTableView;

class UtilityInterfaceEditor : public QDialog
{
    Q_OBJECT
public:
    explicit UtilityInterfaceEditor(
        UtilityTableName utilityTableName,
        long long int itemID,
        TableModel* dataModel,
        TableModel_UtilityInterface* dataInterface,
        SqlUtilityTable& utilityData,
        QSqlDatabase& db,
        QWidget* parent = nullptr);
    virtual ~UtilityInterfaceEditor();
    
private:
    void createWidgets();
    void applyChange();

    UtilityTableName m_utilityTableName;
    long long int m_itemID;
    TableModel* m_dataModel;
    TableModel_UtilityInterface* m_dataIterface;
    SqlUtilityTable& m_utilityData;

    QTableView* m_tableView;
    UtilityInterfaceEditorModel* m_model;
};

#endif // GAMESORTING_UTILITYINTERFACEEDITOR_H_