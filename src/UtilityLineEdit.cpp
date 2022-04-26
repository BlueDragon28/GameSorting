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

#include "UtilityLineEdit.h"
#include <QCompleter>
#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QSqlError>
#include <iostream>

UtilityLineEdit::UtilityLineEdit(UtilityTableName tableName, SqlUtilityTable& utilityTable, QSqlDatabase& db, QWidget* parent) :
    QLineEdit(parent),
    m_tableName(tableName),
    m_utilityTable(utilityTable),
    m_db(db),
    m_completer(nullptr)
{
    createCompleter();
}

UtilityLineEdit::~UtilityLineEdit()
{}

QCompleter* UtilityLineEdit::completer() const
{
    return m_completer;
}

void UtilityLineEdit::setCompleter(QCompleter* completer)
{
    // Set the completer
    if (m_completer)
    {
        m_completer->disconnect(this);
        m_completer->deleteLater();
    }

    m_completer = completer;
    if (!m_completer)
        return;
    
    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_completer, QOverload<const QString&>::of(&QCompleter::activated),
        this, &UtilityLineEdit::applyCompletion);
}

void UtilityLineEdit::applyCompletion(const QString& str)
{
    // Apply completion to the view.
    if (m_completer->widget() != this)
        return;
    
    QString lineStr = text();
    int cursorPos = cursorPosition();
    int commaBegin = lineStr.mid(0, cursorPos).lastIndexOf(',');
    int commaEnd = -1;
    if (commaBegin >= 0)
        commaEnd = lineStr.mid(cursorPos).indexOf(',');
    
    lineStr.erase(
        lineStr.cbegin()+commaBegin+1,
        commaEnd >= 0 ? lineStr.cbegin()+commaEnd : lineStr.cend());
    
    QString newStrUtility = QString(" %1 ").arg(str.trimmed());
    if (commaEnd == -1)
        newStrUtility += ", ";
    lineStr.insert(commaBegin+1, newStrUtility);
    setText(lineStr);
    if (commaEnd == -1)
        setCursorPosition(lineStr.size());
    else
        setCursorPosition(commaBegin+1+lineStr.mid(commaBegin+1).indexOf(',')+1);
}

void UtilityLineEdit::focusInEvent(QFocusEvent* e)
{
    // Apply this widget to the completer when this widget get focus.
    if (m_completer)
        m_completer->setWidget(this);
    return QLineEdit::focusInEvent(e);
}

void UtilityLineEdit::keyPressEvent(QKeyEvent* e)
{
    // If completer is valid and visible, redirect some
    // action to the completer.
    if (m_completer && m_completer->popup()->isVisible())
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        }
    }

    // Do not process Ctrl+E event when there is a completer.
    const bool shortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E);
    if (!m_completer || !shortcut)
        QLineEdit::keyPressEvent(e);
    
    const bool ctrlOrShift = (e->modifiers().testFlag(Qt::ControlModifier) ||
        e->modifiers().testFlag(Qt::ShiftModifier));
    if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
        return;
    
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    // Retrieve utility text.
    int commaLastIndex = text().mid(0, cursorPosition()).lastIndexOf(',')+1;
    QString strPrefix = text().mid(
        commaLastIndex,
        cursorPosition()-commaLastIndex)
        .trimmed();
    
    // Hide completer popup is nothing is needed.
    if (!shortcut && (hasModifier || e->text().isEmpty() || strPrefix.isEmpty() || e->text().right(1) == ","))
    {
        m_completer->popup()->hide();
        return;
    }

    // Set completion prefix.
    if (strPrefix != m_completer->completionPrefix())
    {
        m_completer->setCompletionPrefix(strPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0,0));
    }
    
    // Show completion popup.
    QRect lineRect = rect();
    lineRect.setWidth(m_completer->popup()->sizeHintForColumn(0) +
        m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(lineRect);
}

void UtilityLineEdit::createCompleter()
{
    // Retrieve utility list.
    QString statement = QString(
        "SELECT\n"
        "   Name\n"
        "FROM\n"
        "   \"%1\"\n"
        "ORDER BY\n"
        "   OrderID ASC;")
            .arg(m_utilityTable.tableName(m_tableName));

#ifndef NDEBUG
    std::cout << statement.toLocal8Bit().constData() << '\n' << std::endl;
#endif

    QSqlQuery query(m_db);
    QStringList utilityList;
    if (query.exec(statement))
    {
        while(query.next())
            utilityList.append(query.value(0).toString());
    }
    else
    {
#ifndef NDEBUG
        std::cerr << QString("Failed to query utility value from table %1.\n\t%2")
            .arg(m_utilityTable.tableName(m_tableName), query.lastError().text())
            .toLocal8Bit().constData() << '\n' << std::endl;
#endif
        return;
    }

    QCompleter* completer = new QCompleter(utilityList, this);
    setCompleter(completer);
}