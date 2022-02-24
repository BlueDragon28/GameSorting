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
    int order;
    QString name;
};

enum class ListType
{
    UNKNOWN,
    GAMELIST,
    MOVIESLIST,
    COMMONLIST,
    BOOKSLIST,
    SERIESLIST
};

enum class ViewType
{
    GAME,
    MOVIE,
    COMMON,
    BOOKS,
    SERIES,
    UTILITY,
};

struct SensitiveContent
{
    int explicitContent = 0;
    int violenceContent = 0;
    int badLanguageContent = 0;
};
Q_DECLARE_METATYPE(SensitiveContent);

struct GameItem
{
    long long int gameID;
    long long int gamePos;
    QString name;
    QString series;
    QString categories;
    QString developpers;
    QString publishers;
    QString platform;
    QString services;
    SensitiveContent sensitiveContent;
    QString url;
    int rate;
};

struct MovieItem
{
    long long int movieID;
    long long int moviePos;
    QString name;
    QString series;
    QString categories;
    QString directors;
    QString actors;
    QString productions;
    QString music;
    QString services;
    SensitiveContent sensitiveContent;
    QString url;
    int rate;
};

struct CommonItem
{
    long long int commonID;
    long long int commonPos;
    QString name;
    QString series;
    QString categories;
    QString authors;
    SensitiveContent sensitiveContent;
    QString url;
    int rate;
};

struct BooksItem
{
    long long int bookID;
    long long int bookPos;
    QString name;
    QString series;
    QString categories;
    QString authors;
    QString publishers;
    QString services;
    SensitiveContent sensitiveContent;
    QString url;
    int rate;
};

struct SeriesItem
{
    long long int serieID;
    long long int seriePos;
    QString name;
    int episodePos;
    int seasonPos;
    QString categories;
    QString directors;
    QString actors;
    QString production;
    QString music;
    QString services;
    SensitiveContent sensitiveContent;
    QString url;
    int rate;
};

enum class UtilityTableName
{
    CATEGORIES,
    DEVELOPPERS,
    PUBLISHERS,
    PLATFORM,
    SERVICES,
    SENSITIVE_CONTENT,
    DIRECTOR,
    ACTORS,
    PRODUCTION,
    MUSIC,
    AUTHORS,
    SERIES
};

namespace Game
{
    enum ColumnIndex
    {
        NAME = 0,
        SERIES = 1,
        CATEGORIES = 2,
        DEVELOPPERS = 3,
        PUBLISHERS = 4,
        PLATFORMS = 5,
        SERVICES = 6,
        SENSITIVE_CONTENT = 7,
        RATE = 8
    };

    struct SensitiveContentData
    {
        long long int explicitContent;
        long long int violenceContent;
        long long int badLanguageContent;
    };

    struct SaveUtilityData
    {
        QList<ItemUtilityData> series;
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
        QList<SaveUtilityInterfaceItem> series;
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
        int series;
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
        signed char columnSort;
        unsigned char sortOrder;
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
Q_DECLARE_METATYPE(Game::SensitiveContentData);

namespace Movie
{
    enum ColumnIndex
    {
        NAME = 0,
        SERIES = 1,
        CATEGORIES = 2,
        DIRECTORS = 3,
        ACTORS = 4,
        PRODUCTIONS = 5,
        MUSIC = 6,
        SERVICES = 7,
        SENSITIVE_CONTENT = 8,
        RATE = 9
    };

    struct SaveUtilityData
    {
        QList<ItemUtilityData> series;
        QList<ItemUtilityData> categories;
        QList<ItemUtilityData> directors;
        QList<ItemUtilityData> actors;
        QList<ItemUtilityData> productions;
        QList<ItemUtilityData> music;
        QList<ItemUtilityData> services;
    };

    struct SaveUtilityInterfaceData
    {
        QList<Game::SaveUtilityInterfaceItem> series;
        QList<Game::SaveUtilityInterfaceItem> categories;
        QList<Game::SaveUtilityInterfaceItem> directors;
        QList<Game::SaveUtilityInterfaceItem> actors;
        QList<Game::SaveUtilityInterfaceItem> productions;
        QList<Game::SaveUtilityInterfaceItem> music;
        QList<Game::SaveUtilityInterfaceItem> services;
        QList<Game::SaveUtilitySensitiveContentItem> sensitiveContent;
    };

    struct ColumnsSize
    {
        int name;
        int series;
        int categories;
        int directors;
        int actors;
        int productions;
        int music;
        int services;
        int sensitiveContent;
        int rate;
    };

    struct SaveItem
    {
        long long int movieID;
        long long int moviePos;
        QString name;
        QString url;
        int rate;
    };

    struct SaveDataTable
    {
        QString tableName;
        QList<SaveItem> movieList;
        SaveUtilityInterfaceData interface;
        ColumnsSize viewColumnsSize;
        signed char columnSort;
        unsigned char sortOrder;
    };

    struct SaveData
    {
        QList<SaveDataTable> movieTables;
        SaveUtilityData utilityData;
    };
}
Q_DECLARE_METATYPE(Movie::SaveUtilityData);
Q_DECLARE_METATYPE(Movie::SaveUtilityInterfaceData);
Q_DECLARE_METATYPE(Movie::SaveDataTable);
Q_DECLARE_METATYPE(Movie::SaveData);

namespace Common
{
    enum ColumnIndex
    {
        NAME = 0,
        SERIES = 1,
        CATEGORIES = 2,
        AUTHORS = 3,
        SENSITIVE_CONTENT = 4,
        RATE = 5
    };

    struct SaveUtilityData
    {
        QList<ItemUtilityData> series;
        QList<ItemUtilityData> categories;
        QList<ItemUtilityData> authors;
    };

    struct SaveUtilityInterfaceData
    {
        QList<Game::SaveUtilityInterfaceItem> series;
        QList<Game::SaveUtilityInterfaceItem> categories;
        QList<Game::SaveUtilityInterfaceItem> authors;
        QList<Game::SaveUtilitySensitiveContentItem> sensitiveContent;
    };

    struct ColumnsSize
    {
        int name;
        int series;
        int categories;
        int authors;
        int sensitiveContent;
        int rate;
    };

    struct SaveItem
    {
        long long int commonID;
        long long int commonPos;
        QString name;
        QString url;
        int rate;
    };

    struct SaveDataTable
    {
        QString tableName;
        QList<SaveItem> commonList;
        SaveUtilityInterfaceData interface;
        ColumnsSize viewColumnsSize;
        signed char columnSort;
        unsigned char sortOrder;
    };

    struct SaveData
    {
        QList<SaveDataTable> commonTables;
        SaveUtilityData utilityData;
    };
}

Q_DECLARE_METATYPE(Common::SaveUtilityData);
Q_DECLARE_METATYPE(Common::SaveUtilityInterfaceData);
Q_DECLARE_METATYPE(Common::SaveDataTable);
Q_DECLARE_METATYPE(Common::SaveData);

namespace Books
{
    enum ColumnIndex
    {
        NAME = 0,
        SERIES = 1,
        CATEGORIES = 2,
        AUTHORS = 3,
        PUBLISHERS = 4,
        SERVICES = 5,
        SENSITIVE_CONTENT = 6,
        RATE = 7
    };

    struct SaveUtilityData
    {
        QList<ItemUtilityData> series;
        QList<ItemUtilityData> categories;
        QList<ItemUtilityData> authors;
        QList<ItemUtilityData> publishers;
        QList<ItemUtilityData> services;
    };

    struct SaveUtilityInterfaceData
    {
        QList<Game::SaveUtilityInterfaceItem> series;
        QList<Game::SaveUtilityInterfaceItem> categories;
        QList<Game::SaveUtilityInterfaceItem> authors;
        QList<Game::SaveUtilityInterfaceItem> publishers;
        QList<Game::SaveUtilityInterfaceItem> services;
        QList<Game::SaveUtilitySensitiveContentItem> sensitiveContent;
    };

    struct ColumnsSize
    {
        int name;
        int series;
        int categories;
        int authors;
        int publishers;
        int services;
        int sensitiveContent;
        int rate;
    };

    struct SaveItem
    {
        long long int bookID;
        long long int bookPos;
        QString name;
        QString url;
        int rate;
    };

    struct SaveDataTable
    {
        QString tableName;
        QList<SaveItem> booksList;
        SaveUtilityInterfaceData interface;
        ColumnsSize viewColumnsSize;
        signed char columnSort;
        unsigned char sortOrder;
    };

    struct SaveData
    {
        QList<SaveDataTable> booksTables;
        SaveUtilityData utilityData;
    };
}
Q_DECLARE_METATYPE(Books::SaveUtilityData);
Q_DECLARE_METATYPE(Books::SaveUtilityInterfaceData);
Q_DECLARE_METATYPE(Books::SaveDataTable);
Q_DECLARE_METATYPE(Books::SaveData);

namespace Series
{
    enum ColumnIndex
    {
        NAME = 0,
        EPISODE = 1,
        SEASON = 2,
        CATEGORIES = 3,
        DIRECTORS = 4,
        ACTORS = 5,
        PRODUCTION = 6,
        MUSIC = 7,
        SERVICES = 8,
        SENSITIVE_CONTENT = 9,
        URL = 10,
        RATE = 11
    };

    struct SaveUtilityData
    {
        QList<ItemUtilityData> categories;
        QList<ItemUtilityData> directors;
        QList<ItemUtilityData> actors;
        QList<ItemUtilityData> production;
        QList<ItemUtilityData> music;
        QList<ItemUtilityData> services;
    };

    struct SaveUtilityInterfaceData
    {
        QList<Game::SaveUtilityInterfaceItem> categories;
        QList<Game::SaveUtilityInterfaceItem> directors;
        QList<Game::SaveUtilityInterfaceItem> actors;
        QList<Game::SaveUtilityInterfaceItem> production;
        QList<Game::SaveUtilityInterfaceItem> music;
        QList<Game::SaveUtilityInterfaceItem> services;
        QList<Game::SaveUtilitySensitiveContentItem> sensitiveContent;
    };

    struct ColumnsSize
    {
        int name;
        int episode;
        int season;
        int categories;
        int directors;
        int actors;
        int production;
        int music;
        int services;
        int sensitiveContent;
        int rate;
    };

    struct SaveItem
    {
        long long int serieID;
        long long int seriePos;
        QString name;
        QString url;
        QString rate;
    };

    struct SaveDataTable
    {
        QString tableName;
        QList<SaveItem> serieList;
        SaveUtilityInterfaceData interface;
        ColumnsSize viewColumnsSize;
        signed char columnSort;
        unsigned char sortOrder;
    };

    struct SaveData
    {
        QList<SaveDataTable> serieTables;
        SaveUtilityData utilityData;
    };
}
Q_DECLARE_METATYPE(Series::SaveUtilityData);
Q_DECLARE_METATYPE(Series::SaveUtilityInterfaceData);
Q_DECLARE_METATYPE(Series::SaveDataTable);
Q_DECLARE_METATYPE(Series::SaveData);

struct RecentFileData
{
    QString filePath;
    QString fileName;
};

// Struct used to specify item to filter.
// pattern and utilityList are not link, there
// are used depending on the column used.
struct ListFilter
{
    int column = -1;
    QString pattern;
    QList<long long int> utilityList;
    int rate;
};

#endif // GAMESORTING_DATASTRUCT_H_