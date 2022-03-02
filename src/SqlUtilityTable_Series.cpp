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

void SqlUtilityTable::createSeriesTables()
{
    // Creating all the Series utility tables.
    createCategoriesTable();
    createDirectoryTable();
    createActorsTable();
    createProductionsTable();
    createMusicTable();
    createServicesTable();
}

void SqlUtilityTable::destroySeriesTables()
{
    destroyTableByName(tableName(UtilityTableName::CATEGORIES));
    destroyTableByName(tableName(UtilityTableName::DIRECTOR));
    destroyTableByName(tableName(UtilityTableName::ACTORS));
    destroyTableByName(tableName(UtilityTableName::PRODUCTION));
    destroyTableByName(tableName(UtilityTableName::MUSIC));
    destroyTableByName(tableName(UtilityTableName::SERVICES));
}

QVariant SqlUtilityTable::querySeriesData() const
{
    Series::SaveUtilityData data = {};
    data.categories = retrieveTableData(UtilityTableName::SERIES);
    data.directors = retrieveTableData(UtilityTableName::DIRECTOR);
    data.actors = retrieveTableData(UtilityTableName::ACTORS);
    data.production = retrieveTableData(UtilityTableName::PRODUCTION);
    data.music = retrieveTableData(UtilityTableName::MUSIC);
    data.services = retrieveTableData(UtilityTableName::SERVICES);
    return QVariant::fromValue(data);
}

bool SqlUtilityTable::setSeriesData(const QVariant& variant)
{
    Series::SaveUtilityData data = qvariant_cast<Series::SaveUtilityData>(variant);

    if (!setStandardData(UtilityTableName::CATEGORIES, data.services) ||
        !setStandardData(UtilityTableName::DIRECTOR, data.directors) ||
        !setStandardData(UtilityTableName::ACTORS, data.actors) ||
        !setStandardData(UtilityTableName::PRODUCTION, data.production) ||
        !setStandardData(UtilityTableName::MUSIC, data.music) ||
        !setStandardData(UtilityTableName::SERVICES, data.services))
        return false;
    
    return true;
}