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

namespace Game
{
    enum GameColumnIndex
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
}

enum class ListType
{
    GAMELIST
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
    int sensitiveContent;
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

#endif // GAMESORTING_DATASTRUCT_H_