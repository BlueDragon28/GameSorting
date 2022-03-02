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

#include "SqlUtilityTable.h"
#include <iostream>
#include <QSqlError>

void SqlUtilityTable::createBooksTables()
{
    // Creating all the Common utility table.
    createSeriesTable();
    createCategoriesTable();
    createAuthorsTables();
    createPublishersTable();
    createServicesTable();
}

void SqlUtilityTable::destroyBooksTables()
{
    destroyTableByName(tableName(UtilityTableName::SERIES));
    destroyTableByName(tableName(UtilityTableName::CATEGORIES));
    destroyTableByName(tableName(UtilityTableName::AUTHORS));
    destroyTableByName(tableName(UtilityTableName::PUBLISHERS));
    destroyTableByName(tableName(UtilityTableName::SERVICES));
}

QVariant SqlUtilityTable::queryBooksData() const
{
    Books::SaveUtilityData data = {};
    data.series = retrieveTableData(UtilityTableName::SERIES);
    data.categories = retrieveTableData(UtilityTableName::CATEGORIES);
    data.authors = retrieveTableData(UtilityTableName::AUTHORS);
    data.publishers = retrieveTableData(UtilityTableName::PUBLISHERS);
    data.services = retrieveTableData(UtilityTableName::SERVICES);
    return QVariant::fromValue(data);
}

bool SqlUtilityTable::setBooksData(const QVariant& variant)
{
    Books::SaveUtilityData data = qvariant_cast<Books::SaveUtilityData>(variant);

    if (!setStandardData(UtilityTableName::SERIES, data.series) ||
        !setStandardData(UtilityTableName::CATEGORIES, data.categories) ||
        !setStandardData(UtilityTableName::AUTHORS, data.authors) ||
        !setStandardData(UtilityTableName::PUBLISHERS, data.publishers) ||
        !setStandardData(UtilityTableName::SERVICES, data.services))
        return false;
    
    return true;
}