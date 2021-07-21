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

#ifndef GAMESORTING_UTILITYSENSITIVECONTENTEDITOR_H_
#define GAMESORTING_UTILITYSENSITIVECONTENTEDITOR_H_

#include <QDialog>
#include <QSqlDatabase>

class TableModel_UtilityInterface;

class UtilitySensitiveContentEditor : public QDialog
{
    Q_OBJECT
public:
    explicit UtilitySensitiveContentEditor(
        long long int itemID,
        TableModel_UtilityInterface* dataInterface,
        QSqlDatabase& db,
        QWidget* parent = nullptr);
    virtual ~UtilitySensitiveContentEditor();

private:
    void retrieveSensitiveContentOnItem();
    void createWidgets();
    void applyChange();

    long long int m_itemID;
    TableModel_UtilityInterface* m_dataInterface;
    QSqlDatabase& m_db;
    int m_nExplicitContent,
        m_nViolenceContent,
        m_nBadLanguageContent;
};

#endif // GAMESORTING_UTILITYSENSITIVECONTENTEDITOR_H_