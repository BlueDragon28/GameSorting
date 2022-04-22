/*
* MIT Licence
*
* This file is part of the GameSorting
*
* Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
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

#ifndef GAMESORTING_LISTVIEWDELETAGE_H_
#define GAMESORTING_LISTVIEWDELETAGE_H_

#include <QStyledItemDelegate>
#include <QSqlDatabase>
#include "SqlUtilityTable.h"

class TableModel;
class TableModel_UtilityInterface;

class ListViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ListViewDelegate(
		TableModel* tableModel,
		SqlUtilityTable& utilityTable,
		QSqlDatabase& db,
		QObject* parent = nullptr);
	virtual ~ListViewDelegate();

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& parent) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
	void commitAndCloseEditor(QWidget* editor);
	void paintSensitiveStars(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const;
	void paintRateStars(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const;
	void applyUtilityLineEditData(long long int itemID, UtilityTableName tableName, const QString& utilityText) const;

	TableModel* m_tableModel;
	SqlUtilityTable& m_utilityTable;
	TableModel_UtilityInterface* m_utilityInterface;
	QSqlDatabase& m_db;
	bool m_legacyUtilEdit;
};

#endif // GAMESORTING_LISTVIEWDELETAGE_H_