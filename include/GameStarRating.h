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

#ifndef GAMESORTING_GAMESTARRATING
#define GAMESORTING_GAMESTARRATING

#include <QPolygonF>
#include <QRect>
#include <QPalette>
#include <QDataStream>

class GameStarRating
{
public:
	enum class EditMode { Editable, ReadOnly };

	GameStarRating(int starCount = 1, int maxStarCount = 5, QWidget* parent = nullptr);
	GameStarRating(const GameStarRating& gameStarRating);

	void paint(QPainter* painter, const QRect& rect, const QPalette& palette, EditMode editMode) const;
	QSize sizeHint() const;
	int starCount() const;
	int maxStarCount() const;
	void setStarCount(int starCount);
	void setMaxStarCount(int maxStarCount);
	void initStarPolygon();

	bool operator<(const GameStarRating& other) const;
	bool operator>(const GameStarRating& other) const;
	bool operator<=(const GameStarRating& other) const;
	bool operator>=(const GameStarRating& other) const;

private:
	int m_starCount, m_maxStarCount, m_paintScaleFactor;
	QPolygonF m_starPolygon;
};

Q_DECLARE_METATYPE(GameStarRating)

QDataStream& operator<<(QDataStream& out, const GameStarRating& star);
QDataStream& operator>>(QDataStream& in, GameStarRating& star);

#endif // GAMESORTING_GAMESTARRATING