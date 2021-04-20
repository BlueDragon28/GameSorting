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

bool SaveInterface::saveGame(const QString& filePath, const QVariant& variant)
{
    // Retrieving the data from the QVariant.
    Game::SaveData data = qvariant_cast<Game::SaveData>(variant);

    // Opening the file in write mode.
    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        // Streaming into the file.
        QDataStream out(&file);

        // Writing the headers.
        // Writing the identifier
        const char* fileIdentifier = GLD_IDENTIFIER;
        out.writeRawData(fileIdentifier, 3);

        // Writing the version of the file.
        int fileVersion = GLD_VERSION;
        out << fileVersion;

        // Writing the data of the game list.
        out << data;

        // The last things, writing at the and of a file a unsigned char of value 0xFF (255),
        // it's used to check if the file has been loaded correctly.
        const unsigned char endCheck = 0xFF;
        out << endCheck;

        if (!file.commit())
            return false;
        return true;
    }
    else
    {
#ifndef NDEBUG
        std::cerr << "Cannot save the game into the file: " << filePath.toLocal8Bit().constData() << std::endl;
#endif
        return false;
    }
}

bool SaveInterface::openGame(QDataStream* in, QVariant& variant)
{
    // Reading the game list file.
    if (in->atEnd())
        return false;
    // Reading the version of the file
    int fileVersion;
    *in >> fileVersion;
    // Checking if the version of the file is a valid version.
    if (fileVersion >= GLD_VERSION && fileVersion < GLD_VERSION+100)
    {
        if (in->atEnd())
            return false;
        // Reading the data.
        Game::SaveData data = {};
        *in >> data;
        if (in->atEnd())
            return false;

        // Retrieve the unsigned char and check if is value is equal to 0x55 (255).
        // If it's not true, it's mean the file is not loaded correctly.
        unsigned char endFile;
        *in >> endFile;
        if (endFile != 0xFF)
            return false;
        
        // Set the data into the variant.
        variant = QVariant::fromValue(data);
        return true;
    }
    else
        return false;
}

// Game data.
QDataStream& operator<<(QDataStream& out, const Game::SaveUtilityData& data)
{
    // Writing the Item SQL Utility data.
    // Categories
    // Writing the number of rows.
    long long int count = data.categories.size();
    out << count;
    // Then writing each row.
    for (long long int i = 0; i < count; i++)
        out << data.categories.at(i);
    
    // Developpers
    count = data.developpers.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.developpers.at(i);
    
    // Publishers
    count = data.publishers.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.publishers.at(i);
    
    // Platform
    count = data.platform.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.publishers.at(i);
    
    // Services
    count = data.services.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.services.at(i);
    
    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveUtilityData& data)
{
    // Reading the Item SQL Utility data from the data stream.
    // Categories
    // Reading the number of rows.
    long long int count;
    in >> count;
    if (count > 0)
    {
        // Then, reading each row.
        data.categories.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.categories[i];
    }

    // Developpers
    in >> count;
    if (count > 0)
    {
        data.developpers.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.developpers[i];
    }

    // Publishers
    in >> count;
    if (count > 0)
    {
        data.publishers.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.publishers[i];
    }

    // Platform
    in >> count;
    if (count > 0)
    {
        data.platform.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.platform[i];
    }

    // Services
    in >> count;
    if (count > 0)
    {
        data.services.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.services[i];
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const Game::SaveUtilityInterfaceData& data)
{
    // Writing the game list utility interface data.
    // Categories
    long long int count = data.categories.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.categories.at(i);
    
    // Developpers
    count = data.developpers.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.developpers.at(i);
    
    // Publishers
    count = data.pubishers.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.pubishers.at(i);
    
    // Platform
    count = data.platform.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.platform.at(i);
    
    // Services
    count = data.services.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.services.at(i);
    
    // Last one, the sensitive content list.
    count = data.sensitiveContent.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.sensitiveContent.at(i);
    
    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveUtilityInterfaceData& data)
{
    // Reading the game list utility interface data from the data stream.
    // Categories
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.categories.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.categories[i];
    }

    // Developpers
    in >> count;
    if (count > 0)
    {
        data.developpers.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.developpers[i];
    }

    // Publishers
    in >> count;
    if (count > 0)
    {
        data.pubishers.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.pubishers[i];
    }

    // Platform
    in >> count;
    if (count > 0)
    {
        data.platform.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.platform[i];
    }

    // Services
    in >> count;
    if (count > 0)
    {
        data.services.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.services[i];
    }

    // Sensitive content
    in >> count;
    if (count > 0)
    {
        data.sensitiveContent.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.sensitiveContent[i];
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const Game::SaveData& data)
{
    // Writing Game::SaveData into a data stream.

    // Writing the games tables.
    // Storing the number of tables.
    int count = data.gameTables.size();
    out << count;
    for (int i = 0; i < count; i++)
        out << data.gameTables.at(i);
    
    // Writing the Item SQL Utility data.
    out << data.utilityData;

    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveData& data)
{
    // Reading Game::SaveData from the data stream.

    // Reading the games tables.
    // Retrieve the number of tables.
    int count;
    in >> count;
    if (count > 0)
    {
        data.gameTables.resize(count);
        for (int i = 0; i < count; i++)
            in >> data.gameTables[i];
    }

    // Reading the Item SQL Utility data.
    in >> data.utilityData;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Game::SaveDataTable& data)
{
    // Writing the Game::SaveDataTable into a data stream.
    // First, writing the table name
    out << data.tableName;
    // Writing the games item.
    // Writing the number of games.
    long long int count = data.gameList.size();
    out << count;

    // Then, the data.
    for (long long int i = 0; i < count; i++)
        out << data.gameList.at(i);
    
    // Writing the utility interface data.
    out << data.interface;

    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveDataTable& data)
{
    // Reading the Game::SaveDataTable from the data stream.
    // Reading the table name;
    in >> data.tableName;
    // Reading the games item.
    // Reading the number of games.
    long long int count;
    in >> count;

    // Reading the data.
    if (count > 0)
    {
        data.gameList.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.gameList[i];
    }

    // Reading the utility interface data.
    in >> data.interface;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Game::SaveItem& data)
{
    // Writing Game::SaveItem into a data stream.
    out << data.gameID;
    out << data.gamePos;
    out << data.name;
    out << data.url;
    unsigned char rate = data.rate;
    out << rate;
    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveItem& data)
{
    // Reading Game::SaveItem form the data stream.
    in >> data.gameID;
    in >> data.gamePos;
    in >> data.name;
    in >> data.url;
    unsigned char rate;
    in >> rate;
    data.rate = rate;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Game::SaveUtilityInterfaceItem& data)
{
    // Write Game::SaveUtilityInterfaceItem into a data stream.
    out << data.gameID;
    out << data.utilityID;
    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveUtilityInterfaceItem& data)
{
    // Reading the Game::SaveUtilityInterfaceItem from the data stream.
    in >> data.gameID;
    in >> data.utilityID;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Game::SaveUtilitySensitiveContentItem& data)
{
    // Write Game::SaveUtilitySensitiveContentItem into a data stream.
    out << data.SensitiveContentID;
    out << data.gameID;
    out << data.explicitContent;
    out << data.violenceContent;
    out << data.badLanguageContent;
    return out;
}

QDataStream& operator>>(QDataStream& in, Game::SaveUtilitySensitiveContentItem& data)
{
    // Reading Game::SaveUtilitySensitiveContentItem form the data stream.
    in >> data.SensitiveContentID;
    in >> data.gameID;
    in >> data.explicitContent;
    in >> data.violenceContent;
    in >> data.badLanguageContent;
    return in;
}