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
#include "UtilitySensitiveContentEditor.h"
#include "Common.h"
#include "TableModel.h"
#include "StarEditor.h"
#include <iostream>

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QSpinBox>

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

ListViewDelegate::~ListViewDelegate()
{}

void ListViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Painting the delegate.
	if (m_tableModel->listType() == ListType::GAMELIST)
	{
		if (index.column() == Game::RATE)
		{
			// Painting the stars.
			paintRateStars(painter, option, index);
			return;
		}
		else if (index.column() == Game::SENSITIVE_CONTENT)
		{
			paintSensitiveStars(painter, option, index);
			return;
		}
	}
	else if (m_tableModel->listType() == ListType::MOVIESLIST)
	{
		if (index.column() == Movie::RATE)
		{
			// Painting the stars.
			paintRateStars(painter, option, index);
			return;
		}
		else if (index.column() == Movie::SENSITIVE_CONTENT)
		{
			paintSensitiveStars(painter, option, index);
			return;
		}
	}
	else if (m_tableModel->listType() == ListType::COMMONLIST)
	{
		if (index.column() == Common::RATE)
		{
			// Painting the stars.
			paintRateStars(painter, option, index);
			return;
		}
		else if (index.column() == Common::SENSITIVE_CONTENT)
		{
			paintSensitiveStars(painter, option, index);
			return;
		}
	}
	else if (m_tableModel->listType() == ListType::BOOKSLIST)
	{
		if (index.column() == Books::RATE)
		{
			// Painting the starts.
			paintRateStars(painter, option, index);
			return;
		}
		else if (index.column() == Books::SENSITIVE_CONTENT)
		{
			paintSensitiveStars(painter, option, index);
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
		else if (index.column() == Game::SENSITIVE_CONTENT)
		{
			return QSize((int)StarEditor::paintFactor() * 15, 1);
		}
	}
	else if (m_tableModel->listType() == ListType::MOVIESLIST)
	{
		if (index.column() == Movie::RATE)
		{
			return StarEditor::sizeHint(5);
		}
		else if (index.column() == Movie::SENSITIVE_CONTENT)
		{
			return QSize((int)StarEditor::paintFactor() * 15, 1);
		}
	}
	else if (m_tableModel->listType() == ListType::COMMONLIST)
	{
		if (index.column() == Common::RATE)
		{
			return StarEditor::sizeHint(5);
		}
		else if (index.column() == Common::SENSITIVE_CONTENT)
		{
			return QSize((int)StarEditor::paintFactor() * 15, 1);
		}
	}
	else if (m_tableModel->listType() == ListType::BOOKSLIST)
	{
		if (index.column() == Books::RATE)
		{
			return StarEditor::sizeHint(5);
		}
		else if (index.column() == Books::SENSITIVE_CONTENT)
		{
			return QSize((int)StarEditor::paintFactor() * 15, 1);
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
			if (option.rect.width() >= 5 * StarEditor::paintFactor())
			{
				StarEditor* editor = new StarEditor(parent);
				connect(editor, &StarEditor::editFinished, this, &ListViewDelegate::commitAndCloseEditor);
				return editor;
			}
			else
			{
				QSpinBox* editor = new QSpinBox(parent);
				editor->setRange(0, 5);
				editor->setSingleStep(1);
				return editor;
			}
		}
		else if (index.column() == Game::SERIES ||
				 index.column() == Game::CATEGORIES ||
				 index.column() == Game::DEVELOPPERS ||
				 index.column() == Game::PUBLISHERS ||
				 index.column() == Game::PLATFORMS ||
				 index.column() == Game::SERVICES)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilityTableName tableName;

			if (index.column() == Game::SERIES)
				tableName = UtilityTableName::SERIES;
			else if (index.column() == Game::CATEGORIES)
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
		else if (index.column() == Game::SENSITIVE_CONTENT)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilitySensitiveContentEditor* editor =
				new UtilitySensitiveContentEditor(
					itemID,
					m_utilityInterface,
					m_db,
					parent);
			editor->raise();
			editor->activateWindow();
			editor->show();
			return nullptr;
		}
	}
	else if (m_tableModel->listType() == ListType::MOVIESLIST)
	{
		if (index.column() == Movie::RATE)
		{
			// Creating the editor for the edition of the rate column.
			if (option.rect.width() >= 5 * StarEditor::paintFactor())
			{
				StarEditor* editor = new StarEditor(parent);
				connect(editor, &StarEditor::editFinished, this, &ListViewDelegate::commitAndCloseEditor);
				return editor;
			}
			else
			{
				QSpinBox* editor = new QSpinBox(parent);
				editor->setRange(0, 5);
				editor->setSingleStep(1);
				return editor;
			}
		}
		else if (index.column() >= Movie::SERIES &&
				 index.column() <= Movie::SERVICES)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilityTableName tableName;

			if (index.column() == Movie::SERIES)
				tableName = UtilityTableName::SERIES;
			else if (index.column() == Movie::CATEGORIES)
				tableName = UtilityTableName::CATEGORIES;
			else if (index.column() == Movie::DIRECTORS)
				tableName = UtilityTableName::DIRECTOR;
			else if (index.column() == Movie::ACTORS)
				tableName = UtilityTableName::ACTORS;
			else if (index.column() == Movie::PRODUCTIONS)
				tableName = UtilityTableName::PRODUCTION;
			else if (index.column() == Movie::MUSIC)
				tableName = UtilityTableName::MUSIC;
			else if (index.column() == Movie::SERVICES)
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
		else if (index.column() == Movie::SENSITIVE_CONTENT)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;

			UtilitySensitiveContentEditor* editor =
				new UtilitySensitiveContentEditor(
					itemID,
					m_utilityInterface,
					m_db,
					parent);
			editor->raise();
			editor->activateWindow();
			editor->show();
			return nullptr;
		}
	}
	else if (m_tableModel->listType() == ListType::COMMONLIST)
	{
		if (index.column() == Common::RATE)
		{
			// Creating the editor for the edition of the rate column.
			if (option.rect.width() >= 5 * StarEditor::paintFactor())
			{
				StarEditor* editor = new StarEditor(parent);
				connect(editor, &StarEditor::editFinished, this, &ListViewDelegate::commitAndCloseEditor);
				return editor;
			}
			else
			{
				QSpinBox* editor = new QSpinBox(parent);
				editor->setRange(0, 5);
				editor->setSingleStep(1);
				return editor;
			}
		}
		else if (index.column() == Common::CATEGORIES ||
				 index.column() == Common::AUTHORS)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilityTableName tableName;

			if (index.column() == Common::CATEGORIES)
				tableName = UtilityTableName::CATEGORIES;
			else if (index.column() == Common::AUTHORS)
				tableName = UtilityTableName::AUTHORS;
			
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
		else if (index.column() == Common::SENSITIVE_CONTENT)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilitySensitiveContentEditor* editor =
				new UtilitySensitiveContentEditor(
					itemID,
					m_utilityInterface,
					m_db,
					parent);
			editor->raise();
			editor->activateWindow();
			editor->show();
			return nullptr;
		}
	}
	else if (m_tableModel->listType() == ListType::BOOKSLIST)
	{
		if (index.column() == Books::RATE)
		{
			// Creating the editor for the edition of the rate column.
			if (option.rect.width() >= 5 * StarEditor::paintFactor())
			{
				StarEditor* editor = new StarEditor(parent);
				connect(editor, &StarEditor::editFinished, this, &ListViewDelegate::commitAndCloseEditor);
				return editor;
			}
			else
			{
				QSpinBox* editor = new QSpinBox(parent);
				editor->setRange(0, 5);
				editor->setSingleStep(1);
				return editor;
			}
		}
		else if (index.column() == Books::CATEGORIES ||
				 index.column() == Books::AUTHORS ||
				 index.column() == Books::PUBLISHERS ||
				 index.column() == Books::SERVICES)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilityTableName tableName;

			if (index.column() == Books::CATEGORIES)
				tableName = UtilityTableName::CATEGORIES;
			else if (index.column() == Books::AUTHORS)
				tableName = UtilityTableName::AUTHORS;
			else if (index.column() == Books::PUBLISHERS)
				tableName = UtilityTableName::PUBLISHERS;
			else if (index.column() == Books::SERVICES)
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
		else if (index.column() == Books::SENSITIVE_CONTENT)
		{
			long long int itemID = m_tableModel->itemID(index);

			if (itemID <= 0)
				return nullptr;
			
			UtilitySensitiveContentEditor* editor =
				new UtilitySensitiveContentEditor(
					itemID, m_utilityInterface,
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
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				starEditor->setStars(index.data().toInt());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				spinEditor->setValue(index.data().toInt());
				return;
			}
		}
	}
	else if (model->listType() == ListType::MOVIESLIST)
	{
		if (index.column() == Movie::RATE)
		{
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				starEditor->setStars(index.data().toInt());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				spinEditor->setValue(index.data().toInt());
				return;
			}
		}
	}
	else if (model->listType() == ListType::COMMONLIST)
	{
		if (index.column() == Common::RATE)
		{
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				starEditor->setStars(index.data().toInt());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				spinEditor->setValue(index.data().toInt());
				return;
			}
		}
	}
	else if (model->listType() == ListType::BOOKSLIST)
	{
		if (index.column() == Books::RATE)
		{
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				starEditor->setStars(index.data().toInt());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				spinEditor->setValue(index.data().toInt());
				return;
			}
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
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				model->setData(index, starEditor->stars());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				model->setData(index, spinEditor->value());
				return;
			}
		}
	}
	else if (model->listType() == ListType::MOVIESLIST)
	{
		if (index.column() == Movie::RATE)
		{
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				model->setData(index, starEditor->stars());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				model->setData(index, spinEditor->value());
				return;
			}
		}
	}
	else if (model->listType() == ListType::COMMONLIST)
	{
		if (index.column() == Common::RATE)
		{
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				model->setData(index, starEditor->stars());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				model->setData(index, spinEditor->value());
				return;
			}
		}
	}
	else if (model->listType() == ListType::BOOKSLIST)
	{
		if (index.column() == Books::RATE)
		{
			StarEditor* starEditor = dynamic_cast<StarEditor*>(e);
			if (starEditor)
			{
				model->setData(index, starEditor->stars());
				return;
			}
			QSpinBox* spinEditor = dynamic_cast<QSpinBox*>(e);
			if (spinEditor)
			{
				model->setData(index, spinEditor->value());
				return;
			}
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

void ListViewDelegate::paintSensitiveStars(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
{
	// Paint the stars of the three categories of sensitive content items.
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->save();

	if (options.rect.width() >= 15 * StarEditor::paintFactor())
	{
		QPolygonF starPolygons = StarEditor::polygonData();

		// Sensitive Content
		painter->setPen(Qt::NoPen);
		painter->setBrush(QBrush(QColor(255, 0, 0)));
		QRect rect = options.rect;
		painter->translate(rect.x() + StarEditor::paintFactor() * 0.55, rect.y() + (rect.height() / 2.));
		painter->scale(StarEditor::paintFactor(), StarEditor::paintFactor());

		for (int i = 0; i < 3; i++)
		{
			QColor color;
			if (i == 0)
				color = QColor(255, 0, 0);
			else if (i == 1)
				color = QColor(0, 255, 0);
			else
				color = QColor(0, 0, 255);
			
			painter->setBrush(QBrush(color));

			int numStars;
			if (i == 0)
				numStars = qvariant_cast<SensitiveContent>(index.data()).explicitContent;
			else if (i == 1)
				numStars = qvariant_cast<SensitiveContent>(index.data()).violenceContent;
			else if (i == 2)
				numStars = qvariant_cast<SensitiveContent>(index.data()).badLanguageContent;

			for (int j = 0; j < 5; j++)
			{
				if (j < numStars)
					painter->drawPolygon(starPolygons, Qt::WindingFill);
				painter->translate(1, 0);
			}
		}
	}
	else
	{
		QString sensText = QString("%1 %2 %3")
			.arg(qvariant_cast<SensitiveContent>(index.data()).explicitContent)
			.arg(qvariant_cast<SensitiveContent>(index.data()).violenceContent)
			.arg(qvariant_cast<SensitiveContent>(index.data()).badLanguageContent);
		
		QFont font = painter->font();
		font.setPixelSize(20);
		painter->setFont(font);

		painter->setPen(Qt::SolidLine);
		painter->drawText(options.rect, sensText);
	}

	painter->restore();
}

void ListViewDelegate::paintRateStars(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
{
	if (options.rect.width() >= 5 * StarEditor::paintFactor())
		StarEditor::paintStars(index.data().toInt(), painter, options.rect, options.palette);
	else
	{
		QString starNB = QString::number(index.data().toInt());

		QFont font = painter->font();
		font.setPixelSize(20);
		painter->setFont(font);

		painter->setPen(Qt::SolidLine);
		painter->drawText(options.rect, starNB);
	}
}