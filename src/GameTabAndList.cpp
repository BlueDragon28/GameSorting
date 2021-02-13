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

#include "GameTabAndList.h"
#include "GameStarDelegate.h"
#include "GameTextLineEditor.h"
#include "GameListView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QTableView>
#include <QHeaderView>
#include <QLineEdit>
#include <QResizeEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedLayout>

#include <iostream>

GameTabAndList::GameTabAndList(QWidget* parent) :
	QWidget(parent),
	m_tabBar(new QTabBar(this)),
	m_gViews(new QStackedLayout()),
	m_isListEdited(false)
{
	m_tabBar->setTabsClosable(true);
	m_tabBar->setMovable(true);
	m_tabBar->setUsesScrollButtons(true);
	m_tabBar->setChangeCurrentOnDrag(true);
	m_tabBar->setAcceptDrops(true);

	connect(m_tabBar, &QTabBar::tabCloseRequested, this, &GameTabAndList::removeTab);
	connect(m_tabBar, &QTabBar::currentChanged, this, &GameTabAndList::tabChanged);
	connect(m_tabBar, &QTabBar::tabBarDoubleClicked, this, &GameTabAndList::editTabBarName);
	connect(m_tabBar, &QTabBar::tabMoved, this, &GameTabAndList::tabBarMoved);

	createLayout();
	restoreToDefault();
}

GameTabAndList::~GameTabAndList()
{
	if (m_tabBar)
	{
		for (int i = 0; i < m_tabBar->count(); i++)
		{
			if (m_gViews)
			{
				QWidget* view = m_gViews->widget(i);
				if (view)
					m_gViews->removeWidget(view);
				m_tabBar->removeTab(i);
				delete view;
			}
		}
	}
}

void GameTabAndList::createLayout()
{
	// Create the layout and placing in the tabbar and the stacklayout
	QHBoxLayout* hLayout = new QHBoxLayout();
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(0);
	hLayout->addWidget(m_tabBar, 1);
	QIcon addIcon(":/Images/Add.svg");
	QPushButton* addTabButton = new QPushButton();
	addTabButton->setIcon(addIcon);
	connect(addTabButton, &QPushButton::clicked, this, &GameTabAndList::addTab);
	hLayout->addWidget(addTabButton, 0);

	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(0);
	vLayout->addLayout(hLayout, 0);
	vLayout->addLayout(m_gViews, 1);

	setLayout(vLayout);
}

bool GameTabAndList::appendGame(int index)
{
	// Append a new game at the tab index.
	// if the index if incorrect, leaving the function and return false.
	if (!checkTabIndex(index)) return false;

	// If the index is equal to -1, using the current index instead.
	GameListView* view;
	if (index == -1)
		view = static_cast<GameListView*>(m_gViews->currentWidget());
	else
		view = static_cast<GameListView*>(m_gViews->widget(index));

	view->appendGame();
	return true;
}

bool GameTabAndList::appendGame(const GameListModel::GameList& game, int index)
{
	// Append a new game at the tab index.
	// if the index if incorrect, leaving the function and return false.
	if (!checkTabIndex(index)) return false;

	// If the index is equal to -1, using the current index instead.
	GameListView* view;
	if (index == -1)
		view = static_cast<GameListView*>(m_gViews->currentWidget());
	else
		view = static_cast<GameListView*>(m_gViews->widget(index));

	view->appendGame(game);
	return true;
}

bool GameTabAndList::appendGames(const QVector<GameListModel::GameList>& games, int index)
{
	// Append games to the tab index.
	// if the index if incorrect, leaving the function and return false.
	if (!checkTabIndex(index)) return false;

	// If the index is equal to -1, using the current index instead.
	GameListView* view;
	if (index == -1)
		view = static_cast<GameListView*>(m_gViews->currentWidget());
	else
		view = static_cast<GameListView*>(m_gViews->widget(index));

	view->appendGames(games);
	return true;
}

bool GameTabAndList::removeGames(int index)
{
	// Remove a game from the tab index.
	// if the index if incorrect, leaving the function and return false.
	if (!checkTabIndex(index)) return false;

	// If the index is equal to -1, using the current index instead.
	GameListView* view;
	if (index == -1)
		view = static_cast<GameListView*>(m_gViews->currentWidget());
	else
		view = static_cast<GameListView*>(m_gViews->widget(index));

	view->removeGames();
	return true;
}

bool GameTabAndList::clearGamesList(int index)
{
	// Clear all the game in the tab index.
	// if the index if incorrect, leaving the function and return false.
	if (!checkTabIndex(index)) return false;

	// If the index is equal to -1, using the current index instead.
	GameListView* view;
	if (index == -1)
		view = static_cast<GameListView*>(m_gViews->currentWidget());
	else
		view = static_cast<GameListView*>(m_gViews->widget(index));

	view->clearGameList();
	return true;
}

void GameTabAndList::tabChanged(int index)
{
	// When the tab is changed, change the current index from stacklayout to the currentIndex from the tabBar
	if (index < 0 || index >= m_tabBar->count()) return;

	m_gViews->setCurrentIndex(index);
	GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
	if (view)
	{
		// This signal is used to update or enable and disable the menu actions.
		emit gameListSorted(view->isSortingEnabled());
		emit gameListFiltered(view->isListFiltered());
	}
}

void GameTabAndList::addTab()
{
	// Adding a new tab.
	GameListView* view = new GameListView();
	view->setSortingEnabled(false);
	m_gViews->addWidget(view);
	m_tabBar->addTab(tr("New game list"));
	m_isListEdited = true;
	connect(view, &GameListView::listEdited, this, &GameTabAndList::listHasBeenEdited);
	connect(view, &GameListView::listFiltered, this, &GameTabAndList::gameListFiltered);

	// If the tabBar has more than 1 tab, allow the tabs to be closable.
	if (m_tabBar->count() > 1)
		m_tabBar->setTabsClosable(true);

	// Used to ask the user to save when is closing the app.
	emit listEdited();
}

void GameTabAndList::removeTab(int index)
{
	// Remove a tab.
	if (index < 0 && index >= m_tabBar->count()) return;

	// Get the view from the stacked layout.
	GameListView* view = static_cast<GameListView*>(m_gViews->widget(index));

	// If there is games in this tab, ask the user if he realy want to delete the tab.
	if (view->gamesCount() > 0)
	{
		int result = QMessageBox::warning(this,
			tr("Delete tab request"),
			tr("There is games data in the tab \"%1\", do you realy want to delete it?").arg(m_tabBar->tabText(index)),
			QMessageBox::Yes,
			QMessageBox::No);

		// If the user anwser no, we leaving the method.
		if (result == QMessageBox::No)
			return;
	}

	// Deleting the tab.
	m_tabBar->removeTab(index);
	m_gViews->removeWidget(view);
	m_isListEdited = true;

	// If the tabBar has 1 or less tabs, disable the tabs to be closable.
	if (m_tabBar->count() <= 1)
		m_tabBar->setTabsClosable(false);

	// Used to ask the user to save when is closing the app.
	emit listEdited();
}

void GameTabAndList::clearTabs()
{
	// clear all avalaible tabs
	for (int i = m_tabBar->count() - 1; i >= 0; i--)
	{
		m_tabBar->removeTab(i);
		m_gViews->removeWidget(m_gViews->widget(i));
	}
	m_isListEdited = false;
}

void GameTabAndList::restoreToDefault()
{
	// Clear alls the available tabs and create default tabs 
	// (usally used when creating a new game list).
	clearTabs();

	GameListView *gView1 = new GameListView(),
				 *gView2 = new GameListView(),
				 *gView3 = new GameListView();

	gView1->setSortingEnabled(false);
	gView2->setSortingEnabled(false);
	gView3->setSortingEnabled(false);

	m_gViews->addWidget(gView1);
	m_tabBar->addTab(tr("Already played games"));
	m_gViews->addWidget(gView2);
	m_tabBar->addTab(tr("Games checked"));
	m_gViews->addWidget(gView3);
	m_tabBar->addTab(tr("Games idea"));

	connect(gView1, &GameListView::listEdited, this, &GameTabAndList::listHasBeenEdited);
	connect(gView1, &GameListView::listFiltered, this, &GameTabAndList::gameListFiltered);
	connect(gView2, &GameListView::listEdited, this, &GameTabAndList::listHasBeenEdited);
	connect(gView2, &GameListView::listFiltered, this, &GameTabAndList::gameListFiltered);
	connect(gView3, &GameListView::listEdited, this, &GameTabAndList::listHasBeenEdited);
	connect(gView3, &GameListView::listFiltered, this, &GameTabAndList::gameListFiltered);

	m_tabBar->setTabsClosable(true);
}

bool GameTabAndList::isListEdited() const
{
	return m_isListEdited;
}

void GameTabAndList::pendingChangeSaved()
{
	m_isListEdited = false;
	emit listEdited();
}

TabAndListData GameTabAndList::gamesData() const
{
	// Returning in a struct, all the game data from all the tabs.
	// (usaly used to save the data to a file).
	TabAndListData gData = {};

	gData.tabCount = m_tabBar->count();

	if (gData.tabCount == 0) return gData;

	gData.tabNames.resize(gData.tabCount);
	gData.gamesData.resize(gData.tabCount);
	gData.gamesListSortIndicator.resize(gData.tabCount);
	for (int i = 0; i < m_tabBar->count(); i++)
	{
		gData.tabNames[i] = m_tabBar->tabText(i);

		GameListView* view = static_cast<GameListView*>(m_gViews->widget(i));

		gData.gamesListSortIndicator[i].sortColumn = view->view()->horizontalHeader()->sortIndicatorSection();
		gData.gamesListSortIndicator[i].sortOrder = view->view()->horizontalHeader()->sortIndicatorOrder();
		gData.gamesData[i] = view->gamesData();
		gData.gamesListSortIndicator[i].isEnabled = view->isSortingEnabled();
	}

	return gData;
}

void GameTabAndList::setGamesAndTabData(const TabAndListData& gamesData)
{
	// Apply all the game data from the struct gamesData to the tabBar and stackLayout
	for (int i = 0; i < gamesData.tabCount; i++)
	{
		GameListView* view = new GameListView();
		view->setSortingEnabled(gamesData.gamesListSortIndicator.at(i).isEnabled);
		view->appendGames(gamesData.gamesData.at(i), false); // If sorting is enabled in the file, it's not necessary to sorting it again.
		view->view()->horizontalHeader()->setSortIndicator(gamesData.gamesListSortIndicator.at(i).sortColumn, static_cast<Qt::SortOrder>(gamesData.gamesListSortIndicator.at(i).sortOrder));
		m_gViews->addWidget(view);
		m_tabBar->addTab(gamesData.tabNames.at(i));
		connect(view, &GameListView::listEdited, this, &GameTabAndList::listHasBeenEdited);
	}

	if (m_tabBar->count() > 1)
		m_tabBar->setTabsClosable(true);
	else
		m_tabBar->setTabsClosable(false);
}

void GameTabAndList::resizeEvent(QResizeEvent* event)
{
	// This signal is emit to update the edit tab bar name size when the window is resized.
	emit resized(event);
	QWidget::resizeEvent(event);
}

bool GameTabAndList::checkTabIndex(int index) const
{
	if (index >= -1 && index < m_tabBar->count())
		return true;
	else
		return false;
}

void GameTabAndList::tabBarMoved(int from, int to)
{
	// When a tabs is moved, apply the move to the stackLayout.
	if (from < 0 || from >= m_tabBar->count() ||
		to < 0 || to >= m_tabBar->count())
		return;

	QWidget* view = m_gViews->widget(from);
	m_gViews->removeWidget(view);
	view->setParent(nullptr);
	m_gViews->insertWidget(to, view);
	m_isListEdited = true;
}

void GameTabAndList::editTabBarName(int index)
{
	// When the user is double cliking to a tab, creating a QLineEdit to allow the user to edit the tab name.
	// If the index is not valid, leaving the method without doing anithing.
	if (index < 0 || index >= m_tabBar->count()) return;

	// If the editor is not destroyed, detroying it manually
	if (!m_textLineEditor)
	{
		m_textLineEditor->deleteLater();
		m_textLineEditor = nullptr;
	}

	// Create an line edit for editing the tab name
	m_textLineEditor = new GameTextLineEditor(index, m_tabBar, this);
	connect(this, &GameTabAndList::resized, m_textLineEditor, &GameTextLineEditor::resizeToTab);
	connect(m_textLineEditor, &GameTextLineEditor::editingFinished, [this]() { m_textLineEditor->deleteLater(); m_textLineEditor = nullptr; listHasBeenEdited(); });
	connect(m_textLineEditor, &GameTextLineEditor::editingCancelled, [this]() { m_textLineEditor->deleteLater(); m_textLineEditor = nullptr; });
}

void GameTabAndList::listHasBeenEdited()
{
	m_isListEdited = true;
	emit listEdited();
}

void GameTabAndList::setSortingEnabled(bool isEnabled)
{
	/*for (int i = 0; i < m_gViews->count(); i++)
		static_cast<GameListView*>(m_gViews->widget(i))->setSortingEnabled(isEnabled);*/
	GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
	if (view)
		view->setSortingEnabled(isEnabled);
}

bool GameTabAndList::isSortingEnabled() const
{
	GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
	if (view)
		return view->isSortingEnabled();
	return false;
}

void GameTabAndList::filterGamesFromType(const QString& types)
{
	// Filtering the current focussed games tab.
	if (m_tabBar->count() > 0)
	{
		GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
		view->filterGamesFromType(types);
		emit gameListFiltered(view->isListFiltered());
	}
}

void GameTabAndList::copyGameNameToClipboard() const
{
	// Copying the games name of the selected games of the current focussed games tab.
	if (m_tabBar->count() > 0)
	{
		GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
		view->copyToClipboard();
	}
}

void GameTabAndList::copyGameNameTypeAndRateToClipboard() const
{
	// Copying all the columns of the selected games of the current focussed games tab.
	if (m_tabBar->count() > 0)
	{
		GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
		view->copyAllColumnToClipboard();
	}
}

void GameTabAndList::pasteGameNameFromCliptboard()
{
	// Paste the clipboard into the current focussed games tab.
	if (m_tabBar->count() > 0)
	{
		GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
		view->pasteFromClipboard();
	}
}

QString GameTabAndList::gameFilteringTypeText() const
{
	GameListView* view = static_cast<GameListView*>(m_gViews->widget(m_tabBar->currentIndex()));
	if (view)
		return view->GameFilteringTypeText();
	return QString();
}