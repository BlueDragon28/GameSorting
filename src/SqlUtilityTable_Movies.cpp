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

void SqlUtilityTable::createMoviesTables()
{
    // Creating all the Movies utility table.
    createSeriesTable();
    createCategoriesTable();
    createDirectoryTable();
    createActorsTable();
    createProductionsTable();
    createMusicTable();
    createServicesTable();
}

void SqlUtilityTable::destroyMoviesTables()
{
    destroyTableByName(tableName(UtilityTableName::SERIES));
    destroyTableByName(tableName(UtilityTableName::CATEGORIES));
    destroyTableByName(tableName(UtilityTableName::DIRECTOR));
    destroyTableByName(tableName(UtilityTableName::ACTORS));
    destroyTableByName(tableName(UtilityTableName::PRODUCTION));
    destroyTableByName(tableName(UtilityTableName::MUSIC));
    destroyTableByName(tableName(UtilityTableName::SERVICES));
}

void SqlUtilityTable::createDirectoryTable()
{
    standardTableCreation(UtilityTableName::DIRECTOR);
}

void SqlUtilityTable::createActorsTable()
{
    standardTableCreation(UtilityTableName::ACTORS);
}

void SqlUtilityTable::createProductionsTable()
{
    standardTableCreation(UtilityTableName::PRODUCTION);
}

void SqlUtilityTable::createMusicTable()
{
    standardTableCreation(UtilityTableName::MUSIC);
}

QVariant SqlUtilityTable::queryMoviesData() const
{
    Movie::SaveUtilityData data = {};
    data.series = retrieveTableData(UtilityTableName::SERIES);
    data.categories = retrieveTableData(UtilityTableName::CATEGORIES);
    data.directors = retrieveTableData(UtilityTableName::DIRECTOR);
    data.actors = retrieveTableData(UtilityTableName::ACTORS);
    data.productions = retrieveTableData(UtilityTableName::PRODUCTION);
    data.music = retrieveTableData(UtilityTableName::MUSIC);
    data.services = retrieveTableData(UtilityTableName::SERVICES);

    return QVariant::fromValue(data);
}

bool SqlUtilityTable::setMoviesData(const QVariant& variant)
{
    Movie::SaveUtilityData data = qvariant_cast<Movie::SaveUtilityData>(variant);

    if (!setStandardData(UtilityTableName::SERIES, data.series) ||
        !setStandardData(UtilityTableName::CATEGORIES, data.categories) ||
        !setStandardData(UtilityTableName::DIRECTOR, data.directors) ||
        !setStandardData(UtilityTableName::ACTORS, data.actors) ||
        !setStandardData(UtilityTableName::PRODUCTION, data.productions) ||
        !setStandardData(UtilityTableName::MUSIC, data.music) ||
        !setStandardData(UtilityTableName::SERVICES, data.services))
        return false;
    
    return true;
}