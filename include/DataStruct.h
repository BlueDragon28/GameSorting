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

#ifndef GAMESORTING_DATASTRUCT_H_
#define GAMESORTING_DATASTRUCT_H_

#include <QString>
#include <QList>
#include <QMetaType>

struct ItemUtilityData
{
    long long int utilityID;
    QString name;
};

enum class ListType
{
    UNKNOWN,
    GAMELIST
};

enum class ViewType
{
    GAME,
    UTILITY,
};

struct GameItem
{
    long long int gameID;
    long long int gamePos;
    QString name;
    QString categories;
    QString developpers;
    QString publishers;
    QString platform;
    QString services;
    QString sensitiveContent;
    QString url;
    int rate;
};

enum class UtilityTableName
{
    // Game
    CATEGORIES,
    DEVELOPPERS,
    PUBLISHERS,
    PLATFORM,
    SERVICES,
    SENSITIVE_CONTENT
};

namespace Game
{
    enum ColumnIndex
    {
        NAME = 0,
        CATEGORIES = 1,
        DEVELOPPERS = 2,
        PUBLISHERS = 3,
        PLATFORMS = 4,
        SERVICES = 5,
        SENSITIVE_CONTENT = 6,
        RATE = 7
    };

    struct SaveUtilityData
    {
        QList<ItemUtilityData> categories;
        QList<ItemUtilityData> developpers;
        QList<ItemUtilityData> publishers;
        QList<ItemUtilityData> platform;
        QList<ItemUtilityData> services;
    };

    struct SaveItem
    {
        long long int gameID;
        long long int gamePos;
        QString name;
        QString url;
        int rate;
    };

    struct SaveUtilityInterfaceItem
    {
        long long int gameID;
        long long int utilityID;
    };

    struct SaveUtilitySensitiveContentItem
    {
        long long int SensitiveContentID;
        long long int gameID;
        int explicitContent;
        int violenceContent;
        int badLanguageContent;
    };

    struct SaveUtilityInterfaceData
    {
        QList<SaveUtilityInterfaceItem> categories;
        QList<SaveUtilityInterfaceItem> developpers;
        QList<SaveUtilityInterfaceItem> pubishers;
        QList<SaveUtilityInterfaceItem> platform;
        QList<SaveUtilityInterfaceItem> services;
        QList<SaveUtilitySensitiveContentItem> sensitiveContent;
    };

    struct ColumnsSize
    {
        int name;
        int categories;
        int developpers;
        int publishers;
        int platform;
        int services;
        int sensitiveContent;
        int rate;
    };

    struct SaveDataTable
    {
        QString tableName;
        QList<SaveItem> gameList;
        SaveUtilityInterfaceData interface;
        ColumnsSize viewColumnsSize;
    };

    struct SaveData
    {
        QList<SaveDataTable> gameTables;
        SaveUtilityData utilityData;
    };
}
Q_DECLARE_METATYPE(Game::SaveUtilityInterfaceData);
Q_DECLARE_METATYPE(Game::SaveUtilityData);
Q_DECLARE_METATYPE(Game::SaveDataTable);
Q_DECLARE_METATYPE(Game::SaveData);

#endif // GAMESORTING_DATASTRUCT_H_