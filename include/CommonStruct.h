#ifndef GAMESORTING_COMMONSTRUCT
#define GAMESORTING_COMMONSTRUCT

#include <QVector>
#include <QString>
#include "GameListModel.h"

#define GAMESORTING_VERSION "0.11"

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