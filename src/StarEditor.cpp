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

#include "StarEditor.h"
#include "Common.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPolygonF>

QPolygonF StarEditor::starPolygonData = 
{
	QPointF(-0.294f, -0.405f) * .5f,
	QPointF(0.0f, -1.0f) * .5f,
	QPointF(0.294f, -0.405f) * .5f,
	QPointF(0.951f, -0.309f) * .5f,
	QPointF(0.476f, 0.155f) * .5f,
	QPointF(0.588f, 0.809f) * .5f,
	QPointF(0.0f, 0.5f) * .5f,
	QPointF(-0.588f, 0.809f) * .5f,
	QPointF(-0.476f, 0.155f) * .5f,
	QPointF(-0.951f, -0.309f) * .5f,
	QPointF(-0.294f, -0.405f) * .5f
};

double StarEditor::staticPaintFactor = 19.;

StarEditor::StarEditor(QWidget* parent) :
	StarEditor(5, parent)
{}

StarEditor::StarEditor(int maxStars, QWidget* parent) :
	QWidget(parent),
	m_stars(0),
	m_maxStars(maxStars),
	m_cursorXPos(0)
{
	// Enable mouse tracking to force qt to call mouseMoveEvent every time the mouse move.
	setMouseTracking(true);
}

void StarEditor::paintStars(int starNB, QPainter* painter, QRect rect, QPalette palette, bool isEditMode)
{
	// Painting the stars.
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->save();
	painter->setPen(Qt::NoPen);

	painter->setBrush((isEditMode ? palette.highlight() : palette.windowText()));
	painter->translate(rect.x() + paintFactor() * 0.55, rect.y() + (rect.height() / 2.));
	painter->scale(paintFactor(), paintFactor());

	for (int i = 0; i < starNB; i++)
	{
		painter->drawPolygon(starPolygonData, Qt::WindingFill);
		painter->translate(1, 0);
	}

	painter->restore();
}

int StarEditor::stars() const
{
	return m_stars;
}

int StarEditor::maxStars() const
{
	return m_maxStars;
}

void StarEditor::paintEvent(QPaintEvent* event)
{
	// This function is called every time Qt needs to repaint the editor.
	QPainter painter(this);
	paintStars(m_stars, &painter, rect(), palette(), true);
}

void StarEditor::mouseMoveEvent(QMouseEvent* event)
{
	// Capturing the mouse position and determine how many star if there.
	int stars = inRange(int((event->position().x() / (width() / m_maxStars)) + 1), 0, m_maxStars);

	m_stars = stars;
	update();

	QWidget::mouseMoveEvent(event);
}

void StarEditor::mousePressEvent(QMouseEvent* event)
{
	// When the user press the left mouse button it's mean he has finished editing.
	if (event->button() == Qt::LeftButton)
		emit editFinished(this);

	QWidget::mousePressEvent(event);
}

QSize StarEditor::sizeHint() const
{
	return sizeHint(m_maxStars);
}

QSize StarEditor::sizeHint(int maxStars)
{
	return QSize(paintFactor(), 1);
}

double StarEditor::paintFactor()
{
	return staticPaintFactor;
}

void StarEditor::setStars(int stars)
{
	m_stars = inRange(stars, 0, m_maxStars);
}

void StarEditor::setMaxStars(int maxStars)
{
	if (maxStars > 0)
	{
		m_maxStars = maxStars;
		m_stars = inRange(m_stars, 0, m_maxStars);
	}
}