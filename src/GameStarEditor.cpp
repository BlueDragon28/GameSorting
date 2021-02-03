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

#include "GameStarEditor.h"

#include <QPainter>
#include <QMouseEvent>

GameStarEditor::GameStarEditor(QWidget* parent) :
	QWidget(parent)
{
	setMouseTracking(true);
	setAutoFillBackground(true);
}

QSize GameStarEditor::sizeHint() const
{
	// Returning the recommanded size of the GameStarEditor
	// based on the recommanded size of the GameStarRating.
	return m_gStarRating.sizeHint();
}

void GameStarEditor::setGameStarRating(const GameStarRating& gStarRating)
{
	// Set the GameStarRating data to edit.
	m_gStarRating = gStarRating;
}

GameStarRating GameStarEditor::gameStarRating()
{
	// Resturning the GameStarRating data edited.
	return m_gStarRating;
}

void GameStarEditor::paintEvent(QPaintEvent* event)
{
	// Painting the stars using the paint method provided by GameStarRating.
	QPainter painter(this);
	m_gStarRating.paint(&painter, rect(), palette(), GameStarRating::EditMode::Editable);
}

void GameStarEditor::mouseMoveEvent(QMouseEvent* event)
{
	/*
	* This method is called when the mouse is moving over the widget.
	* First we get the number of stars based on the mouse position in the x axis.
	* The if statement is here to ensure that the star returned is a valid number, then
	* the method called the update method to force the widget to repaint and finally, the 
	* mouseMoveEvent from the QWidget is called to process the mouse event.
	*/
	int starNumber = getStarNumberFromCursorPosition(event->x());

	if (starNumber != m_gStarRating.starCount() && starNumber != -1)
	{
		m_gStarRating.setStarCount(starNumber);
		update();
	}
	QWidget::mouseMoveEvent(event);
}

void GameStarEditor::mouseReleaseEvent(QMouseEvent* event)
{
	// When the user have finished to edit the star numbers, he press the left mouse button,
	// this fonction is called when he release the button on the widget, it's mean he have finished
	// to edit.
	emit editFinished(this);
	QWidget::mouseReleaseEvent(event);
}

int GameStarEditor::getStarNumberFromCursorPosition(int x) const
{
	// This method return the number of star based on the cursor position in the x axis and the max star count.
	int starNumber = (x / (m_gStarRating.sizeHint().width() / m_gStarRating.maxStarCount())) + 1;

	if (starNumber <= 0 || starNumber > m_gStarRating.maxStarCount())
		return -1;

	return starNumber;
}