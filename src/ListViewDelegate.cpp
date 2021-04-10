/*
* MIT Licence
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

#include "ListViewDelegate.h"
#include "TableModel_UtilityInterface.h"
#include "UtilityInterfaceEditor.h"
#include "Common.h"
#include "TableModel.h"
#include "StarEditor.h"
#include <iostream>

ListViewDelegate::ListViewDelegate(
	TableModel* tableModel,
	SqlUtilityTable& utilityTable,
	QSqlDatabase& db,
	QObject* parent) :
		QStyledItemDelegate(parent),
		m_tableModel(tableModel),
		m_utilityTable(utilityTable),
		m_utilityInterface(m_tableModel->utilityInterface()),
		m_db(db)
{}

void ListViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Painting the delegate.
	if (m_tableModel->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			// Painting the stars.
			StarEditor::paintStars(index.data().toInt(), painter, option.rect, option.palette);
			return;
		}
	}

	QStyledItemDelegate::paint(painter, option, index);
}

QSize ListViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Returning the recommanded size of the field.
	if (m_tableModel->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			return StarEditor::sizeHint(5);
		}
	}

	return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* ListViewDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Creating the editor 
	if (m_tableModel->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			// Creating the editor for the edition of the rate column.
			StarEditor* editor = new StarEditor(parent);
			connect(editor, &StarEditor::editFinished, this, &ListViewDelegate::commitAndCloseEditor);
			return editor;
		}
		else if (index.column() == Game::CATEGORIES ||
				 index.column() == Game::DEVELOPPERS ||
				 index.column() == Game::PUBLISHERS ||
				 index.column() == Game::PLATFORMS ||
				 index.column() == Game::SERVICES)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID < 0)
				return nullptr;
			
			UtilityTableName tableName;

			if (index.column() == Game::CATEGORIES)
				tableName = UtilityTableName::CATEGORIES;
			else if (index.column() == Game::DEVELOPPERS)
				tableName = UtilityTableName::DEVELOPPERS;
			else if (index.column() == Game::PUBLISHERS)
				tableName = UtilityTableName::PUBLISHERS;
			else if (index.column() == Game::PLATFORMS)
				tableName = UtilityTableName::PLATFORM;
			else if (index.column() == Game::SERVICES)
				tableName = UtilityTableName::SERVICES;
			
			UtilityInterfaceEditor* editor = new UtilityInterfaceEditor(
				tableName,
				itemID,
				m_tableModel,
				m_utilityInterface,
				m_utilityTable,
				m_db,
				parent);
			editor->raise();
			editor->activateWindow();
			editor->show();
			return nullptr;
		}
	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}

void ListViewDelegate::setEditorData(QWidget* e, const QModelIndex& index) const
{
	// Setting the fild data to the editor.
	const TableModel* model = reinterpret_cast<const TableModel*>(index.model());

	if (model->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			StarEditor* editor = reinterpret_cast<StarEditor*>(e);
			editor->setStars(index.data().toInt());
		}
	}

	return QStyledItemDelegate::setEditorData(e, index);
}

void ListViewDelegate::setModelData(QWidget* e, QAbstractItemModel* m, const QModelIndex& index) const
{
	// When the edit is finished, apply the editor data to the field.
	TableModel* model = reinterpret_cast<TableModel*>(m);

	if (model->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			StarEditor* editor = reinterpret_cast<StarEditor*>(e);
			model->setData(index, editor->stars());
		}
	}

	return QStyledItemDelegate::setModelData(e, m, index);
}

void ListViewDelegate::commitAndCloseEditor(QWidget* editor)
{
	// Commit the data of the editor and close it.
	if (editor)
	{
		emit commitData(editor);
		emit closeEditor(editor);
	}
}