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

#ifndef GAMESORTING_SERIESLISTVIEW_H_
#define GAMESORTING_SERIESLISTVIEW_H_

#include "AbstractListView.h"
#include "DataStruct.h"
#include "SqlUtilityTable.h"

#include <QWidget>
#include <QSqlDatabase>

class TableModelSeries;
class QTableView;
class QVBoxLayout;

class SeriesListView : public AbstractListView
{
    Q_OBJECT
public:
    explicit SeriesListView(const QString& tableName, ListType type, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent= nullptr);
    explicit SeriesListView(const QVariant& data, QSqlDatabase& db, SqlUtilityTable& utilityTable, QWidget* parent = nullptr);
    virtual ~SeriesListView();

    QString tableName() const;
    void setTableName(const QString& tableName);
    ListType listType() const;
    QVariant listData() const;
    virtual ViewType viewType() const override;

signals:
    void listEdited();

public slots:
    void addingItem();
    void deletingItems();

private slots:
    void setUrl();
    void openUrl();
    void moveItemUp();
    void moveItemDown();
    void moveItemTo();
    void filter();
    void copy();

private:
    void setupWidget();
    void setupView();
    void createMenu(QVBoxLayout* vLayout);
    void setColumnsSizeAndSortingOrder(const QVariant& data);
    void enableAction(QAction* action) const;

    QSqlDatabase& m_db;
    ListType m_type;
    QTableView* m_view;
    TableModelSeries* m_model;
    SqlUtilityTable& m_utilityTable;
};

#endif // GAMESORTING_SERIESLISTVIEW_H_