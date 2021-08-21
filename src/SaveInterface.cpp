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

#include "SaveInterface.h"

#include <QString>
#include <QList>
#include <QFile>
#include <QSaveFile>
#include <QDataStream>
#include <QFileInfo>

#include <iostream>
#include <cstring>

bool SaveInterface::save(const QString& filePath, const QVariant& data)
{
    // If the file is not valid.
    if (filePath.isEmpty())
        return false;

    // Check wich type is inside the QVariant
    // If its a game, save the data has a game.
    if (data.canConvert<Game::SaveData>())
        return saveGame(filePath, data);
    // If its a movies list, save the data has a movies list.
    else if (data.canConvert<Movie::SaveData>())
        return saveMovies(filePath, data);
    
    return false;
}

bool SaveInterface::open(const QString& filePath, QVariant& data)
{
    // Opening the file
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        // Streaming from the file.
        QDataStream in(&file);

        // Read the primary identifier
        char primaryIdentifier[PRIMARY_IDENTIFIER_SIZE];
        for (int i = 0; i < PRIMARY_IDENTIFIER_SIZE; i++)
            primaryIdentifier[i] = 0x00; // Initialize to null
        in.readRawData(primaryIdentifier, PRIMARY_IDENTIFIER_SIZE-1);

        // Check if the primary identifier is a valid.
        char validPrimaryIdentifier[PRIMARY_IDENTIFIER_SIZE] = PRIMARY_IDENTIFIER;
        if (strcmp(primaryIdentifier, validPrimaryIdentifier) != 0)
            return false;

        // Reading the identifier
        char fileIdentifier[4] = {0,0,0,0};
        in.readRawData(fileIdentifier, 3);

        bool ret = false;
        // Checking if the file is a GameList file.
        if (strcmp(fileIdentifier, GLD_IDENTIFIER) == 0)
            ret = openGame(&in, data);
        // Cheking if the file is a MovieList file.
        if (strcmp(fileIdentifier, MLD_IDENTIFIER) == 0)
            ret = openMovies(&in, data);
        file.close();
        return ret;
    }
    else
        return false;
}

// Item utility data
QDataStream& operator<<(QDataStream& out, const ItemUtilityData& data)
{
    // Writing ItemUtilityData into a data stream.
    // Writing the long long int utilityID
    out << data.utilityID;
    // Writing the position of the item in the list.
    out << data.order;
    // Then, writing the QString name;
    out << data.name;

    // Returning the dataStream.
    return out;
}

QDataStream& operator>>(QDataStream& in, ItemUtilityData& data)
{
    // Reading ItemUtilityData from the data stream.
    // Reading the long long int utilityID
    in >> data.utilityID;
    // Reading the position of the item in the list.
    in >> data.order;
    // Reading the QString name.
    in >> data.name;

    return in;
}

