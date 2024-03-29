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

#ifndef GAMESORTING_FILTERDIALOG_H_
#define GAMESORTING_FILTERDIALOG_H_

#include "DataStruct.h"
#include "SqlUtilityTable.h"

#include <QDialog>
#include <QSqlDatabase>

class QVBoxLayout;
class UtilityInterfaceEditorModel;
class StarWidget;
class TableModel_UtilityInterface;
class TableModel;
class QTableView;
class QLineEdit;
class QStackedLayout;

class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FilterDialog(
        TableModel* model,
        TableModel_UtilityInterface* interface,
        SqlUtilityTable& utility,
        QSqlDatabase& db,
        QWidget* parent = nullptr);
    
    void createWidget();
    void applyFilter();

signals:
    void tabChanged();

private:
    void comboBoxChanged(int index);
    void searchUtilities(const QString& pattern);

    TableModel* m_model;
    TableModel_UtilityInterface* m_interface;
    SqlUtilityTable& m_utility;
    QSqlDatabase& m_db;
    QStackedLayout* m_stackedLayout;
    QLineEdit* m_nameText;
    QVBoxLayout* m_utilityVLayout;
    UtilityInterfaceEditorModel* m_utilityModel;
    QTableView* m_utilityView;
    StarWidget* m_starWidget;
    int m_lastIndex;
};

#endif // GAMESORTING_FILTERDIALOG_H_