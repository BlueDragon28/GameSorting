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

#ifndef GAMESORTING_GAMELISTVIEW
#define GAMESORTING_GAMELISTVIEW

#include <QWidget>
#include "GameListModel.h"

class ListTableView;
class QTableView;
class GameListSortFilterProxyModel;

class GameListView : public QWidget
{
	Q_OBJECT
public:
	explicit GameListView(QWidget* parent = nullptr);
	~GameListView();

	QTableView* view();
	GameListModel* model();
	const QVector<GameListModel::GameList>& gamesData() const;
	int gamesCount() const;
	bool isSortingEnabled() const;
	bool isListFiltered() const;
	QString GameFilteringTypeText() const;

public slots:
	void appendGame();
	void appendGame(const GameListModel::GameList& game);
	void appendGames(const QVector<GameListModel::GameList>& games);
	void removeGames();
	void clearGameList();
	void setSortingEnabled(bool isEnabled);
	void filterGamesFromType(const QString& type);
	void copyToClipboard() const;
	void copyAllColumnToClipboard() const;
	void pasteFromClipboard();

signals:
	void listEdited();
	void listFiltered(bool isFiltered);

private slots:
	void filteringTheList(bool value);

private:
	void createLayout();
	void makeGameRatingAtSizeHint();

private:
	ListTableView* m_view;
	GameListModel* m_model;
	bool m_noGameAddedToList;
	bool m_isSortingEnabled;
	bool m_listIsFiltered;
};

#endif // GAMESORTING_GAMELISTVIEW