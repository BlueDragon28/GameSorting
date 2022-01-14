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

#include "SqlUtilityTable.h"
#include <iostream>
#include <QSqlError>

void SqlUtilityTable::createCommonTables()
{
    // Creating all the Common utility table.
    createCategoriesTable();
    createAuthorsTables();
}

void SqlUtilityTable::destroyCommonTables()
{
    destroyTableByName(tableName(UtilityTableName::CATEGORIES));
    destroyTableByName(tableName(UtilityTableName::AUTHORS));
}

void SqlUtilityTable::createAuthorsTables()
{
    standardTableCreation(UtilityTableName::AUTHORS);
}

QVariant SqlUtilityTable::queryCommonsData() const
{
    Common::SaveUtilityData data = {};
    data.categories = retrieveTableData(UtilityTableName::CATEGORIES);
    data.authors = retrieveTableData(UtilityTableName::AUTHORS);
    return QVariant::fromValue(data);
}

bool SqlUtilityTable::setCommonsData(const QVariant& variant)
{
    Common::SaveUtilityData data = qvariant_cast<Common::SaveUtilityData>(variant);

    if (!setStandardData(UtilityTableName::CATEGORIES, data.categories) ||
        !setStandardData(UtilityTableName::AUTHORS, data.authors))
        return false;
    
    return true;
}