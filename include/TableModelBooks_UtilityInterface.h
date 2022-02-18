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

#ifndef GAMESORTING_TABLEMODELBOOKS_UTILITYINTERFACE_H_
#define GAMESORTING_TABLEMODELBOOKS_UTILITYINTERFACE_H_

#include "TableModel_UtilityInterface.h"

class TableModelBooks_UtilityInterface : public TableModel_UtilityInterface
{
    Q_OBJECT
public:
    TableModelBooks_UtilityInterface(const QString& parentTableName, QSqlDatabase& db);
    TableModelBooks_UtilityInterface(const QString& parentTableName, QSqlDatabase& db, const QVariant& variant);
    virtual ~TableModelBooks_UtilityInterface();

    virtual QString tableName(UtilityTableName tableName) const override;
    virtual void newParentName(const QString& tableName) override;
    virtual void rowRemoved(const QList<long long int>& booksIDs) override;
    virtual void updateItemUtility(long long int booksID, UtilityTableName tableName, const QVariant& data) override;
    virtual ListType listType() const override;
    virtual QVariant data() const;

protected:
    virtual bool setData(const QVariant& data) override;
    virtual void createTables() override;
    virtual void destroyTables() override;
};

#endif // GAMESORTING_TABLEMODELBOOKS_UTILITYINTERFACE_H_