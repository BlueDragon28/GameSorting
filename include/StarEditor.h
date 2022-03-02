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

#ifndef GAMESORTING_STAREDITOR_H_
#define GAMESORTING_STAREDITOR_H_

#include <QWidget>

class StarEditor : public QWidget
{
	Q_OBJECT
public:
	StarEditor(QWidget* parent = nullptr);
	StarEditor(int maxStars, QWidget* parent = nullptr);
	virtual ~StarEditor();

	static void paintStars(int starNB, QPainter* painter, QRect rect, QPalette palette, bool isEditMode = false, bool showHidenStars = false, int maxStars = 5);
	static QSize sizeHint(int maxStars);
	static double paintFactor();
	static const QPolygonF& polygonData();

	int stars() const;
	int maxStars() const;
	void setStars(int stars);
	void setMaxStars(int maxStars);

	virtual QSize sizeHint() const override;

signals:
	void editFinished(QWidget* editor);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;

private:
	int m_stars;
	int m_maxStars;
	int m_cursorXPos;

	static QPolygonF starPolygonData;
	static QList<QLineF> starLineData;
	static double staticPaintFactor;
};

#endif // GAMESORTING_STAREDITOR_H_