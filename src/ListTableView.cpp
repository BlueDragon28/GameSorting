#include "ListTableView.h"

#include <QItemSelectionModel>
#include <QMouseEvent>

ListTableView::ListTableView(QWidget* parent) :
	QTableView(parent)
{}

void ListTableView::mousePressEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::RightButton)
		selectionModel()->clearSelection();
	else
		QTableView::mousePressEvent(evt);
}