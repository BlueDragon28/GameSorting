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
#include <QCloseEvent>
#include <QSettings>
#include <QScreen>
#include <QSize>

MainWindow::MainWindow(const QString& filePath, bool resetSettings, QWidget* parent) :
	QMainWindow(parent),
	m_db(QSqlDatabase::addDatabase("QSQLITE")),
	m_tabAndList(nullptr),
	m_listToolBar(nullptr),
	m_listChanged(false),

	m_isResetSettings(resetSettings)
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
	updateWindowTitle();
	readSettings();

	if (!filePath.isEmpty())
		m_tabAndList->open(filePath);
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
	connect(openListAct, &QAction::triggered, m_tabAndList, qOverload<>(&TabAndList::open));
	fileToolBar->addAction(openListAct);

	// Save a list into a file.
	QIcon saveListIcon = QIcon(":/Images/Save.svg");
	QAction* saveListAct = new QAction(saveListIcon, tr("Save"), this);
	saveListAct->setShortcut(QKeySequence::Save);
	saveListAct->setToolTip(tr("Save a list into a file"));
	menuFile->addAction(saveListAct);
	connect(saveListAct, &QAction::triggered, m_tabAndList, &TabAndList::save);
	fileToolBar->addAction(saveListAct);

	// Save as a list into a file.
	QAction* saveAsListAct = new QAction(tr("Save as"), this);
#ifdef WIN32
	saveAsListAct->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_S);
#else
	saveAsListAct->setShortcut(QKeySequence::SaveAs);
#endif
	saveAsListAct->setToolTip(tr("Save a list into a new file."));
	connect(saveAsListAct, &QAction::triggered, m_tabAndList, &TabAndList::saveAs);
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
	connect(m_tabAndList, &TabAndList::newList, this, &MainWindow::newListCreated);
	connect(m_tabAndList, &TabAndList::newListFileName, this, &MainWindow::listFilePathChanged);
	connect(m_tabAndList, &TabAndList::listChanged, this, &MainWindow::listChanged);
	setCentralWidget(m_tabAndList);
}

void MainWindow::createGameToolBar()
{
	// Creating the toolbar next to the file toolbar.
	// This toolbar is used to open the utility editor.
	m_listToolBar = addToolBar(tr("Game ToolBar"));
	m_listToolBar->setMovable(false);

	// Create the utility menu.
	QMenu* utilityMenu = new QMenu(tr("Game Utility"), m_listToolBar);
	menuBar()->addMenu(utilityMenu);
	QAction* catAct = new QAction(tr("Categories"), m_listToolBar);
	catAct->setToolTip(tr("Open the categories editor."));
	connect(catAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::CATEGORIES);});
	utilityMenu->addAction(catAct);

	QAction* devAct = new QAction(tr("Developpers"), m_listToolBar);
	devAct->setToolTip(tr("Open the developpers editor."));
	connect(devAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::DEVELOPPERS);});
	utilityMenu->addAction(devAct);

	QAction* pbAct = new QAction(tr("Publishers"), m_listToolBar);
	pbAct->setToolTip(tr("Open the publishers editor."));
	connect(pbAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PUBLISHERS);});
	utilityMenu->addAction(pbAct);

	QAction* platAct = new QAction(tr("Platforms"), m_listToolBar);
	platAct->setToolTip(tr("Open the platform editor."));
	connect(platAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PLATFORM);});
	utilityMenu->addAction(platAct);

	QAction* servAct = new QAction(tr("Services"), m_listToolBar);
	servAct->setToolTip(tr("Open the services editor."));
	connect(servAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERVICES);});
	utilityMenu->addAction(servAct);

	// Creating the toolButton used to open the utilityMenu.
	QIcon utilityIcon(":/Images/Utility.svg");
	QToolButton* utilityToolButton = new QToolButton(m_listToolBar);
	utilityToolButton->setText(tr("Game Utility"));
	utilityToolButton->setIcon(utilityIcon);
	utilityToolButton->setMenu(utilityMenu);
	utilityToolButton->setToolTip(tr("Set the Categories, Developpers, Publishers, Platforms and Services of the game list."));
	utilityToolButton->setPopupMode(QToolButton::InstantPopup);
	m_listToolBar->addWidget(utilityToolButton);
}

void MainWindow::newListCreated(ListType type)
{
	// Recreating the menu when a new list is created.
	if (m_listToolBar)
	{
		delete m_listToolBar;
		m_listToolBar = nullptr;
	}

	if (type == ListType::GAMELIST)
		createGameToolBar();
}

void MainWindow::listFilePathChanged(const QString& filePath)
{
	m_listFilePath = filePath;
	m_listChanged = false;
	updateWindowTitle();
}

void MainWindow::listChanged(bool isChanged)
{
	m_listChanged = isChanged;
	updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
	// Update the window title using the program name,
	// the file path and an indicator if the list is modified.
	QString title("Game Sorting");

	if (!m_listFilePath.isEmpty())
	{
		title += QString(" : %2%1")
			.arg(m_listFilePath)
			.arg(m_listChanged ? "*" : "");
	}

	setWindowTitle(title);
}

void MainWindow::closeEvent(QCloseEvent* evt)
{
	// This member function is called when the user hit the X button
	// to close this window. It check if the list need to be saved and if
	// the want to leave.
	if (m_tabAndList->maybeSave())
	{
		writeSettings();
		evt->accept();
	}
	else
		evt->ignore();
}

void MainWindow::writeSettings()
{
	// Write the settings into Windows Registery or Linux Config File.
	// The settings are used to save the geometry of the windows, the last opened file, etc.
	QSettings settings("Erwan28250", "GameSorting_alpha");

	// Save the geometry of the window.
	settings.setValue("mainwindow/geometry", saveGeometry());

	// Save the opened file path.
	if (!m_tabAndList->filePath().isEmpty())
		settings.setValue("core/filepath", m_tabAndList->filePath());
	else
		settings.remove("core/filepath");

	// Save the current directory, this is the last directory opened with a file dialog.
	if (!m_tabAndList->currentDirectory().isEmpty())
		settings.setValue("core/currentdir", m_tabAndList->currentDirectory());
	else
		settings.remove("core/currentdir");
}

void MainWindow::readSettings()
{
	// Read the settings saved in the Windows Registery or in Linux Config File.
	QSettings settings("Erwan28250", "GameSorting_alpha");

	// Read the geometry of the window.
	QVariant vGeometry = settings.value("mainwindow/geometry");
	if (vGeometry.isValid() && !m_isResetSettings)
		restoreGeometry(vGeometry.toByteArray());
	else
	{
		// If there is no available geometry settings,
		// put the window in the center of the screen.
		QSize wSize(
			screen()->size().width() / 2,
			screen()->size().height() / 1.5);
		QPoint wPos(
			(screen()->size().width() - wSize.width()) / 2,
			(screen()->size().height() - wSize.height()) / 2);
		setGeometry(QRect(wPos, wSize));
	}
	
	// Read the saved file path.
	QVariant vFilePath = settings.value("core/filepath");
	if (vFilePath.isValid() && !m_isResetSettings)
		m_tabAndList->open(vFilePath.toString());
	
	// Read the current directory.
	QVariant vCurrentDir = settings.value("core/currentdir");
	if (vCurrentDir.isValid() && !m_isResetSettings)
		m_tabAndList->setCurrentDit(vCurrentDir.toString());
}