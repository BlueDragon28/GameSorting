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

#include "GameStarRating.h"

#include <QPainter>
#include <QColor>
#include <QLineF>

GameStarRating::GameStarRating(int starCount, int maxStarCount, QWidget* parent) :
	m_starCount(std::max(1, std::min(maxStarCount, starCount))),
	m_maxStarCount(std::max(1, maxStarCount)),
	m_paintScaleFactor(20)
{
	initStarPolygon();
}

GameStarRating::GameStarRating(const GameStarRating& gameStarRating) :
	m_starCount(gameStarRating.m_starCount),
	m_maxStarCount(gameStarRating.m_maxStarCount),
	m_paintScaleFactor(gameStarRating.m_paintScaleFactor),
	m_starPolygon(gameStarRating.m_starPolygon)
{}

int GameStarRating::starCount() const
{
	return m_starCount;
}

int GameStarRating::maxStarCount() const
{
	return m_maxStarCount;
}

void GameStarRating::setStarCount(int starCount)
{
	m_starCount = std::max(1, std::min(m_maxStarCount, starCount));
}

void GameStarRating::setMaxStarCount(int maxStarCount)
{
	m_maxStarCount = std::max(1, maxStarCount);
}

void GameStarRating::paint(QPainter* painter, const QRect& rect, const QPalette& palette, EditMode editMode) const
{
	// Painting the stars.
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->save();
	painter->setPen(Qt::NoPen);

	painter->setBrush(editMode == EditMode::Editable ? palette.highlight() : palette.windowText());
	painter->translate(rect.x() + (0.5 * m_paintScaleFactor), rect.y() + (rect.height() / 2.));
	painter->scale(m_paintScaleFactor, m_paintScaleFactor);

	for (int i = 0; i < starCount(); i++)
	{
		painter->drawPolygon(m_starPolygon, Qt::WindingFill);
		painter->translate(1, 0);
	}

	// Drawing line in editMode
	if (editMode == EditMode::Editable)
	{
		const QVector<QLine> lines =
		{
			QLine(0, 0, rect.width(), 0),
			QLine(rect.width(), 0, rect.width(), rect.height()),
			QLine(rect.width(), rect.height(), 0, rect.height()),
			QLine(0, rect.height(), 0, 0)
		};

		painter->restore();
		painter->translate(rect.x(), rect.y());
		painter->setBrush(palette.dark());
		painter->drawLines(lines);
	}

	painter->restore();
}

QSize GameStarRating::sizeHint() const
{
	// Returning the recommanded size for the stars to be nicely draw on the TableView.
	return m_paintScaleFactor * QSize(m_maxStarCount, 1);
}

void GameStarRating::initStarPolygon()
{
	if (m_starPolygon.isEmpty())
	{
		m_starPolygon
			<< QPointF(-0.294f, -0.405f) * .5f
			<< QPointF(0.0f, -1.0f) * .5f
			<< QPointF(0.294f, -0.405f) * .5f
			<< QPointF(0.951f, -0.309f) * .5f
			<< QPointF(0.476f, 0.155f) * .5f
			<< QPointF(0.588f, 0.809f) * .5f
			<< QPointF(0.0f, 0.5f) * .5f
			<< QPointF(-0.588f, 0.809f) * .5f
			<< QPointF(-0.476f, 0.155f) * .5f
			<< QPointF(-0.951f, -0.309f) * .5f
			<< QPointF(-0.294f, -0.405f) * .5f;
	}
}

bool GameStarRating::operator<(const GameStarRating& other) const
{
	return m_starCount < other.m_starCount;
}

bool GameStarRating::operator>(const GameStarRating& other) const
{
	return m_starCount > other.m_starCount;
}

bool GameStarRating::operator<=(const GameStarRating& other) const
{
	return m_starCount <= other.m_starCount;
}

bool GameStarRating::operator>=(const GameStarRating& other) const
{
	return m_starCount >= other.m_starCount;
}

QDataStream& operator<<(QDataStream& out, const GameStarRating& star)
{
	out.writeRawData(reinterpret_cast<const char*>(&star), sizeof(int) * 3);
	return out;
}

QDataStream& operator>>(QDataStream& in, GameStarRating& star)
{
	in.readRawData(reinterpret_cast<char*>(&star), sizeof(int) * 3);
	return in;
}