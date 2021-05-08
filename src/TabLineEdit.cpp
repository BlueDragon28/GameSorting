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

#include "TabLineEdit.h"
#include <QTabBar>
#include <QKeyEvent>
#include <QFocusEvent>

TabLineEdit::TabLineEdit(int tabIndex, const QString& tabName, QRect rect, QWidget* parent) :
    QLineEdit(parent),
    m_tabIndex(tabIndex),
    m_tabStartName(tabName)
{
    setGeometry(rect);
    setAlignment(Qt::AlignCenter);
    setText(tabName);
    selectAll();

    connect(this, &TabLineEdit::editingFinished, this, &TabLineEdit::applyChange);
    connect(this, &TabLineEdit::finished, this, &TabLineEdit::deleteLater);
    connect(this, &TabLineEdit::discarded, this, &TabLineEdit::deleteLater);
}

void TabLineEdit::keyPressEvent(QKeyEvent* evt)
{
    // Called when the user press a key.
    // We check if the user to discard the edition.
    if (evt->key() == Qt::Key_Escape)
        emit discarded();
    
    return QLineEdit::keyPressEvent(evt);
}

void TabLineEdit::focusOutEvent(QFocusEvent* evt)
{
    // When the line edit lost focus, emit the editingFinished signal.
    //emit editingFinished();
    return QLineEdit::focusOutEvent(evt);
}

void TabLineEdit::applyChange()
{
    // Emit the finished signal.
    if (text().isEmpty() || text().compare(m_tabStartName, Qt::CaseInsensitive) == 0)
        emit discarded();
    else
        emit finished(m_tabIndex, text());
}