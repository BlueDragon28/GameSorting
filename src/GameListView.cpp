#include "GameListView.h"
#include "GameStarDelegate.h"
#include "ListTableView.h"

#include <QGuiApplication>
#include <QItemSelectionModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QClipboard>

GameListView::GameListView(QWidget* parent) :
	QWidget(parent),
	m_view(new ListTableView()),
	m_model(new GameListModel()),
	m_noGameAddedToList(true),
	m_isSortingEnabled(false),
	m_listIsFiltered(false)
{
	// Creating the table view and the game list proxy model.
	m_view->setModel(m_model);
	m_view->setSortingEnabled(m_isSortingEnabled);
	// set the two first column to expend to all the size avalaible, except for the 3 column
	m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	// Hide the vertical header.
	m_view->verticalHeader()->hide();
	// Set the custom delegate for the stars
	// QTableView do not take ownership of item delegate, so we deleting it manually.
	QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
	m_view->setItemDelegate(new GameStarDelegate());
	delete itemDelegate;
	m_view->setSelectionBehavior(QTableView::SelectRows);
	m_view->setSelectionMode(QTableView::ExtendedSelection);
	m_view->setDragDropMode(QTableView::DragDrop);
	m_view->setAcceptDrops(true);
	m_view->setDragDropOverwriteMode(false);
	connect(m_model, &GameListModel::listEdited, [this]() { emit listEdited(); });
	connect(m_model, &GameListModel::listFiltered, this, &GameListView::filteringTheList);
	createLayout();
}

GameListView::~GameListView()
{
// Deleting all the memory allocated manually.
if (m_view)
{
	// the selection model from the view is not deleted manually.
	QAbstractItemDelegate* itemDelegate = m_view->itemDelegate();
	delete m_view;
	delete itemDelegate;
}
if (m_model)
delete m_model;
}

void GameListView::createLayout()
{
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(m_view);
	setLayout(vLayout);
}

QTableView* GameListView::view()
{
	return static_cast<QTableView*>(m_view);
}

GameListModel* GameListView::model()
{
	return m_model;
}

void GameListView::appendGame()
{
	m_model->appendGame();
}

void GameListView::appendGame(const GameListModel::GameList& game)
{
	m_model->appendGame(game);
}

void GameListView::appendGames(const QVector<GameListModel::GameList>& games)
{
	m_model->appendGames(games);
}

void GameListView::removeGames()
{
	QItemSelectionModel* selectionModel = m_view->selectionModel();
	if (selectionModel->hasSelection())
	{
		QModelIndexList gamesToRemoves = selectionModel->selectedRows(0);
		m_model->removeGames(gamesToRemoves);
	}
}

void GameListView::clearGameList()
{
	m_model->clear();
}

void GameListView::makeGameRatingAtSizeHint()
{
	if (m_noGameAddedToList)
		m_view->resizeColumnToContents(2);
}

const QVector<GameListModel::GameList>& GameListView::gamesData() const
{
	return m_model->getGameData();
}

int GameListView::gamesCount() const
{
	return m_model->size();
}

void GameListView::setSortingEnabled(bool isEnabled)
{
	m_model->setSortingEnabled(isEnabled);
	m_view->setSortingEnabled(isEnabled);
}

bool GameListView::isSortingEnabled() const
{
	return m_model->isSortingEnabled();
}

void GameListView::filterGamesFromType(const QString& types)
{
	m_model->filteringType(types);
}

void GameListView::copyToClipboard() const
{
	/*
	* This method is copying the selected games into the clipboard.
	* This method only copy the name of the game. Each games are seperated with
	* a line break '\n'.
	*/

	QClipboard* clipboard = QGuiApplication::clipboard();

	QItemSelectionModel* selectionModel = m_view->selectionModel();
	if (selectionModel->hasSelection())
	{
		QModelIndexList selectionList = selectionModel->selectedRows(0);
		if (selectionList.size() > 0)
		{
			QString gamesText;
			for (QModelIndexList::const_iterator it = selectionList.cbegin(); it != selectionList.cend(); it++)
				gamesText += m_model->getGame(it->row()).name + '\n';
			gamesText.remove(gamesText.size() - 1, 1);
			clipboard->setText(gamesText);
		}
	}
}

void GameListView::copyAllColumnToClipboard() const
{
	/*
	* This method is used to copy the selected games into the clipboard.
	* All the column are copying using the following format :
	* Name;Type;Rate. Each game is seperated by a line break '\n'.
	*/

	QClipboard* clipboard = QGuiApplication::clipboard();

	QItemSelectionModel* selectionModel = m_view->selectionModel();
	if (selectionModel->hasSelection())
	{
		QModelIndexList selectionList = selectionModel->selectedRows(0);
		if (selectionList.size() > 0)
		{
			QString gamesText;
			for (QModelIndexList::const_iterator it = selectionList.cbegin(); it != selectionList.cend(); it++)
			{
				gamesText += QString("%1;%2;%3\n")
					.arg(m_model->getGame(it->row()).name)
					.arg(m_model->getGame(it->row()).type)
					.arg(m_model->getGame(it->row()).rate.starCount());
			}
			gamesText.remove(gamesText.size() - 1, 1);
			clipboard->setText(gamesText);
		}
	}
}

void GameListView::pasteFromClipboard()
{
	/*
	* Pasting the content of the clipboard into the the game list.
	* The user can past different game by seperating each one with a
	* line break '\n'. The user can also paste the game with the
	* name, type and rating by seperating on the same line with ';'.
	* Also note that the space added by the user from the ';' and the
	* first caracter of the type or rate is preserved.
	* If there is no selected game in the view, the clipboard games are append to
	* the end of the list. Otherwise, the selected games are replaced by the clipboard
	* games.
	*/

	QClipboard* clipboard = QGuiApplication::clipboard();

	QString clipboardText = clipboard->text();

	if (!clipboardText.isEmpty())
	{
		QStringList gameNames = clipboardText.split('\n');
		for (QStringList::reverse_iterator it = gameNames.rbegin(); it != gameNames.rend(); it++)
		{
			if ((*it).isEmpty())
				gameNames.erase(it.base());
		}

		/*QItemSelectionModel* selectionModel = m_view->selectionModel();

		if (selectionModel->hasSelection())
		{
			QList<QModelIndex> selectionList = selectionModel->selectedRows(0);

			int gameNamesIndex(0), gameNamesEnd(gameNames.size() - 1);

			for (int i = 0; i < selectionList.size(); i++)
			{
				QStringList strGameTypeRate = gameNames.at(gameNamesIndex).split(';');
				for (QStringList::reverse_iterator rit = gameNames.rbegin(); rit != gameNames.rend(); rit++)
				{
					if ((*rit).isEmpty())
						gameNames.erase(rit.base());
				}

				if (strGameTypeRate.size() > 0)
					m_model->setData(selectionList.at(i), QVariant::fromValue(strGameTypeRate.at(0)));
				if (strGameTypeRate.size() > 1)
					m_model->setData(m_model->index(selectionList.at(i).row(), 1), QVariant::fromValue(strGameTypeRate.at(1)));
				if (strGameTypeRate.size() > 2)
				{
					bool result;
					int rate = strGameTypeRate.at(2).toInt(&result);
					if (result)
						m_model->setData(m_model->index(selectionList.at(i).row(), 2), QVariant::fromValue(GameStarRating(rate)));
				}

				if (gameNamesIndex < gameNamesEnd)
					gameNamesIndex++;
			}
		}
		else
		{*/

		QVector<GameListModel::GameList> games(gameNames.size(), { "", "No categorie", 1 });

		for (int i = 0; i < gameNames.size(); i++)
		{
			QStringList strGameTypeRate = gameNames.at(i).split(';');
			for (QStringList::reverse_iterator rit = gameNames.rbegin(); rit != gameNames.rend(); rit++)
			{
				if ((*rit).isEmpty())
					gameNames.erase(rit.base());
			}

			if (strGameTypeRate.size() > 0)
				games[i].name = strGameTypeRate.at(0);
			if (strGameTypeRate.size() > 1)
				games[i].type = strGameTypeRate.at(1);
			if (strGameTypeRate.size() > 2)
			{
				bool result;
				int rate = strGameTypeRate.at(2).toInt(&result);
				if (result)
					games[i].rate = GameStarRating(rate);
			}
		}

		m_model->appendGames(games);
		//}
	}
}

void GameListView::filteringTheList(bool value)
{
	// This method is called when whenever the filtering 
	// is enabled or disabled in the model.
	m_listIsFiltered = value;
	emit listFiltered(value);
}

bool GameListView::isListFiltered() const
{
	return m_listIsFiltered;
}

QString GameListView::GameFilteringTypeText() const
{
	return m_model->filteringTypeText();
}