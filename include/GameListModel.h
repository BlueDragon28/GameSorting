#ifndef GAMESORTING_GAMELISTMODEL
#define GAMESORTING_GAMELISTMODEL

#include <QAbstractTableModel>
#include <QTimer>

#include <thread>
#include <chrono>

#include "GameStarRating.h" 

class GameListModelUpdateSorting;

class GameListModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	struct GameList
	{
		QString name, type;
		GameStarRating rate;
	};

	GameListModel(QObject* parent = nullptr);
	~GameListModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	Qt::DropActions supportedDropActions() const override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	//bool moveRows(const QModelIndex& scrParent, int scrRow, int count, const QModelIndex& dstParent, int dstRow) override;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
	const GameList getGame(int index) const;
	const QVector<GameList>& getGameData() const;
	int size() const;
	bool isSortingEnabled() const;
	QString filteringTypeText() const;

signals:
	void listEdited();
	void emitUpdateSorting();
	void updateSorting();
	void listFiltered(bool isFiltered);

public slots:
	void appendGame();
	void appendGame(const GameList& game);
	void appendGames(const QVector<GameList>& games);
	void removeGames(QModelIndexList& selectionList);
	void clear();
	void setSortingEnabled(bool isEnabled);
	void filteringType(const QString& types);

private:
	bool lessThan(int column, const GameList& left, const GameList& right) const;
	bool greaterThan(int column, const GameList& left, const GameList& right) const;
	void waitBeforeUpdateSorting();
	void updateSortingWhenDataChanged();

	QVector<GameList*> m_filteredTypeData;
	QVector<GameList> m_data;
	bool m_isSortingEnabled;
	bool m_isFilteringTypeEnabled;
	int m_sortingColumn;
	Qt::SortOrder m_sortingOrder;
	QTimer m_waitBeforeUpdateSingleShotTimer;
	QString m_filteringTypes;
};

#endif // GAMESORTING_GAMELISTMODEL