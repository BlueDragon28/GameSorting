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

#include "GameStarDelegate.h"
#include "GameStarRating.h"
#include "GameStarEditor.h"
#include "GameListModel.h"

#include <QPainter>

GameStarDelegate::GameStarDelegate(QWidget* parent) :
	QStyledItemDelegate(parent)
{}

void GameStarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	/*
	* If the data from the model is type GameStarRating, we paint stars.
	* Otherwise, we let the parent class do the job for the standard type.
	*/
	if (index.data().canConvert<GameStarRating>())
	{
		GameStarRating gStarRating = qvariant_cast<GameStarRating>(index.data());

		if (option.state & QStyle::State_Selected && option.state & QStyle::State_Active)
			painter->fillRect(option.rect, option.palette.highlight());
		else if (option.state & QStyle::State_Selected && !(option.state & QStyle::State_Active))
			painter->fillRect(option.rect, option.palette.midlight());

		gStarRating.paint(painter, option.rect, option.palette, GameStarRating::EditMode::ReadOnly);
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize GameStarDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	/*
	* returning the prefered size of the row.
	* Note: this function is not called automatically from the view,
	* that's why the function makeGameRatingAtSizeHint has been implemented in
	* the MainWindow class, to force the view to match the size of the custom stars.
	* */
	if (index.data().canConvert<GameStarRating>())
	{
		GameStarRating gStarRating = qvariant_cast<GameStarRating>(index.data());

		return gStarRating.sizeHint();
	}
	else
		return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* GameStarDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	/*
	* When the user whant to edit the data whe create an editor.
	* If the type is GameStarRating, we use our custom star editor.
	* Otherwise, we let the parent class do the job for us for the 
	* standard type.
	*/
	if (index.data().canConvert<GameStarRating>())
	{
		GameStarRating gStarRating = qvariant_cast<GameStarRating>(index.data());

		GameStarEditor* editor = new GameStarEditor(parent);
		editor->setGameStarRating(gStarRating);
		connect(editor, &GameStarEditor::editFinished, this, &GameStarDelegate::commitAndCloseEditor);
		return editor;
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void GameStarDelegate::commitAndCloseEditor(GameStarEditor* editor)
{
	/*
	* when the editor had finished to edit,
	* we commit the data to the model and 
	* delete the editor.
	* */
	if (editor)
	{
		emit commitData(editor);
		emit closeEditor(editor);
	}
}

void GameStarDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	// Set the editor with the model data if neccessary.
	if (index.data().canConvert<GameStarRating>())
	{
		GameStarRating gStarRating = qvariant_cast<GameStarRating>(index.data());
		GameStarEditor* gStarEditor = qobject_cast<GameStarEditor*>(editor);
		gStarEditor->setGameStarRating(gStarRating);
	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void GameStarDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	// When commitData is called, this function is called to update the model data.
	if (index.data().canConvert<GameStarRating>())
	{
		GameStarEditor* gStarEditor = qobject_cast<GameStarEditor*>(editor);
		GameListModel* gameModel = qobject_cast<GameListModel*>(model);
		gameModel->setData(index, QVariant::fromValue(gStarEditor->gameStarRating()), Qt::EditRole);
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}