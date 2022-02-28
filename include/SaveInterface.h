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


#ifndef GAMESORTING_SAVEINTERFACE_H_
#define GAMESORTING_SAVEINTERFACE_H_

#include <DataStruct.h>
#include <QVariant>
#include <QString>

// The primary identifier is used to identify the file of the program.
// It is place at the beginning of the file and it is followed by the type identifier.
#define PRIMARY_IDENTIFIER \
    { 0x41, 0x56, 0x45, 0x20, 0x4D, 0x41, 0x52, 0x49, 0x41, 0x00 }
#define PRIMARY_IDENTIFIER_SIZE (int)(10)

// Type identifier and version of a GLD file.
#define GLD_IDENTIFIER "GLD"
#define GLD_LEGACY_VERSION (int)(500)
#define GLD_LEGACY_MAX_SUPPORT (int)(600)
#define GLD_VERSION (int)(600)
#define GLD_VERSION_MAX_SUPPORT (int)(700)

#define MLD_IDENTIFIER "MLD"
#define MLD_LEGACY_VERSION (int)(100)
#define MLD_LEGACY_MAX_SUPPORT (int)(600)
#define MLD_VERSION (int)(600)
#define MLD_VERSION_MAX_SUPPORT (int) (700)

#define CLD_IDENTIFIER "CLD"
#define CLD_VERSION (int)(200)
#define CLD_VERSION_MAX_SUPPORT (int)(300)

#define BLD_IDENTIFIER "BLD"
#define BLD_VERSION (int)(200)
#define BLD_VERSION_MAX_SUPPORT (int)(300)

#define SLD_IDENTIFIER "SLD"
#define SLD_VERSION (int)(100)
#define SLD_VERSION_MAX_SUPPORT (int)(200)

class QDataStream;

class SaveInterface
{
public:
    static bool save(const QString& filePath, const QVariant& data);
    static bool open(const QString& filePath, QVariant& data);

    static bool isLegacy();

private:
    static bool saveGame(const QString& filePath, const QVariant& data);
    static bool openGame(QDataStream* in, QVariant& data);

    static bool saveMovies(const QString& filePath, const QVariant& data);
    static bool openMovies(QDataStream* in, QVariant& data);

    static bool saveCommonList(const QString& filePath, const QVariant& data);
    static bool openCommonList(QDataStream* in, QVariant& data);

    static bool saveBooksList(const QString& filePath, const QVariant& data);
    static bool openBooksList(QDataStream* in, QVariant& data);

    static bool saveSeriesList(const QString& filePath, const QVariant& data);
    static bool openSeriesList(QDataStream* in, QVariant& data);

    static bool m_isLegacy;
};

// ItemUtilityData QDataStream operators
QDataStream& operator<<(QDataStream& out, const ItemUtilityData& data);
QDataStream& operator>>(QDataStream& in, ItemUtilityData& data);

// Game data QDataStream operators.
QDataStream& operator<<(QDataStream& out, const Game::SaveUtilityData& data);
QDataStream& operator>>(QDataStream& in, Game::SaveUtilityData& data);
QDataStream& operator<<(QDataStream& out, const Game::SaveUtilityInterfaceData& data);
QDataStream& operator>>(QDataStream& in, Game::SaveUtilityInterfaceData& data);
QDataStream& operator<<(QDataStream& out, const Game::SaveData& data);
QDataStream& operator>>(QDataStream& in, Game::SaveData& data);
QDataStream& operator<<(QDataStream& out, const Game::SaveDataTable& data);
QDataStream& operator>>(QDataStream& in, Game::SaveDataTable& data);
QDataStream& operator<<(QDataStream& out, const Game::SaveItem& data);
QDataStream& operator>>(QDataStream& in, Game::SaveItem& data);
QDataStream& operator<<(QDataStream& out, const Game::SaveUtilityInterfaceItem& data);
QDataStream& operator>>(QDataStream& in, Game::SaveUtilityInterfaceItem& data);
QDataStream& operator<<(QDataStream& out, const Game::SaveUtilitySensitiveContentItem& data);
QDataStream& operator>>(QDataStream& in, Game::SaveUtilitySensitiveContentItem& data);
QDataStream& operator<<(QDataStream& out, const Game::ColumnsSize& data);
QDataStream& operator>>(QDataStream& in, Game::ColumnsSize& data);

// Movie data QDataStream operators.
QDataStream& operator<<(QDataStream& out, const Movie::SaveUtilityData& data);
QDataStream& operator>>(QDataStream& in, Movie::SaveUtilityData& data);
QDataStream& operator<<(QDataStream& out, const Movie::SaveUtilityInterfaceData& data);
QDataStream& operator>>(QDataStream& in, Movie::SaveUtilityInterfaceData& data);
QDataStream& operator<<(QDataStream& out, const Movie::SaveData& data);
QDataStream& operator>>(QDataStream& in, Movie::SaveData& data);
QDataStream& operator<<(QDataStream& out, const Movie::SaveDataTable& data);
QDataStream& operator>>(QDataStream& in, Movie::SaveDataTable& data);
QDataStream& operator<<(QDataStream& out, const Movie::SaveItem& data);
QDataStream& operator>>(QDataStream& in, Movie::SaveItem& data);
QDataStream& operator<<(QDataStream& out, const Movie::ColumnsSize& data);
QDataStream& operator>>(QDataStream& in, Movie::ColumnsSize& data);

// Common data QDataStream operators.
QDataStream& operator<<(QDataStream& out, const Common::SaveUtilityData& data);
QDataStream& operator>>(QDataStream& in, Common::SaveUtilityData& data);
QDataStream& operator<<(QDataStream& out, const Common::SaveUtilityInterfaceData& data);
QDataStream& operator>>(QDataStream& in, Common::SaveUtilityInterfaceData& data);
QDataStream& operator<<(QDataStream& out, const Common::SaveData& data);
QDataStream& operator>>(QDataStream& in, Common::SaveData& data);
QDataStream& operator<<(QDataStream& out, const Common::SaveDataTable& data);
QDataStream& operator>>(QDataStream& in, Common::SaveDataTable& data);
QDataStream& operator<<(QDataStream& out, const Common::SaveItem& data);
QDataStream& operator>>(QDataStream& in, Common::SaveItem& data);
QDataStream& operator<<(QDataStream& out, const Common::ColumnsSize& data);
QDataStream& operator>>(QDataStream& in, Common::ColumnsSize& data);

// Books data QDataStream operators.
QDataStream& operator<<(QDataStream& out, const Books::SaveUtilityData& data);
QDataStream& operator>>(QDataStream& in, Books::SaveUtilityData& data);
QDataStream& operator<<(QDataStream& out, const Books::SaveUtilityInterfaceData& data);
QDataStream& operator>>(QDataStream& in, Books::SaveUtilityInterfaceData& data);
QDataStream& operator<<(QDataStream& out, const Books::SaveData& data);
QDataStream& operator>>(QDataStream& in, Books::SaveData& data);
QDataStream& operator<<(QDataStream& out, const Books::SaveDataTable& data);
QDataStream& operator>>(QDataStream& in, Books::SaveDataTable& data);
QDataStream& operator<<(QDataStream& out, const Books::SaveItem& data);
QDataStream& operator>>(QDataStream& in, Books::SaveItem& data);
QDataStream& operator<<(QDataStream& out, const Books::ColumnsSize& data);
QDataStream& operator>>(QDataStream& in, Books::ColumnsSize& data);

// Series data QDataStream operators.
QDataStream& operator<<(QDataStream& out, const Series::SaveUtilityData& data);
QDataStream& operator>>(QDataStream& in, Series::SaveDataTable& data);
QDataStream& operator<<(QDataStream& out, const Series::SaveUtilityInterfaceData& data);
QDataStream& operator>>(QDataStream& in, Series::SaveUtilityInterfaceData& data);
QDataStream& operator<<(QDataStream& out, const Series::SaveData& data);
QDataStream& operator>>(QDataStream& in, Series::SaveData& data);
QDataStream& operator<<(QDataStream& out, const Series::SaveDataTable& data);
QDataStream& operator>>(QDataStream& in, Series::SaveDataTable& data);
QDataStream& operator<<(QDataStream& out, const Series::SaveItem& data);
QDataStream& operator>>(QDataStream& in, Series::SaveItem& data);
QDataStream& operator<<(QDataStream& out, const Series::ColumnsSize& data);
QDataStream& operator>>(QDataStream& in, Series::ColumnsSize& data);

#endif // GAMESORTING_SAVEINTERFACE_H_