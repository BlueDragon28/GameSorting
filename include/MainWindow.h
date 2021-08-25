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

#ifndef GAMESORTING_MAINWINDOW
#define GAMESORTING_MAINWINDOW

#include "DataStruct.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QString>

class TabAndList;
class QTableView;
class QToolBar;
class LicenceDialog;
class AboutDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(const QString& filePath, bool resetSettings, bool doNotSaveSettings, QWidget* parent = nullptr);
	virtual ~MainWindow();

protected:
	void closeEvent(QCloseEvent* evt) override;

private:
	void createMenu();
	void createCentralWidget();
	void newListCreated(ListType type);
	void createGameToolBar();
	void createMovieToolBar();
	void listFilePathChanged(const QString& filePath);
	void listChanged(bool isChanged);
	void updateWindowTitle();
	void writeSettings();
	void readSettings();
	void showLicence();
	void about();
	void reinsertMenu();
	void updateRecentFileMenu();
	void removeOldRecentFile();
	void removeInvalidRecentFile(const QString& filePath);
	void openRecentFile(const QString& filePath);

	TabAndList* m_tabAndList;
	QSqlDatabase m_db;
	QToolBar* m_listToolBar;
	QString m_listFilePath;
	bool m_listChanged;
	
	bool m_isResetSettings;
	bool m_doNotSaveSettings;

	QMenu* m_fileMenu;
	QMenu* m_utilityMenu;
	QMenu* m_helpMenu;
	QMenu* m_recentFileMenu;
	QList<RecentFileData> m_recentFileData;
	LicenceDialog* m_licenceDialog;
	AboutDialog* m_aboutDialog;
};

#endif // GAMESORTING_MAINWINDOW