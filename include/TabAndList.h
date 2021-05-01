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

#ifndef GAMESORTING_TABANDLIST_H
#define GAMESORTING_TABANDLIST_H

#include "DataStruct.h"
#include "SqlUtilityTable.h"

#include <QWidget>
#include <QSqlDatabase>
#include <QTabBar>

class QTabBar;
class SqlListView;
class QStackedLayout;

class TabAndList : public QWidget
{
    Q_OBJECT
public:
    explicit TabAndList(QSqlDatabase& db, QWidget* parent = nullptr);
    virtual ~TabAndList();

public slots:
    void newGameList();
    void open();
    void saveAs();
    void openUtility(UtilityTableName tableName);

signals:
    void newList(ListType listType);

private slots:
    void tabChanged(int index);
    void addTable();
    void removeTable(int index);

private:
    void setupView();
    bool saveFile(const QString& filePath) const;
    bool openFile(const QString& filePath);

    QSqlDatabase& m_db;
    ListType m_listType;
    SqlUtilityTable m_sqlUtilityTable;
    QTabBar* m_tabBar;
    QStackedLayout* m_stackedViews;
    QString m_filePath;
};

#endif