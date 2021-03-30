/*
* MIT License
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

#ifndef GAMESORTING_SQLLISTVIEW_H
#define GAMESORTING_SQLLISTVIEW_H

#include "DataStruct.h"
#include "SqlUtilityTable.h"

#include <QWidget>
#include <QSqlDatabase>

class TableModel;
class QTableView;
class QVBoxLayout;

class SqlListView : public QWidget
{
    Q_OBJECT
public:
    explicit SqlListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent = nullptr);
    ~SqlListView();

    QString tableName() const;

public slots:
    void addingItem();
    void deletingItems();
    
protected:

private:
    void setupWidget();
    void setupView();
    void createMenu(QVBoxLayout* vLayout);

    QSqlDatabase& m_db;
    ListType m_type;
    QTableView* m_view;
    TableModel* m_model;
};

#endif