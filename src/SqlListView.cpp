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

#include "SqlListView.h"
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlError>

SqlListView::SqlListView(QWidget* parent) :
    QWidget(parent),
    m_view(new QTableView(this)),
    m_model(new QSqlRelationalTableModel(this))
{
    setupWidget();
    setupView();

    // Create TestTable table
    QSqlQuery defaultTableQuery(
        "CREATE TABLE TestTable ("
        "GameID INTEGER PRIMARY KEY,"
        "Name TEXT NOT NULL,"
        "Type TEXT NOT NULL,"
        "Rate INTEGER);");
    if (!defaultTableQuery.isActive())
        QMessageBox::critical(this, tr("Table creation error"), tr("Failed to create table TestTable.\n%1").arg(defaultTableQuery.lastError().text()), QMessageBox::Ok);
    defaultTableQuery.clear();

    // Insert data into table
    if (!defaultTableQuery.exec(
        "INSERT INTO TestTable (Name, Type, Rate) "
        "VALUES "
        "(\"Horizon Zero Dawn\", \"Adventure\", 5), "
        "(\"Subnautica\", \"Subwater\", 5), "
        "(\"Among Us\", \"Survival\", 5); "))
        QMessageBox::critical(this, tr("Table insertion error"), tr("Failed to insert data into table TestTable.\n%1").arg(defaultTableQuery.lastError().text()), QMessageBox::Ok);

    m_model->setTable("TestTable");
    m_model->select();
    m_view->setColumnHidden(0, true);

    if (!defaultTableQuery.exec(
        "INSERT INTO TestTable (Name, Type, Rate) "
        "VALUES "
        "(\"Horizon Zero Dawn\", \"Adventure\", 5), "
        "(\"Subnautica\", \"Subwater\", 5), "
        "(\"Among Us\", \"Survival\", 5); "))
        QMessageBox::critical(this, tr("Table insertion error"), tr("Failed to insert data into table TestTable.\n%1").arg(defaultTableQuery.lastError().text()), QMessageBox::Ok);
    m_model->select();
}

void SqlListView::setupWidget()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_view);
}

void SqlListView::setupView()
{
    m_view->setModel(m_model);
}