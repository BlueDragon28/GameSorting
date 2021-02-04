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

#ifndef GAMESORTING_MAINWINDOW
#define GAMESORTING_MAINWINDOW

#include <QMainWindow>
#include <QString>
#include "GameListModel.h"
#include "CommonStruct.h"

class GameTabAndList;
class QTableView;
class AboutDialog;
class LicenceDialog;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr);
	MainWindow(const QString& filePath, bool noSettings = false, QWidget* parent = nullptr);
	~MainWindow();

#ifndef NDEBUG
	void addGame();
#endif // NDEBUG

public slots:
	void newGameList();
	void openGameList();
	bool saveGameList();
	bool saveAsGameList();
	void filteringGameList();

protected:
	virtual void closeEvent(QCloseEvent* e);

private slots:
	void aboutApp();
	void aboutLicence();

private:
	void createMenu();
	void createCentralWidget();
	void readSettings(bool readSettings = true, bool loadLastSavedFile = true);
	void writeSettings();
	bool saveFile(const QString& filePath) const;
	const TabAndListData openFile(const QString& filePath, bool& result) const;
	const TabAndListData openFileVersion002And003(QDataStream& dataStream, bool v003, bool& result) const;
	void appendGame();
	void removeGame();
	bool maybeSave();
	void setFileNameIntoWindowTitle();

	GameTabAndList* m_gTabAndList;
	QString m_gFilePath, m_lastOpenedDir;
	AboutDialog* m_aboutDialog;
	LicenceDialog* m_licenceDialog;
};

#endif // GAMESORTING_MAINWINDOW