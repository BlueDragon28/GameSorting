#include "GameTextLineEditor.h"

#include <QKeyEvent>
#include <QTabBar>

GameTextLineEditor::GameTextLineEditor(int tabIndex, QTabBar* tabBar, QWidget* parent) :
	QLineEdit(parent),
	m_tabIndex(tabIndex),
	m_tabBar(tabBar)
{
	setText(m_tabBar->tabText(m_tabIndex));
	selectAll();
	setGeometry(m_tabBar->tabRect(m_tabIndex));
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	show();
	setFocus();
	connect(this, &GameTextLineEditor::editingFinished, this, &GameTextLineEditor::updateTabBarName);
}

void GameTextLineEditor::keyReleaseEvent(QKeyEvent* evt)
{
	if (evt->key() == Qt::Key_Escape)
		emit editingCancelled();
}

void GameTextLineEditor::resizeToTab()
{
	setGeometry(m_tabBar->tabRect(m_tabIndex));
}

void GameTextLineEditor::updateTabBarName()
{
	m_tabBar->setTabText(m_tabIndex, text());
}