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