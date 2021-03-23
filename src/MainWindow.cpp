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

#include "MainWindow.h"
#include "TabAndList.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QScreen>
#include <QMessageBox>
#include <QToolBar>
#include <QIcon>
#include <QAction>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_db(QSqlDatabase::addDatabase("QSQLITE")),
	m_tabAndList(new TabAndList(m_db, this))
{
	// Opening the database
	m_db.setDatabaseName(":memory:");
	if (!m_db.open())
	{
		QMessageBox::critical(this, tr("SQL Error"), tr("Failed to open sqlite database."), QMessageBox::Ok);
		std::exit(EXIT_FAILURE);
	}

	// Calling the methods for the creation of the menu and toolbar and also
	// set the QTableView as the central object of the MainWindow.
	createMenu();
	createCentralWidget();
	resize(800, 600);
	move((screen()->availableSize().width() - 800) / 2, (screen()->availableSize().height() - 600) / 2);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenu()
{
	QToolBar* editBar = addToolBar(tr("Edit"));

	QIcon addIcon(":/Images/Add.svg");
	QAction* addAct = new QAction(addIcon, tr("Add Item"), this);
	addAct->setToolTip(tr("Adding a new item into the current listed view."));
	connect(addAct, &QAction::triggered, m_tabAndList, &TabAndList::addingItem);
	editBar->addAction(addAct);
}

void MainWindow::createCentralWidget()
{
	setCentralWidget(m_tabAndList);
}