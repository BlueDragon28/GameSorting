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

#include "MainWindow.h"
#include "TabAndList.h"
#include "LicenceDialog.h"
#include "AboutDialog.h"
#include "Common.h"

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
#include <QToolButton>
#include <QFileInfo>

MainWindow::MainWindow(const QString& filePath, bool resetSettings, bool doNotSaveSettings, QWidget* parent) :
	QMainWindow(parent),
	m_db(QSqlDatabase::addDatabase("QSQLITE")),
	m_tabAndList(nullptr),
	m_listToolBar(nullptr),
	m_listChanged(false),

	m_isResetSettings(resetSettings),
	m_doNotSaveSettings(doNotSaveSettings),

	m_fileMenu(nullptr),
	m_utilityMenu(nullptr),
	m_helpMenu(nullptr),
	m_recentFileMenu(nullptr),
	m_licenceDialog(nullptr),
	m_aboutDialog(nullptr)
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
		openRecentFile(filePath);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenu()
{
	// Creation of all the menus and the toolbar.

	// Create the menu file.
	m_fileMenu = menuBar()->addMenu(tr("&File"));
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
	// Movies list.
	QAction* newMovieListAct = new QAction(tr("Movie list"), this);
	newMovieListAct->setToolTip(tr("Creating a movie list."));
	newListMenu->addAction(newMovieListAct);
	connect(newMovieListAct, &QAction::triggered, m_tabAndList, &TabAndList::newMoviesList);
	// Books list.
	QAction* newBooksListAct = new QAction(tr("Books List"), this);
	newBooksListAct->setToolTip(tr("Creating a books list."));
	newListMenu->addAction(newBooksListAct);
	connect(newBooksListAct, &QAction::triggered, m_tabAndList, &TabAndList::newBooksList);
	// Series list.
	QAction* newSeriesListAct = new QAction(tr("Series List"), this);
	newSeriesListAct->setToolTip(tr("Creating a series list."));
	newListMenu->addAction(newSeriesListAct);
	connect(newSeriesListAct, &QAction::triggered, m_tabAndList, &TabAndList::newSeriesList);
	// Common list.
	QAction* newCommonListAct = new QAction(tr("Common list"), this);
	newCommonListAct->setToolTip(tr("Creating a common list."));
	newListMenu->addAction(newCommonListAct);
	connect(newCommonListAct, &QAction::triggered, m_tabAndList, &TabAndList::newCommonList);
	m_fileMenu->addMenu(newListMenu);

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
	m_fileMenu->addAction(openListAct);
	connect(openListAct, &QAction::triggered, [this](){this->openRecentFile(QString());});
	fileToolBar->addAction(openListAct);

	// Save a list into a file.
	QIcon saveListIcon = QIcon(":/Images/Save.svg");
	QAction* saveListAct = new QAction(saveListIcon, tr("Save"), this);
	saveListAct->setShortcut(QKeySequence::Save);
	saveListAct->setToolTip(tr("Save a list into a file"));
	m_fileMenu->addAction(saveListAct);
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
	m_fileMenu->addAction(saveAsListAct);

	// Open recent file.
	m_recentFileMenu = new QMenu(tr("Recent file"), this);
	m_fileMenu->addMenu(m_recentFileMenu);

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
	m_fileMenu->addAction(quitAct);

	// Add help menu
	m_helpMenu = menuBar()->addMenu(tr("Help"));
	QAction* licenceAct = new QAction(tr("Licence"), this);
	connect(licenceAct, &QAction::triggered, this, &MainWindow::showLicence);
	m_helpMenu->addAction(licenceAct);

	QAction* aboutAct = new QAction(tr("About"), this);
	connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
	m_helpMenu->addAction(aboutAct);

	QAction* aboutQt = new QAction(tr("About Qt"), this);
	connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
	m_helpMenu->addAction(aboutQt);
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
	m_utilityMenu = new QMenu(tr("Game Utility"), m_listToolBar);
	connect(m_utilityMenu, &QMenu::destroyed, [this](){this->m_utilityMenu = nullptr;});
	reinsertMenu();
	QAction* serAct = new QAction(tr("Series"), m_listToolBar);
	serAct->setToolTip(tr("Open the series editor."));
	connect(serAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERIES);});
	m_utilityMenu->addAction(serAct);

	QAction* catAct = new QAction(tr("Categories"), m_listToolBar);
	catAct->setToolTip(tr("Open the categories editor."));
	connect(catAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::CATEGORIES);});
	m_utilityMenu->addAction(catAct);

	QAction* devAct = new QAction(tr("Developpers"), m_listToolBar);
	devAct->setToolTip(tr("Open the developpers editor."));
	connect(devAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::DEVELOPPERS);});
	m_utilityMenu->addAction(devAct);

	QAction* pbAct = new QAction(tr("Publishers"), m_listToolBar);
	pbAct->setToolTip(tr("Open the publishers editor."));
	connect(pbAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PUBLISHERS);});
	m_utilityMenu->addAction(pbAct);

	QAction* platAct = new QAction(tr("Platforms"), m_listToolBar);
	platAct->setToolTip(tr("Open the platform editor."));
	connect(platAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PLATFORM);});
	m_utilityMenu->addAction(platAct);

	QAction* servAct = new QAction(tr("Services"), m_listToolBar);
	servAct->setToolTip(tr("Open the services editor."));
	connect(servAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERVICES);});
	m_utilityMenu->addAction(servAct);

	// Creating the toolButton used to open the utilityMenu.
	QIcon utilityIcon(":/Images/Utility.svg");
	QToolButton* utilityToolButton = new QToolButton(m_listToolBar);
	utilityToolButton->setText(tr("Game Utility"));
	utilityToolButton->setIcon(utilityIcon);
	utilityToolButton->setMenu(m_utilityMenu);
	utilityToolButton->setToolTip(tr("Set the Categories, Developpers, Publishers, Platforms and Services of the game list."));
	utilityToolButton->setPopupMode(QToolButton::InstantPopup);
	m_listToolBar->addWidget(utilityToolButton);
}

void MainWindow::createMovieToolBar()
{
	// Creating the toolbar next to the file toolbar.
	// This toolbar is used to open the utility editor.
	m_listToolBar = addToolBar(tr("Movie ToolBar"));
	m_listToolBar->setMovable(false);

	// Create the utility menu.
	m_utilityMenu = new QMenu(tr("Movie Utility"), m_listToolBar);
	connect(m_utilityMenu, &QMenu::destroyed, [this](){this->m_utilityMenu = nullptr;});
	reinsertMenu();

	QAction* serAct= new QAction(tr("Series"), m_listToolBar);
	serAct->setToolTip(tr("Open the series editor."));
	connect(serAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERIES);});
	m_utilityMenu->addAction(serAct);

	QAction* catAct = new QAction(tr("Categories"), m_listToolBar);
	catAct->setToolTip(tr("Open the categories editor."));
	connect(catAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::CATEGORIES);});
	m_utilityMenu->addAction(catAct);

	QAction* directorsAct = new QAction(tr("Directors"), m_listToolBar);
	directorsAct->setToolTip(tr("Open the directors editor."));
	connect(directorsAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::DIRECTOR);});
	m_utilityMenu->addAction(directorsAct);

	QAction* actorsAct = new QAction(tr("Actors"), m_listToolBar);
	actorsAct->setToolTip(tr("Open the actors editor."));
	connect(actorsAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::ACTORS);});
	m_utilityMenu->addAction(actorsAct);

	QAction* producAct = new QAction(tr("Productions"), m_listToolBar);
	producAct->setToolTip(tr("Open the productions editor."));
	connect(producAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PRODUCTION);});
	m_utilityMenu->addAction(producAct);

	QAction* musicAct = new QAction(tr("Music"), m_listToolBar);
	musicAct->setToolTip(tr("Open the music editor."));
	connect(musicAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::MUSIC);});
	m_utilityMenu->addAction(musicAct);

	QAction* servAct = new QAction(tr("Services"), m_listToolBar);
	servAct->setToolTip(tr("Open the services editor."));
	connect(servAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERVICES);});
	m_utilityMenu->addAction(servAct);

	// Creating the toolButton  used to open the utilityMenu.
	QIcon utilityIcon(":/Images/Utility.svg");
	QToolButton* utilityToolButton = new QToolButton(m_listToolBar);
	utilityToolButton->setText(tr("Movie Utility"));
	utilityToolButton->setIcon(utilityIcon);
	utilityToolButton->setMenu(m_utilityMenu);
	utilityToolButton->setToolTip(tr("Set the Categories, Directors, Actors, Productions, Music and Services of the movies list."));
	utilityToolButton->setPopupMode(QToolButton::InstantPopup);
	m_listToolBar->addWidget(utilityToolButton);
}

void MainWindow::createCommonToolBar()
{
	m_listToolBar = addToolBar(tr("Common Toolbar"));
	m_listToolBar->setMouseTracking(false);

	// Create the utility menu.
	m_utilityMenu = new QMenu(tr("Movie Utility"), m_listToolBar);
	connect(m_utilityMenu, &QMenu::destroyed, [this](){this->m_utilityMenu = nullptr;});
	reinsertMenu();

	QAction* serAct= new QAction(tr("Series"), m_listToolBar);
	serAct->setToolTip(tr("Open the series editor."));
	connect(serAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERIES);});
	m_utilityMenu->addAction(serAct);

	QAction* catAct = new QAction(tr("Categories"), m_listToolBar);
	catAct->setToolTip(tr("Open the categories editor."));
	connect(catAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::CATEGORIES);});
	m_utilityMenu->addAction(catAct);

	QAction* autAct = new QAction(tr("Authors"), m_listToolBar);
	autAct->setToolTip(tr("Open the authors editor."));
	connect(autAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::AUTHORS);});
	m_utilityMenu->addAction(autAct);

	// Creating the toolButton used to open the utilityMenu.
	QIcon utilityIcon(":/Images/Utility.svg");
	QToolButton* utilityToolButton = new QToolButton(m_listToolBar);
	utilityToolButton->setText(tr("Common Utility"));
	utilityToolButton->setIcon(utilityIcon);
	utilityToolButton->setMenu(m_utilityMenu);
	utilityToolButton->setToolTip(tr("Set the Categories and Authors of the common list."));
	utilityToolButton->setPopupMode(QToolButton::InstantPopup);
	m_listToolBar->addWidget(utilityToolButton);
}

void MainWindow::createBooksToolbar()
{
	// Creating the toolbar next to the file toolbar.
	// This toolbar is used to open the utility editor.
	m_listToolBar = addToolBar(tr("Books Toolbar"));
	m_listToolBar->setMovable(false);

	// Creathe the utility menu.
	m_utilityMenu = new QMenu(tr("Books Utility"), m_listToolBar);
	connect(m_utilityMenu, &QMenu::destroyed, [this](){this->m_utilityMenu = nullptr;});
	reinsertMenu();

	QAction* seriesAct = new QAction(tr("Series"), m_listToolBar);
	seriesAct->setToolTip(tr("Open the series editor"));
	connect(seriesAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERIES);});
	m_utilityMenu->addAction(seriesAct);

	QAction* catAct = new QAction(tr("Categories"), m_listToolBar);
	catAct->setToolTip(tr("Open the categories editor."));
	connect(catAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::CATEGORIES);});
	m_utilityMenu->addAction(catAct);

	QAction* autAct = new QAction(tr("Authors"), m_listToolBar);
	autAct->setToolTip(tr("Open the authors editor."));
	connect(autAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::AUTHORS);});
	m_utilityMenu->addAction(autAct);

	QAction* pubAct = new QAction(tr("Publishers"), m_listToolBar);
	autAct->setToolTip(tr("Open the publishers editor."));
	connect(pubAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PUBLISHERS);});
	m_utilityMenu->addAction(pubAct);

	QAction* serAct = new QAction(tr("Services"), m_listToolBar);
	serAct->setToolTip(tr("Open the services editor."));
	connect(serAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERVICES);});
	m_utilityMenu->addAction(serAct);

	// Creating the toolButton used to open the utilityMenu.
	QIcon utilityIcon(":/Images/Utility.svg");
	QToolButton* utilityToolButton = new QToolButton(m_listToolBar);
	utilityToolButton->setText(tr("Books Utility"));
	utilityToolButton->setIcon(utilityIcon);
	utilityToolButton->setMenu(m_utilityMenu);
	utilityToolButton->setToolTip(tr("Set the Categories, Authors, Publishers and Services of the books list."));
	utilityToolButton->setPopupMode(QToolButton::InstantPopup);
	m_listToolBar->addWidget(utilityToolButton);
}

void MainWindow::createSeriesToolbar()
{
	// Creating the toolbar next to the file toolbar.
	// This toolbar is used to open the utility editor.
	m_listToolBar = addToolBar(tr("Series ToolBar"));
	m_listToolBar->setMovable(false);

	// Create the utility menu.
	m_utilityMenu = new QMenu(tr("Series Utility"), m_listToolBar);
	connect(m_utilityMenu, &QMenu::destroyed, [this](){this->m_utilityMenu = nullptr;});
	reinsertMenu();

	QAction* catAct = new QAction(tr("Categories"), m_listToolBar);
	catAct->setToolTip(tr("Open the categories editor."));
	connect(catAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::CATEGORIES);});
	m_utilityMenu->addAction(catAct);

	QAction* dirAct = new QAction(tr("Directors"), m_listToolBar);
	dirAct->setToolTip(tr("Open the directors editor."));
	connect(dirAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::DIRECTOR);});
	m_utilityMenu->addAction(dirAct);

	QAction* actAct = new QAction(tr("Actors"), m_listToolBar);
	actAct->setToolTip(tr("Open the actors editor."));
	connect(actAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::ACTORS);});
	m_utilityMenu->addAction(actAct);

	QAction* prodAct = new QAction(tr("Productions"), m_listToolBar);
	prodAct->setToolTip(tr("Open the productions editor."));
	connect(prodAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::PRODUCTION);});
	m_utilityMenu->addAction(prodAct);

	QAction* musicAct = new QAction(tr("Musics"), m_listToolBar);
	musicAct->setToolTip(tr("Open the musics editor."));
	connect(musicAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::MUSIC);});
	m_utilityMenu->addAction(musicAct);

	QAction* serAct = new QAction(tr("Services"), m_listToolBar);
	serAct->setToolTip(tr("Open the services editor."));
	connect(serAct, &QAction::triggered, [this](){this->m_tabAndList->openUtility(UtilityTableName::SERVICES);});
	m_utilityMenu->addAction(serAct);

	// Creating the toolButton used to open the utilityMenu.
	QIcon utilityIcon(":/Images/Utility.svg");
	QToolButton* utilityToolButton = new QToolButton(m_listToolBar);
	utilityToolButton->setText(tr("Series Utility"));
	utilityToolButton->setIcon(utilityIcon);
	utilityToolButton->setMenu(m_utilityMenu);
	utilityToolButton->setToolTip(tr("Set the Categories, Directors, Actors, Productions, Musics and Services."));
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
	if (m_utilityMenu)
	{
		m_utilityMenu->deleteLater();
		m_utilityMenu = nullptr;
	}

	if (type == ListType::GAMELIST)
		createGameToolBar();
	else if (type == ListType::MOVIESLIST)
		createMovieToolBar();
	else if (type == ListType::COMMONLIST)
		createCommonToolBar();
	else if (type == ListType::BOOKSLIST)
		createBooksToolbar();
	else if (type == ListType::SERIESLIST)
		createSeriesToolbar();
}

void MainWindow::listFilePathChanged(const QString& filePath)
{
	m_listFilePath = filePath;
	m_listChanged = false;
	updateWindowTitle();
	updateRecentFileMenu();
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

	// If the member variable m_doNotSaveSettings is true,
	// we do not save the settings.
	if (m_doNotSaveSettings)
		return;

#ifdef NDEBUG
	QSettings settings("BlueDragon28", "gamesorting");
#else
	QSettings settings("BlueDragon28", "gamesorting_debug");
#endif

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

	// Save the recent file list.
	if (!m_recentFileData.isEmpty())
	{
		QList<QString> recentFileList(m_recentFileData.size());
		for (int i = 0; i < m_recentFileData.size(); i++)
			recentFileList[i] = m_recentFileData.at(i).filePath;
		settings.setValue("core/recentfiles", recentFileList);
	}
	else
		settings.remove("core/recentfiles");
}

void MainWindow::readSettings()
{
	// Read the settings saved in the Windows Registery or in Linux Config File.
#ifdef NDEBUG
	QSettings settings("BlueDragon28", "gamesorting");
#else
	QSettings settings("BlueDragon28", "gamesorting_debug");
#endif

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

	// Read the recent file list.
	QVariant vRecentFile = settings.value("core/recentfiles");
	if (vRecentFile.isValid() && !m_isResetSettings)
	{
		QList<QString> recentFileList = vRecentFile.toStringList();
		m_recentFileData.resize(recentFileList.size());
		for (int i = 0; i < recentFileList.size(); i++)
			m_recentFileData[i] = getRecentFileData(recentFileList.at(i));
		updateRecentFileMenu();
	}
}

void MainWindow::showLicence()
{
	// Show the licence of the program.
	if (!m_licenceDialog)
	{
		m_licenceDialog = new LicenceDialog(this);
		m_licenceDialog->raise();
		m_licenceDialog->activateWindow();
	}
	
	m_licenceDialog->show();
}

void MainWindow::about()
{
	// Show the about menu.
	if (!m_aboutDialog)
	{
		m_aboutDialog = new AboutDialog(this);
		m_aboutDialog->raise();
		m_aboutDialog->activateWindow();
	}

	m_aboutDialog->show();
}

void MainWindow::reinsertMenu()
{
	// Reinsert the menu into the menuBar.
	menuBar()->clear();
	menuBar()->addMenu(m_fileMenu);
	if (m_utilityMenu)
		menuBar()->addMenu(m_utilityMenu);
	menuBar()->addMenu(m_helpMenu);
}

void MainWindow::updateRecentFileMenu()
{
	// Update the recent file menu.
	if (!m_listFilePath.isEmpty())
	{
		bool found = false;
		for (int i = 0; i < m_recentFileData.size(); i++)
		{
			if (m_recentFileData.at(i).filePath.compare(m_listFilePath) == 0)
			{
				RecentFileData recentFile = m_recentFileData.at(i);
				m_recentFileData.remove(i);
				m_recentFileData.prepend(recentFile);
				found = true;
				break;
			}
		}

		if (!found)
		{
			m_recentFileData.prepend(getRecentFileData(m_listFilePath));
			removeOldRecentFile();
		}
	}

	m_recentFileMenu->clear();

	foreach (const RecentFileData& file, m_recentFileData)
	{
		QAction* recentFileAct = new QAction(file.fileName, m_recentFileMenu);
		connect(recentFileAct, &QAction::triggered, [this, file](){this->openRecentFile(file.filePath);});
		m_recentFileMenu->addAction(recentFileAct);
	}
}

void MainWindow::removeOldRecentFile()
{
	if (m_recentFileData.size() > 10)
	{
		int number = m_recentFileData.size() - 10;
		if (number > 0)
			m_recentFileData.remove(10, number);
	}
}

void MainWindow::removeInvalidRecentFile(const QString& filePath)
{
	// Removing file that do not exist.
	for (int i = 0; i < m_recentFileData.size(); i++)
	{
		if (m_recentFileData.at(i).filePath.compare(filePath) == 0)
		{
			m_recentFileData.remove(i);
			updateRecentFileMenu();
			break;
		}
	}
}

void MainWindow::openRecentFile(const QString& filePath)
{
	// Check if the file exist and opening it, otherwise, removing the file from the recent file list.
	if (filePath.isEmpty())
	{
		m_tabAndList->open();
		return;
	}

	if (QFileInfo::exists(filePath))
		m_tabAndList->open(filePath);
	else if (filePath.isEmpty())
		return;
	else
	{
		QMessageBox::warning(
			this,
			tr("Invalid file"),
			tr("This recent file does not exist anymore and will be remove from the recent file list."),
			QMessageBox::Ok,
			QMessageBox::Ok);
		removeInvalidRecentFile(filePath);
	}
}