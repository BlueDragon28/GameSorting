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

#ifndef GAMESORTING_GAMETABANDLIST
#define GAMESORTING_GAMETABANDLIST

#include "GameListModel.h"
#include "CommonStruct.h"

#include <QWidget>
#include <QVector>
#include <QTabBar>

class QTableView;
class QLineEdit;
class QResizeEvent;
class GameListView;
class QStackedLayout;
class GameTextLineEditor;

class GameTabAndList : public QWidget
{
	Q_OBJECT
public:
	explicit GameTabAndList(QWidget* parent = nullptr);
	~GameTabAndList();

	TabAndListData gamesData() const;
	bool isSortingEnabled() const;
	bool isListEdited() const;
	QString gameFilteringTypeText() const;

signals:
	void resized(QResizeEvent* event);
	void gameListSorted(bool value);
	void gameListFiltered(bool value);
	void listEdited();

public slots:
	bool appendGame(int index = -1);
	bool appendGame(const GameListModel::GameList& game, int index = -1);
	bool appendGames(const QVector<GameListModel::GameList>& games, int index = -1);
	bool removeGames(int index = -1);
	bool clearGamesList(int index = -1);
	void restoreToDefault();
	void addTab();
	void removeTab(int index);
	void setGamesAndTabData(const TabAndListData& games);
	void clearTabs();
	void pendingChangeSaved();
	void setSortingEnabled(bool isEnabled);
	void filterGamesFromType(const QString& types);
	void copyGameNameToClipboard() const;
	void copyGameNameTypeAndRateToClipboard() const;
	void pasteGameNameFromCliptboard();

protected slots:
	void resizeEvent(QResizeEvent* event);

private slots:
	void tabChanged(int index);
	void listHasBeenEdited();

private:
	void createLayout();
	bool checkTabIndex(int index) const;
	void editTabBarName(int index);
	void tabBarMoved(int from, int to);

	QTabBar* m_tabBar;
	QStackedLayout* m_gViews;
	GameTextLineEditor* m_textLineEditor;
	bool m_isListEdited;
};

#endif // GAMESORTING_GAMETABANDLIST