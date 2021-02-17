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

#ifndef GAMESORTING_GAMETEXTLINEEDITOR
#define GAMESORTING_GAMETEXTLINEEDITOR

#include <QLineEdit>

class QTabBar;

class GameTextLineEditor : public QLineEdit
{
	Q_OBJECT
public:
	explicit GameTextLineEditor(int tabIndex, QTabBar* tabBar, QWidget* parent = nullptr);

signals:
	void editingCancelled();

public slots:
	void resizeToTab();

protected:
	void keyReleaseEvent(QKeyEvent* event) override;

private:
	void updateTabBarName();

	int m_tabIndex;
	QTabBar* m_tabBar;
};

#endif // GAMESORTING_GAMETEXTLINEEDITOR