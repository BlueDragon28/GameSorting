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
#define GLD_VERSION (int)(400)

class QDataStream;

class SaveInterface
{
public:
    static bool save(const QString& filePath, const QVariant& data);
    static bool open(const QString& filePath, QVariant& data);

private:
    static bool saveGame(const QString& filePath, const QVariant& data);
    static bool openGame(QDataStream* in, QVariant& data);
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

#endif // GAMESORTING_SAVEINTERFACE_H_