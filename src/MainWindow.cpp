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

#include "MainWindow.h"
#include "TabAndList.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QScreen>
#include <QMessageBox>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_db(QSqlDatabase::addDatabase("QSQLITE")),
	m_tabAndList(nullptr)
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
	createCentralWidget();
	createMenu();
	resize(800, 600);
	move((screen()->availableSize().width() - 800) / 2, (screen()->availableSize().height() - 600) / 2);
}

MainWindow::~MainWindow()
{
}
#include <QToolButton>
void MainWindow::createMenu()
{
	// Creation of all the menus and the toolbar.

	// Create the menu file.
	QMenu* menuFile = menuBar()->addMenu(tr("&File"));
	// Creating the toolbar
	QToolBar* fileToolBar = addToolBar(tr("File"));
	fileToolBar->setMovable(false);

	// Creating and adding the action.
	// Creating a new list.
	QMenu* newListMenu = new QMenu(tr("New"), this);
	// Game list.
	QAction* newGameListAct = new QAction(tr("Game list"), this);
	newGameListAct->setToolTip(tr("Creating a game list."));
	newListMenu->addAction(newGameListAct);
	connect(newGameListAct, &QAction::triggered, m_tabAndList, &TabAndList::newGameList);
	menuFile->addMenu(newListMenu);

	// Adding the newListMenu into the toolbar
	QIcon newIcon = QIcon(":/Images/New.svg");
	QToolButton* newListButton = new QToolButton();
	newListButton->setIcon(newIcon);
	newListButton->setMenu(newListMenu);
	newListButton->setPopupMode(QToolButton::InstantPopup);
	fileToolBar->addWidget(newListButton);

	// Opening a list file.
	QIcon openListIcon = QIcon(":/Images/Open.svg");
	QAction* openListAct = new QAction(openListIcon, tr("Open"), this);
	openListAct->setShortcut(QKeySequence::Open);
	openListAct->setToolTip(tr("Opening a list from a file"));
	menuFile->addAction(openListAct);
	fileToolBar->addAction(openListAct);

	// Save a list into a file.
	QIcon saveListIcon = QIcon(":/Images/Save.svg");
	QAction* saveListAct = new QAction(saveListIcon, tr("Save"), this);
	saveListAct->setShortcut(QKeySequence::Save);
	saveListAct->setToolTip(tr("Save a list into a file"));
	menuFile->addAction(saveListAct);
	fileToolBar->addAction(saveListAct);

	// Save as a list into a file.
	QAction* saveAsListAct = new QAction(tr("Save as"), this);
#ifdef WIN32
	saveAsListAct->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_S);
#elif
	saveAsListAct->setShortcut(QKeySequence::SaveAs);
#endif
	saveAsListAct->setToolTip(tr("Save a list into a new file."));
	menuFile->addAction(saveAsListAct);

	// Exitting the application.
	QIcon quitIcon = QIcon(":/Images/Exit.svg");
	QAction* quitAct = new QAction(quitIcon, tr("&Quit"), this);
	quitAct->setToolTip(tr("Exiting the application."));
#ifdef WIN32
	quitAct->setShortcut(Qt::CTRL | Qt::Key_Q);
#else
	quitAct->setShortcuts(QKeySequence::Quit);
#endif
	connect(quitAct, &QAction::triggered, this, &MainWindow::close);
	menuFile->addAction(quitAct);
}

void MainWindow::createCentralWidget()
{
	m_tabAndList = new TabAndList(m_db, this);
	setCentralWidget(m_tabAndList);
}