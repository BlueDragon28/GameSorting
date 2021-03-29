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

#include "ListViewDelegate.h"
#include "Common.h"
#include "TableModel.h"
#include "StarEditor.h"
#include <iostream>

ListViewDelegate::ListViewDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{}

void ListViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Painting the delegate.
	const TableModel* model = reinterpret_cast<const TableModel*>(index.model());

	if (model->listType() == ListType::GAMELIST)
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
	const TableModel* model = reinterpret_cast<const TableModel*>(index.model());

	if (model->listType() == ListType::GAMELIST)
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
	const TableModel* model = reinterpret_cast<const TableModel*>(index.model());

	if (model->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			StarEditor* editor = new StarEditor(parent);
			connect(editor, &StarEditor::editFinished, this, &ListViewDelegate::commitAndCloseEditor);
			return editor;
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