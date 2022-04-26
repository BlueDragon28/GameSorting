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

#ifndef GAMESORTING_UTILITYLINEEDIT_H_
#define GAMESORTING_UTILITYLINEEDIT_H_

#include "Common.h"
#include "SqlUtilityTable.h"

#include <QLineEdit>
#include <QSqlDatabase>

class UtilityLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    UtilityLineEdit(UtilityTableName tableName, SqlUtilityTable& utilityTable, QSqlDatabase& db, QWidget* parent = nullptr);
    virtual ~UtilityLineEdit();

    void setCompleter(QCompleter* completer);
    QCompleter *completer() const;

protected:
    virtual void keyPressEvent(QKeyEvent* e) override;
    virtual void focusInEvent(QFocusEvent* e) override;

private slots:
    void applyCompletion(const QString& str);

private:
    void createCompleter();

    UtilityTableName m_tableName;
    SqlUtilityTable& m_utilityTable;
    QSqlDatabase& m_db;
    QCompleter* m_completer;
};

#endif // GAMESORTING_UTILITYLINEEDIT_H_