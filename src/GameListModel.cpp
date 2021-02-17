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

#include "GameListModel.h"
#include "CommonStruct.h"

#include <QVector>
#include <QModelIndex>

#include <algorithm>

GameListModel::GameListModel(QObject* parent) :
	QAbstractTableModel(parent),
	m_isSortingEnabled(false),
	m_isFilteringTypeEnabled(false),
	m_sortingColumn(-1),
	m_sortingOrder(Qt::AscendingOrder),
	m_filteringTypes(QString())
{
	connect(this, &GameListModel::updateSorting, this, &GameListModel::updateSortingWhenDataChanged);
	connect(this, &GameListModel::emitUpdateSorting, this, &GameListModel::waitBeforeUpdateSorting);
	connect(&m_waitBeforeUpdateSingleShotTimer, &QTimer::timeout, this, &GameListModel::updateSorting);

	m_waitBeforeUpdateSingleShotTimer.setSingleShot(true);
	m_waitBeforeUpdateSingleShotTimer.setInterval(30);
}

GameListModel::~GameListModel()
{
}

int GameListModel::rowCount(const QModelIndex& parent) const
{
	return m_data.size();
}

int GameListModel::columnCount(const QModelIndex& parent) const
{
	return 3;
}

QVariant GameListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// Set a name for the columns.
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == 0)
			return tr("Game name");
		else if (section == 1)
			return tr("Game type");
		else if (section == 2)
			return tr("Game rate");
	}
	return QVariant();
}

QVariant GameListModel::data(const QModelIndex& index, int role) const
{
	// returning the corresponding data to the view and the delegate widget.
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if (m_isFilteringTypeEnabled && index.row() < m_filteredTypeData.size())
		{
			if (index.column() == 0)
				return m_filteredTypeData[index.row()]->name;
			else if (index.column() == 1)
				return m_filteredTypeData[index.row()]->type;
			else if (index.column() == 2)
				return QVariant::fromValue(m_filteredTypeData[index.row()]->rate);
		}
		else if (!m_isFilteringTypeEnabled && index.row() < m_data.size())
		{
			if (index.column() == 0)
				return m_data[index.row()].name;
			else if (index.column() == 1)
				return m_data[index.row()].type;
			else if (index.column() == 2)
				return QVariant::fromValue(m_data[index.row()].rate);
		}
	}
	
	return QVariant();
}

void GameListModel::appendGame()
{
	// Insert a new row in the array for a new game with a default name, type and rate.
	beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
	m_data.append({ tr("New game"), tr("No categorie"), GameStarRating() });
	endInsertRows();
	emit listEdited();
	emit emitUpdateSorting();
}

void GameListModel::appendGame(const GameList& newGame)
{
	// Insert a newGame in the array.
	beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
	m_data.append(newGame);
	endInsertRows();
	emit listEdited();
	emit emitUpdateSorting();
}

void GameListModel::appendGames(const QVector<GameList>& newGames, bool isSorting)
{
	// Insert newGames in the array.
	if (!newGames.isEmpty())
	{
		int begin(m_data.size()), end(m_data.size() - 1 + newGames.size());
		beginInsertRows(QModelIndex(), begin, end);
		m_data.append(newGames);
		endInsertRows();
		if (isSorting)
		{
			emit listEdited();
			emit emitUpdateSorting();
		}
	}
}

const GameListModel::GameList GameListModel::getGame(int index) const
{
	// returning a game at the specified index.
	if (index < m_data.size())
		return m_data[index];
	else
		return GameList();
}

Qt::ItemFlags GameListModel::flags(const QModelIndex& index) const
{
	// set the flags of the itemView.
	return Qt::ItemIsEditable | /*(index.isValid() ? Qt::ItemIsDragEnabled : 0) | (index.isValid() ? Qt::ItemIsDropEnabled : 0) |*/ Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractTableModel::flags(index);
}

bool GameListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	/*
	* When data are modified from the user, we use this method to refresh the model.
	*/
	if (role == Qt::EditRole)
	{
		if (!checkIndex(index))
			return false;
		if (m_isFilteringTypeEnabled && index.row() < m_filteredTypeData.size())
		{
			if (index.column() == 0)
				m_filteredTypeData[index.row()]->name = qvariant_cast<QString>(value);
			else if (index.column() == 1)
				m_filteredTypeData[index.row()]->type = qvariant_cast<QString>(value);
			else if (index.column() == 2)
				m_filteredTypeData[index.row()]->rate = qvariant_cast<GameStarRating>(value);
			emit listEdited();
			emit dataChanged(index, index, { role });
		}
		else if (!m_isFilteringTypeEnabled && index.row() < m_data.size())
		{
			if (index.column() == 0)
				m_data[index.row()].name = qvariant_cast<QString>(value);
			else if (index.column() == 1)
				m_data[index.row()].type = qvariant_cast<QString>(value);
			else if (index.column() == 2)
				m_data[index.row()].rate = qvariant_cast<GameStarRating>(value);
			emit listEdited();
			emit dataChanged(index, index, { role });
			emit emitUpdateSorting();
		}
		else
			return false;
	}

	return true;
}

void GameListModel::removeGames(QModelIndexList& selectionList)
{
	/*
	* Remove games from the list.
	* The best way to remove item from an array is to start from the end.
	* The first things is to sort the list, it is necessary, because we start
	* deleting the games from the end.
	*/
	QModelIndexList selectionListCopy(selectionList);
	std::sort(selectionListCopy.begin(), selectionListCopy.end(),
		[selectionList](const QModelIndex& index1, const QModelIndex& index2) -> bool
		{
			return index1.row() < index2.row();
		});

	if (!selectionListCopy.isEmpty())
	{
		QModelIndexList::const_reverse_iterator it = selectionListCopy.crbegin();
		while (it != selectionListCopy.crend())
		{
			beginRemoveRows(QModelIndex(), it->row(), it->row());
			m_data.remove(it->row());
			it++;
			endRemoveRows();
		}
		emit listEdited();
	}
}

int GameListModel::size() const
{
	return m_data.size();
}

void GameListModel::clear()
{
	// Remove all the games in the list
	if (!m_data.isEmpty())
	{
		beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
		m_data.clear();
		endRemoveRows();
		emit listEdited();
	}
}

const QVector<GameListModel::GameList>& GameListModel::getGameData() const
{
	return m_data;
}

Qt::DropActions GameListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

bool GameListModel::insertRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || row > m_data.size()) return false;
	beginInsertRows(parent, row, row + count - 1);
	for (int i = 0; i < count; i++)
	{
		GameList gameList = {};
		gameList.name = "New game";
		gameList.type = "No categorie";
		m_data.insert(row + i, gameList);
	}
	endInsertRows();

	return true;
}

bool GameListModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row < 0 || row >= m_data.size()) return false;
	beginRemoveRows(parent, row, row + count - 1);
	m_data.remove(row, count);
	endRemoveRows();

	return true;
}

bool GameListModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	if (row == -1)
		row = rowCount();

	return QAbstractTableModel::dropMimeData(data, action, parent.isValid() ? parent.row() : row, 0, QModelIndex());
}

void GameListModel::sort(int column, Qt::SortOrder order)
{
	// Sorting all the list depending of the column and the order.

	if (m_isFilteringTypeEnabled || ( column < 0 && column >= 3)) return;

	m_sortingColumn = column;
	m_sortingOrder = order;

	std::stable_sort(m_data.begin(), m_data.end(),
		[this, order, column](const GameListModel::GameList& game1, const GameListModel::GameList& game2) -> bool
		{
			if (order == Qt::AscendingOrder)
				return lessThan(column, game1, game2);
			else
				return greaterThan(column, game1, game2);
		});

	emit dataChanged(index(0, 0), index(m_data.size() - 1, 2), { Qt::DisplayRole });
	if (m_data.size() > 0)
		emit listEdited();
}

bool GameListModel::lessThan(int column, const GameListModel::GameList& left, const GameListModel::GameList& right) const
{
	if (column == 0)
		return left.name.compare(right.name) < 0;
	else if (column == 1)
		return left.type.compare(right.type) < 0;
	else if (column == 2)
		return left.rate < right.rate;
	else
		return true;
}

bool GameListModel::greaterThan(int column, const GameListModel::GameList& left, const GameListModel::GameList& right) const
{
	if (column == 0)
		return left.name.compare(right.name) > 0;
	else if (column == 1)
		return left.type.compare(right.type) > 0;
	else if (column == 2)
		return left.rate > right.rate;
	else
		return true;
}

void GameListModel::updateSortingWhenDataChanged()
{
	if (m_isSortingEnabled)
	{
		sort(m_sortingColumn, m_sortingOrder);
	}
}

void GameListModel::setSortingEnabled(bool isEnabled)
{
	m_isSortingEnabled = isEnabled;
}

bool GameListModel::isSortingEnabled() const
{
	return m_isSortingEnabled;
}

void GameListModel::waitBeforeUpdateSorting()
{
	/*
	* It's a little work around of a bug, when the first time the function setData is called
	* when the auto sorting is set, the sorting is called and the view don't have the time to
	* set all the columns data. To fix this, this function is sorting the list only after a short time,
	* this let the view to set the data of all the views.
	*/
	if (m_isSortingEnabled)
	{
		if (m_waitBeforeUpdateSingleShotTimer.isActive())
			m_waitBeforeUpdateSingleShotTimer.stop();

		m_waitBeforeUpdateSingleShotTimer.start();
	}
}

void GameListModel::filteringType(const QString& types)
{
	/*
	* This method is use to filter the list to only show games having
	* the specified "types" in there type column.
	* If the types is an empty string, the method take it as if the user want to disable
	* the filtering. Otherwise, the method split the types in list of string for every ','
	* caracters found. If the game had one the specified types, it's pointer is stored in a separated
	* array. When the filtering is done, the view is updated to only show the content of
	* the separated array "m_filteredTypeData" instead of the base "m_data" array.
	*/
	m_filteringTypes = types;

	if (types.isEmpty())
	{
		if (m_filteredTypeData.size() > 0)
		{
			beginRemoveRows(QModelIndex(), 0, m_filteredTypeData.size() - 1);
			endRemoveRows();
			m_filteredTypeData.clear();
		}
		m_isFilteringTypeEnabled = false;
		emit listFiltered(false);
		if (m_data.size() > 0)
		{
			beginInsertRows(QModelIndex(), 0, m_data.size() - 1);
			endInsertRows();
		}
	}
	else
	{
		if (m_filteredTypeData.size() > 0)
		{
			if (m_isFilteringTypeEnabled)
			{
				beginRemoveRows(QModelIndex(), 0, m_filteredTypeData.size() - 1);
				endRemoveRows();
				m_filteredTypeData.clear();
				m_isFilteringTypeEnabled = false;
			}
		}

		if (m_data.size() > 0)
		{
			if (!m_isFilteringTypeEnabled)
			{
				beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
				endRemoveRows();
			}

			for (QVector<GameList>::iterator it = m_data.begin(); it != m_data.end(); it++)
			{
				QStringList listTypes = types.split(',');
				removeEmptyStrings(listTypes);
				QStringList gameTypes = it->type.split(',');
				removeEmptyStrings(gameTypes);

				bool isTypeAvailable = false;

				GameList* pGame = nullptr;

				for (QStringList::const_iterator gameIt = gameTypes.cbegin(); gameIt != gameTypes.cend(); gameIt++)
				{
					for (QStringList::const_iterator listIt = listTypes.cbegin(); listIt != listTypes.cend(); listIt++)
					{
						if ((*gameIt).compare(*listIt) == 0)
						{
							isTypeAvailable = true;
							pGame = &(*it);
							break;
						}
					}
				}
				if (pGame)
					m_filteredTypeData.append(&(*it));
			}

			if (m_filteredTypeData.size() > 0)
			{
				beginInsertRows(QModelIndex(), 0, m_filteredTypeData.size() - 1);
				endInsertRows();
			}

			m_isFilteringTypeEnabled = true;
			emit listFiltered(true);
		}
	}
}

QString GameListModel::filteringTypeText() const
{
	return m_filteringTypes;
}