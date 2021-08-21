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

bool SaveInterface::saveMovies(const QString& filePath, const QVariant& variant)
{
    // Retreving the data from the QVariant.
    Movie::SaveData data = qvariant_cast<Movie::SaveData>(variant);

    // Opening the file in write mode.
    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);

        // Headers.
        // Primary identifier.
        const char primaryIdentifier[PRIMARY_IDENTIFIER_SIZE] = PRIMARY_IDENTIFIER;
        out.writeRawData(primaryIdentifier, PRIMARY_IDENTIFIER_SIZE-1);

        // Identifier
        const char* fileIdentifier = MLD_IDENTIFIER;
        out.writeRawData(fileIdentifier, 3);

        // Version
        int fileVersion = MLD_VERSION;
        out << fileVersion;

        // Writing movies data.
        out << data;

        // 0xFF at the end of the file.
        const unsigned char endCheck = 0xFF;
        out << endCheck;

        if (!file.commit())
            return false;
        return true;
    }
    else
    {
#ifndef NDEBUG
        std::cerr << "Canot save the movie into the file: " << filePath.toLocal8Bit().constData() << std::endl;
#endif
        return false;
    }
}

bool SaveInterface::openMovies(QDataStream* in, QVariant& variant)
{
    // Reading the movie list file.
    if (in->atEnd())
        return false;
    
    // Version
    int fileVersion;
    *in >> fileVersion;
    if (fileVersion >= MLD_VERSION && fileVersion < MLD_VERSION_MAX_SUPPORT)
    {
        if (in->atEnd())
            return false;
        
        // Data.
        Movie::SaveData data = {};
        *in >> data;
        if (in->atEnd())
            return false;
        
        // 0xFF at the end of the file.
        unsigned char endFile;
        *in >> endFile;
        if (endFile != 0xFF)
            return false;
        
        // Store the data into the variant.
        variant = QVariant::fromValue(data);
        return true;
    }
    else
        return false;
}

QDataStream& operator<<(QDataStream& out, const Movie::SaveUtilityData& data)
{
    // Writing the SQL Utility data to the data stream.
    long long int count = data.categories.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.categories.at(i);
    
    count = data.directors.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.directors.at(i);
    
    count = data.actors.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.actors.at(i);
    
    count = data.productions.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.productions.at(i);
    
    count = data.music.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.music.at(i);
    
    count = data.services.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.services.at(i);
    
    return out;
}

QDataStream& operator>>(QDataStream& in, Movie::SaveUtilityData& data)
{
    // Reading the SQL Utility data from the data stream.
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.categories.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.categories[i];
    }

    in >> count;
    if (count > 0)
    {
        data.directors.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.directors[i];
    }

    in >> count;
    if (count > 0)
    {
        data.actors.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.actors[i];
    }

    in >> count;
    if (count > 0)
    {
        data.productions.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.productions[i];
    }

    in >> count;
    if (count > 0)
    {
        data.music.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.music[i];
    }

    in >> count;
    if (count > 0)
    {
        data.services.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.services[i];
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const Movie::SaveUtilityInterfaceData& data)
{
    // Writing the movies list utility interface data.
    long long int count = data.categories.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.categories.at(i);
    
    count = data.directors.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.directors.at(i);
    
    count = data.actors.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.actors.at(i);
    
    count = data.productions.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.productions.at(i);
    
    count = data.music.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.music.at(i);
    
    count = data.services.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.services.at(i);

    return out;
}

QDataStream& operator>>(QDataStream& in, Movie::SaveUtilityInterfaceData& data)
{
    // Reading the movies list utility interface data.
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.categories.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.categories[i];
    }

    in >> count;
    if (count > 0)
    {
        data.directors.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.directors[i];
    }

    in >> count;
    if (count > 0)
    {
        data.actors.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.actors[i];
    }

    in >> count;
    if (count > 0)
    {
        data.productions.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.productions[i];
    }

    in >> count;
    if (count > 0)
    {
        data.music.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.music[i];
    }

    in >> count;
    if (count > 0)
    {
        data.services.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.services[i];
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const Movie::SaveData& data)
{
    // Writing the Movie::SaveData into the data stream.

    int count = data.movieTables.size();
    out << count;
    for (int i = 0; i < count; i++)
        out << data.movieTables.at(i);
    
    out << data.utilityData;

    return out;
}

QDataStream& operator>>(QDataStream& in, Movie::SaveData& data)
{
    // Reading the Movie::SaveData from the data stream.
    int count;
    in >> count;
    if (count > 0)
    {
        data.movieTables.resize(count);
        for (int i = 0; i < count; i++)
            in >> data.movieTables[i];
    }

    in >> data.utilityData;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Movie::SaveDataTable& data)
{
    // Writing the Movie::SaveDataTable into the data stream.
    out << data.tableName;
    long long int count = data.movieList.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.movieList.at(i);
    out << data.interface;
    out << data.viewColumnsSize;
    out << data.columnSort;
    out << data.sortOrder;

    return out;
}

QDataStream& operator>>(QDataStream& in, Movie::SaveDataTable& data)
{
    // Reading the Movie::SaveDataTable from the data stream.
    in >> data.tableName;
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.movieList.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.movieList[i];
    }
    in >> data.interface;
    in >> data.viewColumnsSize;
    in >> data.columnSort;
    in >> data.sortOrder;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Movie::SaveItem& data)
{
    // Writing the Movie::SaveItem into the data stream.
    out << data.movieID;
    out << data.moviePos;
    out << data.name;
    out << data.url;
    unsigned char rate = data.rate;
    out << rate;
    return out;
}

QDataStream& operator>>(QDataStream& in, Movie::SaveItem& data)
{
    // Reading the Movie::SaveItem from the data stream.
    in >> data.movieID;
    in >> data.moviePos;
    in >> data.name;
    in >> data.url;
    unsigned char rate;
    in >> rate;
    data.rate = rate;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Movie::ColumnsSize& data)
{
    // Writing the size of the columns of the table view.
    out << data.name;
    out << data.categories;
    out << data.directors;
    out << data.actors;
    out << data.productions;
    out << data.music;
    out << data.services;
    out << data.sensitiveContent;
    out << data.rate;
    return out;
}

QDataStream& operator>>(QDataStream& in, Movie::ColumnsSize& data)
{
    // Reading the size of the columns of the table view.
    in >> data.name;
    in >> data.categories;
    in >> data.directors;
    in >> data.actors;
    in >> data.productions;
    in >> data.music;
    in >> data.services;
    in >> data.sensitiveContent;
    in >> data.rate;
    return in;
}