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

#ifndef GAMESORTING_COMMONSTRUCT
#define GAMESORTING_COMMONSTRUCT

#include <QVector>
#include <QString>
#include "GameListModel.h"

#define GAMESORTING_VERSION "0.12"

struct GameListViewSortIndicator
{
	bool isEnabled;
	quint8 sortColumn;
	quint8 sortOrder;
};

QDataStream& operator<<(QDataStream& out, const GameListViewSortIndicator& value);
QDataStream& operator>>(QDataStream& in, GameListViewSortIndicator& value);

struct TabAndListData
{
	int tabCount;
	QVector<QString> tabNames;
	QVector<GameListViewSortIndicator> gamesListSortIndicator;
	QVector<QVector<GameListModel::GameList>> gamesData;
};

void removeEmptyStrings(QStringList& strList);

#endif // GAMESORTING_COMMONSTRUCT