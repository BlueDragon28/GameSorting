#include "MainWindow.h"
#include "GameListModel.h"
#include "GameStarDelegate.h"
#include "GameTabAndList.h"
#include "AboutDialog.h"

#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTableView>
#include <QToolBar>
#include <QIcon>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QDataStream>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QCloseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <algorithm>
#include <functional>

#define DEFINED_GLD_IDENTIFIER "GLD"
#define DEFINED_GLD_VERSION_2 2
#define DEFINED_GLD_VERSION_3 3
#define DEFINED_GLD_VERSION DEFINED_GLD_VERSION_3

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent),
	//m_gameListView(new QTableView(this)),
	//m_gModel(new GameListModel(this)),
	m_gTabAndList(new GameTabAndList(this)),
	m_aboutDialog(nullptr)
{
	// Calling the methods for the creation of the menu and toolbar and also
	// set the QTableView as the central object of the MainWindow.
	createMenu();
	createCentralWidget();
	readSettings();
	setFileNameIntoWindowTitle();
	connect(m_gTabAndList, &GameTabAndList::listEdited, this, &MainWindow::setFileNameIntoWindowTitle);
}

MainWindow::MainWindow(const QString& filePath, bool noSettings, QWidget* parent) :
	QMainWindow(parent),
	//m_gameListView(new QTableView(this)),
	//m_gModel(new GameListModel(this)),
	m_gTabAndList(new GameTabAndList(this)),
	m_aboutDialog(nullptr)
{
	// Calling the methods for the creation of the menu and toolbar and also
	// set the QTableView as the central object of the MainWindow.
	createMenu();
	createCentralWidget();
	readSettings(!noSettings, filePath.isEmpty() ? true : false);
	if (!filePath.isEmpty() && QFile::exists(filePath))
	{
		bool result = false;
		TabAndListData gamesList = openFile(filePath, result);
		if (result)
		{
			m_gTabAndList->clearTabs();
			m_gTabAndList->setGamesAndTabData(gamesList);
		}
		else
			QMessageBox::warning(this, tr("Opening file"), tr("Failed to open the file %1.").arg(filePath), QMessageBox::Ok);
	}
	setFileNameIntoWindowTitle();
	connect(m_gTabAndList, &GameTabAndList::listEdited, this, &MainWindow::setFileNameIntoWindowTitle);
}

MainWindow::~MainWindow()
{}

void MainWindow::createMenu()
{
	/*
	*	Creation of the menu "File" and adding the button Quit
	*	for quitting the application.
	*	The first call of menuBar()->addMenu() create the menu bar
	*	in the MainWindow.
	*/
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QToolBar* fileBar = addToolBar(tr("File"));

	QIcon newGameListIcon(":/Images/New.svg");
	QAction* newGameListAct = new QAction(newGameListIcon, tr("&New"), this);
	newGameListAct->setShortcut(QKeySequence::New);
	newGameListAct->setToolTip(tr("Making a new game list"));
	connect(newGameListAct, &QAction::triggered, this, &MainWindow::newGameList);
	fileMenu->addAction(newGameListAct);
	fileBar->addAction(newGameListAct);

	QIcon openGameListIcon(":/Images/Open.svg");
	QAction* openGameListAct = new QAction(openGameListIcon, tr("&Open"), this);
	openGameListAct->setShortcut(QKeySequence::Open);
	openGameListAct->setToolTip(tr("Open a game list file"));
	connect(openGameListAct, &QAction::triggered, this, &MainWindow::openGameList);
	fileMenu->addAction(openGameListAct);
	fileBar->addAction(openGameListAct);

	QIcon saveGameListIcon(":/Images/Save.svg");
	QAction* saveGameListAct = new QAction(saveGameListIcon, tr("&Save"), this);
	saveGameListAct->setShortcut(QKeySequence::Save);
	saveGameListAct->setToolTip(tr("Save a game list into a file"));
	connect(saveGameListAct, &QAction::triggered, this, &MainWindow::saveGameList);
	fileMenu->addAction(saveGameListAct);
	fileBar->addAction(saveGameListAct);

	QAction* saveAsGameListAct = new QAction(tr("Save &as"), this);
#ifdef WIN32
	saveAsGameListAct->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_S);
#else
	saveAsGameListAct->setShortcut(QKeySequence::SaveAs);
#endif
	saveAsGameListAct->setToolTip(tr("Save as the game list into a file"));
	connect(saveAsGameListAct, &QAction::triggered, this, &MainWindow::saveAsGameList);
	fileMenu->addAction(saveAsGameListAct);

	fileMenu->addSeparator();

	QIcon quitIcon(":/Images/Exit.svg");
	QAction* quitAct = new QAction(quitIcon, tr("&Quit"), this);
#ifdef WIN32
	quitAct->setShortcut(Qt::CTRL | Qt::Key_Q);
#else
	quitAct->setShortcut(QKeySequence::Quit);
#endif
	quitAct->setToolTip(tr("Quit the application."));
	connect(quitAct, &QAction::triggered, this, &MainWindow::close);
	fileMenu->addAction(quitAct);

	// creation of the edit menu
	QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
	QToolBar* editBar = addToolBar(tr("Edit"));

	const QIcon addIcon(":/Images/Add.svg");
	QAction* addAct = new QAction(addIcon, tr("&Add game"), this);
	addAct->setShortcut(Qt::CTRL | Qt::Key_A);
	addAct->setToolTip(tr("Adding a new game to the games list"));
	//connect(addAct, &QAction::triggered, m_gModel, qOverload<>(&GameListModel::appendGame));
	connect(addAct, &QAction::triggered, this, &MainWindow::appendGame);
	connect(m_gTabAndList, &GameTabAndList::gameListFiltered, [addAct](bool value) { addAct->setEnabled(!value); });
	editMenu->addAction(addAct);
	editBar->addAction(addAct);

	const QIcon delIcon(":/Images/Del.svg");
	QAction* delAct = new QAction(delIcon, tr("&Delete games"), this);
	delAct->setShortcut(Qt::CTRL | Qt::Key_D);
	delAct->setToolTip(tr("Removing all selected games"));
	connect(delAct, &QAction::triggered, this, &MainWindow::removeGame);
	connect(m_gTabAndList, &GameTabAndList::gameListFiltered, [delAct](bool value) { delAct->setEnabled(!value); });
	editMenu->addAction(delAct);
	editBar->addAction(delAct);

	editMenu->addSeparator();
	editBar->addSeparator();

	QIcon copyIcon(":/Images/Copy.svg");
	QAction* copyAct = new QAction(copyIcon, tr("Copy"), this);
	copyAct->setShortcut(QKeySequence::Copy);
	copyAct->setToolTip(tr("Copy the selected games names into the clipboard"));
	connect(copyAct, &QAction::triggered, m_gTabAndList, &GameTabAndList::copyGameNameToClipboard);
	editMenu->addAction(copyAct);
	editBar->addAction(copyAct);

	QAction* copyAllColumnsAct = new QAction(tr("Copy all columns"), this);
	copyAllColumnsAct->setShortcut(Qt::SHIFT | Qt::Key_C);
	copyAllColumnsAct->setToolTip(tr("Copy the selected games names, types and rates into the clipboard in this way : name;type;rate."));
	connect(copyAllColumnsAct, &QAction::triggered, m_gTabAndList, &GameTabAndList::copyGameNameTypeAndRateToClipboard);
	editMenu->addAction(copyAllColumnsAct);

	QIcon pasteIcon(":/Images/Paste.svg");
	QAction* pasteAct = new QAction(pasteIcon, tr("Paste"), this);
	pasteAct->setShortcut(QKeySequence::Paste);
	pasteAct->setToolTip(tr("Paste the games names (and if available, types and rates) into the game list."));
	connect(pasteAct, &QAction::triggered, m_gTabAndList, &GameTabAndList::pasteGameNameFromCliptboard);
	connect(m_gTabAndList, &GameTabAndList::gameListFiltered, [pasteAct](bool value) { pasteAct->setEnabled(!value); });
	editMenu->addAction(pasteAct);
	editBar->addAction(pasteAct);

	editMenu->addSeparator();
	editBar->addSeparator();

	QIcon sortingEnabledMenuIcon = QIcon(":/Images/Sorting_lowResolution.svg");
	QAction* sortingEnabledMenuAct = new QAction(sortingEnabledMenuIcon, tr("Enable sorting"), this);
	sortingEnabledMenuAct->setCheckable(true);
	connect(sortingEnabledMenuAct, &QAction::toggled, m_gTabAndList, &GameTabAndList::setSortingEnabled);
	connect(m_gTabAndList, &GameTabAndList::gameListSorted, sortingEnabledMenuAct, &QAction::setChecked);
	connect(m_gTabAndList, &GameTabAndList::gameListFiltered, [sortingEnabledMenuAct](bool value) { if (value) sortingEnabledMenuAct->setChecked(false); sortingEnabledMenuAct->setEnabled(!value); });
	editMenu->addAction(sortingEnabledMenuAct);

	QIcon sortingEnabledBarIcon = QIcon(":/Images/Sorting.svg");
	QAction* sortingEnabledBarAct = new QAction(sortingEnabledBarIcon, tr("Enable sorting"), this);
	sortingEnabledBarAct->setCheckable(true);
	connect(sortingEnabledBarAct, &QAction::toggled, m_gTabAndList, &GameTabAndList::setSortingEnabled);
	connect(m_gTabAndList, &GameTabAndList::gameListSorted, sortingEnabledBarAct, &QAction::setChecked);
	connect(m_gTabAndList, &GameTabAndList::gameListFiltered, [sortingEnabledBarAct](bool value) { if (value) sortingEnabledBarAct->setChecked(false); sortingEnabledBarAct->setEnabled(!value); });
	connect(sortingEnabledMenuAct, &QAction::triggered, sortingEnabledBarAct, &QAction::setChecked);
	connect(sortingEnabledBarAct, &QAction::triggered, sortingEnabledMenuAct, &QAction::setChecked);
	editBar->addAction(sortingEnabledBarAct);

	QIcon filterMenuIcon(":/Images/Filter_lowResolution.svg");
	QAction* filterMenuAct = new QAction(filterMenuIcon, tr("Apply filter"), this);
	filterMenuAct->setToolTip(tr("Apply a filter to the current focussed game list using a dialog."));
	connect(filterMenuAct, &QAction::triggered, this, &MainWindow::filteringGameList);
	editMenu->addAction(filterMenuAct);

	QIcon filterBarIcon(":/Images/Filter.svg");
	QAction* filterBarAct = new QAction(filterBarIcon, tr("Apply filter"), this);
	filterMenuAct->setToolTip(tr("Apply a filter to the current focussed game list using a dialog"));
	connect(filterBarAct, &QAction::triggered, this, &MainWindow::filteringGameList);
	editBar->addAction(filterBarAct);

	// add predefined game only for debuging
#ifndef NDEBUG
	QMenu *debugMenu = menuBar()->addMenu(tr("&Debug"));

	QAction* addGameDebugAct = new QAction(tr("Add games"), this);
	addGameDebugAct->setToolTip(tr("Add predefined game only for debugging"));
	connect(addGameDebugAct, &QAction::triggered, this, &MainWindow::addGame);
	connect(m_gTabAndList, &GameTabAndList::gameListFiltered, [addGameDebugAct](bool value) { addGameDebugAct->setEnabled(!value); });
	debugMenu->addAction(addGameDebugAct);

	debugMenu->addSeparator();

	QAction* debugFilterAct = new QAction(tr("Debug filter"), this);
	debugFilterAct->setToolTip(tr("Debug games with the type \"Type 3\""));
	connect(debugFilterAct, &QAction::triggered, [this]() { m_gTabAndList->filterGamesFromType("Type 3"); });
	debugMenu->addAction(debugFilterAct);

	QAction* disableFilterAct = new QAction(tr("Disable filter"), this);
	disableFilterAct->setToolTip(tr("Disable filter."));
	connect(disableFilterAct, &QAction::triggered, [this]() { m_gTabAndList->filterGamesFromType(QString()); });
	debugMenu->addAction(disableFilterAct);

#endif // NDEBUG

	QMenu* helpMenu = menuBar()->addMenu(tr("Help"));

	QAction* aboutAct = new QAction(tr("About"), this);
	aboutAct->setToolTip(tr("Information about this application"));
	connect(aboutAct, &QAction::triggered, this, &MainWindow::aboutApp);
	helpMenu->addAction(aboutAct);
}

void MainWindow::createCentralWidget()
{
	// Setting the GameTabAndList widget on center of MainWindow
	m_gTabAndList->restoreToDefault();
	setCentralWidget(m_gTabAndList);
}

#ifndef NDEBUG
void MainWindow::addGame()
{
	m_gTabAndList->appendGames(
		{
			{ "Game 1", "Type 1", 1 },
			{ "Game 2", "Type 2", 2 },
			{ "Game 3", "Type 3", 3 },
			{ "Game 4", "Type 4", 4 },
			{ "Game 5", "Type 5", 5 }
		});
}
#endif // NDEBUG

void MainWindow::removeGame()
{
	// Remove games that the user has selected.
	m_gTabAndList->removeGames();
}

void MainWindow::appendGame()
{
	// Add a new empty game in the game list.
	m_gTabAndList->appendGame();
}

void MainWindow::newGameList()
{
	// Creating a new game list, but thirst, 
	// let check if it's needed to save the current list.
	if (maybeSave())
	{
		m_gTabAndList->restoreToDefault();
		m_gFilePath.clear();
		setFileNameIntoWindowTitle();
	}
}

void MainWindow::openGameList()
{
	// Opening a game list from a file.

	// Thirst, check if it's needed to save the current list.
	if (!maybeSave()) return;

	// Opening a dialog to tell the user wich file he want to open.
	QFileDialog dialog(this, tr("Open a game list file"));
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setNameFilters(
		{
			"Game List Data (*.gld)",
			"Any files (*)"
		});
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setDirectory(m_lastOpenedDir);
	if (dialog.exec() != QFileDialog::Accepted)
		return;
	m_lastOpenedDir = dialog.directory().absolutePath();

	// If the user has selected a file, trying to opening it.
	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	bool result = false;
	const TabAndListData gamesData = openFile(dialog.selectedFiles().first(), result);

	if (result)
	{
		m_gFilePath = dialog.selectedFiles().first();
		setFileNameIntoWindowTitle();

		m_gTabAndList->clearTabs();
		m_gTabAndList->setGamesAndTabData(gamesData);
	}

	QGuiApplication::restoreOverrideCursor();
}

bool MainWindow::saveGameList()
{
	/*
	* Save the current gameList.
	* If the game list is already saved on the file,
	* save the game list on the save file stored at
	* m_gFilePath. Otherwise, save it has a new file.
	* */
	if (m_gFilePath.isEmpty())
		return saveAsGameList();
	else
	{
		QGuiApplication::setOverrideCursor(Qt::WaitCursor);
		bool result = saveFile(m_gFilePath);
		if (result)
		{
			m_gTabAndList->pendingChangeSaved();
			setFileNameIntoWindowTitle();
		}
		QGuiApplication::restoreOverrideCursor();
		return result;
	}
}

bool MainWindow::saveAsGameList()
{
	/*
	* Saving the game list has a new file.
	*/
	QFileDialog dialog(this, tr("Save game to a file"));
	dialog.setWindowModality(Qt::WindowModal);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setNameFilters(
		{
			"Game List Data (*.gld)",
			"Any files (*)"
		});
	dialog.setDirectory(m_lastOpenedDir);

	if (dialog.exec() != QFileDialog::Accepted)
		return false;

	m_lastOpenedDir = dialog.directory().absolutePath();

	QGuiApplication::setOverrideCursor(Qt::WaitCursor);
	bool result = saveFile(dialog.selectedFiles().first());
	QGuiApplication::restoreOverrideCursor();
	if (result)
	{
		m_gFilePath = dialog.selectedFiles().first();
		m_gTabAndList->pendingChangeSaved();
		setFileNameIntoWindowTitle();
	}
	return result;
}

void MainWindow::filteringGameList()
{
	// Apply a filter set by the user to the current focussed games list.

	//QDialog dialog(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	QDialog dialog(this);
	Qt::WindowFlags wFlags = dialog.windowFlags();
	wFlags &= ~Qt::WindowContextHelpButtonHint;
	dialog.setWindowFlags(wFlags);
	dialog.setModal(true);
	
	QLabel* filterLabel = new QLabel(tr("Type filter :"));
	QLineEdit* filterText = new QLineEdit();
	QPushButton* acceptButton = new QPushButton(tr("Filter"));
	QPushButton* cancelButton = new QPushButton(tr("Cancel"));

	QVBoxLayout* vLayout = new QVBoxLayout();
	QHBoxLayout* hLayout = new QHBoxLayout();

	vLayout->addWidget(filterLabel);
	vLayout->addWidget(filterText);
	hLayout->addWidget(acceptButton, 1, Qt::AlignHCenter | Qt::AlignVCenter);
	hLayout->addWidget(cancelButton, 1, Qt::AlignHCenter | Qt::AlignVCenter);
	vLayout->addLayout(hLayout);

	dialog.setLayout(vLayout);

	acceptButton->setDefault(true);

	connect(acceptButton, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

	filterText->setText(m_gTabAndList->gameFilteringTypeText());

	if (dialog.exec() == QDialog::Accepted)
		m_gTabAndList->filterGamesFromType(filterText->text());
}

void MainWindow::readSettings(bool readSettings, bool loadLastSavedFile)
{
	// reading the settings
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	// restore the geometry of the main window
	QVariant vGeometry = settings.value("mainwindow/geometry");
	if (vGeometry.isValid() && readSettings)
	{
		QByteArray bGeometry = vGeometry.toByteArray();
		restoreGeometry(bGeometry);
	}
	else
	{
		QSize screenSize = screen()->availableSize();

		QSize windowSize(screenSize.width() / 3, screenSize.height() / 2);
		QPoint windowPos(
			(screenSize.width() - windowSize.width()) / 2,
			(screenSize.height() - windowSize.height()) / 2);
		setGeometry(QRect(windowPos, windowSize));
	}

	// get the last opened directory
	QVariant vLastOpenedDirectory = settings.value("mainwindow/lastopeneddirectory");
	if (vLastOpenedDirectory.isValid() && readSettings)
		m_lastOpenedDir = vLastOpenedDirectory.toString();

	// open the last opened file.
	QVariant vLastFilePath = settings.value("gamelist/lastgamelist");
	if (vLastFilePath.isValid() && readSettings && loadLastSavedFile)
	{
		QString lastFilePath = vLastFilePath.toString();
		if (!lastFilePath.isEmpty() && QFile::exists(lastFilePath))
		{
			bool result = false;
			const TabAndListData gamesData = openFile(lastFilePath, result);
			if (result)
			{
				m_gTabAndList->clearTabs();
				m_gTabAndList->setGamesAndTabData(gamesData);
				m_gFilePath = lastFilePath;
			}
		}
	}

	// Read back if isSortingEnabled is set or not.
	/*QVariant vIsSortingEnabled = settings.value("gamelist/isSortingEnabled");
	if (vIsSortingEnabled.isValid() && readSettings)
	{
		bool bIsSortingEnabled = vIsSortingEnabled.toBool();
		m_isSortingEnabledAct->setChecked(bIsSortingEnabled);
	}*/
}

void MainWindow::writeSettings()
{
	// Write the settings
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	// Save the window geometry
	settings.setValue("mainwindow/geometry", QVariant::fromValue(saveGeometry()));

	// Save the opened file path
	if (m_gFilePath.isEmpty())
	{
		if (settings.contains("gamelist/lastgamelist"))
			settings.remove("gamelist/lastgamelist");
	}
	else
		settings.setValue("gamelist/lastgamelist", QVariant::fromValue(m_gFilePath));

	// Save the last opened directory in a file dialog.
	if (m_lastOpenedDir.isEmpty())
	{
		if (settings.contains("mainwindow/lastopeneddirectory"))
			settings.remove("mainwindow/lastopeneddirectory");
	}
	else
		settings.setValue("mainwindow/lastopeneddirectory", QVariant::fromValue(m_lastOpenedDir));

	// Save if the isSortingEnabled is set or not.
	//settings.setValue("gamelist/isSortingEnabled", m_gTabAndList->isSortingEnabled());
}

bool MainWindow::saveFile(const QString& filePath) const
{
	// Saving the game list at filePath.
	// If the filePath is empty, stop.
	if (filePath.isEmpty())
	{
		QMessageBox::warning((QWidget*)this, tr("Error saving file"), tr("Error: cannot save file %1, game list is empty.").arg(filePath), QMessageBox::Ok);
		return false;
	}

	// getting the games data and tabs names
	TabAndListData gamesData = m_gTabAndList->gamesData();

	// Storing to a QByteArray
	QByteArray bArray;
	// QDataStream is a usefull hight level interface for manupilate data stream
	QDataStream data(&bArray, QIODevice::WriteOnly);

	// Write the file identifier, it's 3 char that is used when opening the file
	// to reconize the data format of the file.
	const char* fileIdentifier = DEFINED_GLD_IDENTIFIER;
	data.writeRawData(fileIdentifier, 3);

	// writing the data version of the file
	data << (quint32)DEFINED_GLD_VERSION;

	// Writing the number of existing tabs available.
	// It is uses when opening the file to know how many tabs we need to read.
	data << (quint32)gamesData.tabCount;

	// if there is existing tab, writing everytihing on the QByteArray, otherwise, write nothing, it's the end of the file.
	if (gamesData.tabCount > 0)
	{
		// First writing all the tab names.
		for (int i = 0; i < gamesData.tabCount; i++)
			data << gamesData.tabNames.at(i);

		// now, writing all the games data from each tab
		for (int i = 0; i < gamesData.tabCount; i++)
		{
			// store the number of games  there is on this tab
			data << (quint32)gamesData.gamesData.at(i).size();

			data << gamesData.gamesListSortIndicator.at(i);

			// if there is any games on this tab, write it, otherwise, write nothing
			for (int j = 0; j < gamesData.gamesData.at(i).size(); j++)
			{
				const GameListModel::GameList& g = gamesData.gamesData.at(i).at(j);

				data << g.name;
				data << g.type;
				data << (quint8)g.rate.starCount();
			}
		}
	}
	data.setDevice(nullptr);

	/*
	* Now, write everything on a file.
	* QSaveFile is usefull for saving file, because when
	* we replace an existing file, the existing file is is deleted then
	* we rewrite on it, but QSaveFile don't replace directly the existing file, it's write everything
	* on a temporary file and then, if the writing success, it replace the existing file
	* with the temporary file. If the process fail to write the file, the existing file is not lost.
	*/
	QSaveFile file(filePath);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(bArray);

		if (!file.commit())
			QMessageBox::warning((QWidget*)this, tr("Error saving file"),
				tr("Cannot write the file %1.").arg(filePath), QMessageBox::Ok);
	}
	else
	{
		QMessageBox::warning((QWidget*)this, tr("Error saving file"),
			tr("Cannot opening or creating the file %1 for writing.").arg(filePath), QMessageBox::Ok);
		return false;
	}
	
	return true;
}

const TabAndListData MainWindow::openFile(const QString& fileName, bool& result) const
{
	std::function<void(const QString&)> wMsg = [this](const QString& message)->void
	{ QMessageBox::warning((QWidget*)this, tr("Error opening file"), message, QMessageBox::Ok); };
	// Opening the file. Same has save.
	// if the filePath is empty, stop.
	if (fileName.isEmpty())
	{
		wMsg(tr("Error: cannot open the file because the file path is empty."));
		result = false;
		return {};
	}

	// Opening the file
	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly))
	{
		// if the file size is less than the GLD data stucture, it's not a valid data file.
		if (file.size() < 11)
		{
			wMsg(tr("Error, the file %1 is not a valid gld file data.").arg(fileName));
			result = false;
			return {};
		}

		// Reading all the file in a byte array.
		QByteArray bData = file.readAll();
	
		// no need file anymore, it can be close.
		file.close();

		// Using the QDataStream class for an hight level interface with the bits data.
		QDataStream dataStream(&bData, QIODevice::ReadOnly);

		// get the first 3 char and see if it's "GLD"
		char identifier[4] = { 0, 0, 0, '\0' };
		dataStream.readRawData(&identifier[0], 3);
		if (strcmp(&identifier[0], DEFINED_GLD_IDENTIFIER) != 0)
		{
			wMsg(tr("Error: file %1 is not a valid GLD file.").arg(fileName));
			result = false;
			return {};
		}

		// verify the version of the file data structure, if it's a different version, 
		// it's mean it cannot be read correctly.
		quint32 fileVersion;
		dataStream >> fileVersion;

		if (fileVersion == DEFINED_GLD_VERSION_2 || fileVersion == DEFINED_GLD_VERSION_3)
		{
			bool file002Result;
			TabAndListData gamesListData = openFileVersion002And003(dataStream, fileVersion == DEFINED_GLD_VERSION_3, file002Result);
			if (!file002Result)
			{
				wMsg(tr("Error: fail to read file %1.").arg(fileName));
				result = false;
				return {};
			}
			else
			{
				result = true;
				return gamesListData;
			}
		}
		else
		{
			wMsg(tr("Error: file %1 is not a valid GLD file format.").arg(fileName));
			result = false;
			return {};
		}
	}
	else
	{
		wMsg(tr("Cannot open file %1.").arg(fileName));
		result = false;
		return {};
	}

	/*// returning a default empty game list if opening the file failed.
	return {};*/
}

const TabAndListData MainWindow::openFileVersion002And003(QDataStream& dataStream, bool v003, bool& result) const
{
	if (dataStream.atEnd()) { result = false; return {}; }

	TabAndListData listData = {};

	quint32 numberOfTabs;
	dataStream >> numberOfTabs;

	if (numberOfTabs == 0)
	{
		listData.tabCount = 0;
		result = false;
		return listData;
	}
	else
		listData.tabCount = numberOfTabs;

	if (dataStream.atEnd()) { result = false; return {}; }

	listData.tabNames.resize(numberOfTabs);
	listData.gamesData.resize(numberOfTabs);
	listData.gamesListSortIndicator.resize(numberOfTabs);

	for (int i = 0; i < static_cast<int>(numberOfTabs); i++)
	{
		if (dataStream.atEnd()) { result = false; return {}; }

		QString tabName;
		dataStream >> tabName;
		listData.tabNames[i] = tabName;
	}

	for (int i = 0; i < static_cast<int>(numberOfTabs); i++)
	{
		if (dataStream.atEnd()) { result = false; return {}; }

		quint32 gamesCount;
		dataStream >> gamesCount;
		listData.gamesData[i].resize(gamesCount);

		if (v003)
		{
			GameListViewSortIndicator sortIndicator;
			dataStream >> sortIndicator;
			listData.gamesListSortIndicator[i] = sortIndicator;
		}
		else
		{
			GameListViewSortIndicator sortIndicator;
			sortIndicator.isEnabled = false;
			sortIndicator.sortColumn = 0;
			sortIndicator.sortOrder = static_cast<quint8>(Qt::DescendingOrder);
			listData.gamesListSortIndicator[i] = sortIndicator;
		}

		for (int j = 0; j < static_cast<int>(gamesCount); j++)
		{
			if (dataStream.atEnd()) { result = false; return {}; }
			
			GameListModel::GameList gameList = {};

			QString gameName, gameType;
			quint8 gameRate;

			dataStream >> gameName;

			if (dataStream.atEnd()) { result = false; return {}; }

			dataStream >> gameType;

			if (dataStream.atEnd()) { result = false; return {}; }

			dataStream >> gameRate;

			listData.gamesData[i][j].name = gameName;
			listData.gamesData[i][j].type = gameType;
			listData.gamesData[i][j].rate = GameStarRating(static_cast<int>(gameRate));
		}
	}

	result = true;
	return listData;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	/*
	* Called when the window is closing.
	* If there is pending change, the code below ask the user if he want to save pending change.
	* If finally the user don't want to exit the application when he asked to save pending change,
	* we ignore the leaving event.
	*/
	if (!maybeSave())
		e->ignore();
	else
	{
		writeSettings();
		e->accept();
	}
}

bool MainWindow::maybeSave()
{
	// If there is pending change, the code below tell the user 
	// if he want to save the pending change.
	if (m_gTabAndList->isListEdited())
	{
		QMessageBox::StandardButton result =
			QMessageBox::warning(this,
				tr("Pending change!"),
				tr("You have pending change not saved.\nWould you like to save them ?"),
				QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
				QMessageBox::Save);

		switch (result)
		{
		case QMessageBox::Save:
		{
			bool result = saveGameList();
			if (result)
				m_gTabAndList->pendingChangeSaved();
			return result;
		}
		case QMessageBox::Discard:
			return true;
		case QMessageBox::Cancel:
			return false;
		}
	}

	return true;
}

void MainWindow::setFileNameIntoWindowTitle()
{
	// Showing the file name into the window title.
	// Showing a '*' before the file name if the list 
	// has been edited.
	QString applicationName("Game Sorting");
	QString fileName;

	if (m_gFilePath.isEmpty())
		fileName = QString("%1.gld").arg(tr("untitled"));
	else
		fileName = QFileInfo(m_gFilePath).fileName();

	if (m_gTabAndList->isListEdited())
		setWindowTitle(QString("%1 : *%2").arg(applicationName, fileName));
	else
		setWindowTitle(QString("%1 : %2").arg(applicationName, fileName));
}

void MainWindow::aboutApp()
{
	// Show a little dialog about this application.

	if (!m_aboutDialog)
	{
		m_aboutDialog = new AboutDialog(this);
		connect(m_aboutDialog, &QDialog::destroyed, [this]() { m_aboutDialog = nullptr; });
		m_aboutDialog->raise();
		m_aboutDialog->activateWindow();
	}

	m_aboutDialog->show();
}