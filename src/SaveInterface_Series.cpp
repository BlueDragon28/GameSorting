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

bool SaveInterface::saveSeriesList(const QString& filePath, const QVariant& variant)
{
    // Retrieving the data from the QVariant.
    Series::SaveData data = qvariant_cast<Series::SaveData>(variant);

    // Opening the file in write mode.
    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);

        // Headers
        // Primary identifier.
        const char primaryIdentifier[PRIMARY_IDENTIFIER_SIZE] = PRIMARY_IDENTIFIER;
        out.writeRawData(primaryIdentifier, PRIMARY_IDENTIFIER_SIZE-1);

        // Identifier
        const char* fileIdentifier = SLD_IDENTIFIER;
        out.writeRawData(fileIdentifier, 3);

        // Version
        int fileVersion = SLD_VERSION;
        out << fileVersion;

        // Writing series data.
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
        std::cerr << "Cannot save the series list into the file: " << filePath.toLocal8Bit().constData() << std::endl;
#endif
        return false;
    }
}

bool SaveInterface::openSeriesList(QDataStream* in, QVariant& variant)
{
    // Reading the series list file.
    if (in->atEnd())
        return false;
    
    // Version
    int fileVersion;
    *in >> fileVersion;
    if (fileVersion >= SLD_VERSION && fileVersion < SLD_VERSION_MAX_SUPPORT)
    {
        if (in->atEnd())
            return false;

        // Data
        Series::SaveData data = {};
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

QDataStream& operator<<(QDataStream& out, const Series::SaveUtilityData& data)
{
    // Wrinting the SQL Utility data to the data stream.
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
    
    count = data.production.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.production.at(i);
    
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

QDataStream& operator>>(QDataStream& in, Series::SaveUtilityData& data)
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
        data.production.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.production[i];
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

QDataStream& operator<<(QDataStream& out, const Series::SaveUtilityInterfaceData& data)
{
    // Writing the series list utility interface data.
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
    
    count = data.production.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.production.at(i);
    
    count = data.music.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.music.at(i);
    
    count = data.services.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.services.at(i);

    count = data.sensitiveContent.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.sensitiveContent.at(i);
    
    return out;
}

QDataStream& operator>>(QDataStream& in, Series::SaveUtilityInterfaceData& data)
{
    // Reading the series list utility interface data.
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
        data.production.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.production[i];
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

    in >> count;
    if (count > 0)
    {
        data.sensitiveContent.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.sensitiveContent[i];
    }

    return in;
}

QDataStream& operator<<(QDataStream& out, const Series::SaveData& data)
{
    // Writing the Series::SaveData into the data stream.
    int count = data.serieTables.size();
    out << count;
    for (int i = 0; i < count; i++)
        out << data.serieTables.at(i);
    
    out << data.utilityData;

    return out;
}

QDataStream& operator>>(QDataStream& in, Series::SaveData& data)
{
    // Reading the Series::SaveData from the data stream.
    int count;
    in >> count;
    if (count > 0)
    {
        data.serieTables.resize(count);
        for (int i = 0; i < count; i++)
            in >> data.serieTables[i];
    }

    in >> data.utilityData;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Series::SaveDataTable& data)
{
    // Writing the Series::SaveDataTable into the data  stream.
    out << data.tableName;
    long long int count = data.serieList.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.serieList.at(i);
    out << data.interface;
    out << data.viewColumnsSize;
    out << data.columnSort;
    out << data.sortOrder;

    return out;
}

QDataStream& operator>>(QDataStream& in, Series::SaveDataTable& data)
{
    // Reading the Series::SaveDataTable drom the data stream.
    in >> data.tableName;
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.serieList.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.serieList[i];
    }
    in >> data.interface;
    in >> data.viewColumnsSize;
    in >> data.columnSort;
    in >> data.sortOrder;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Series::SaveItem& data)
{
    // Writing the Series::SaveItem into the data stream.
    out << data.serieID;
    out << data.seriePos;
    out << data.episodePos;
    out << data.seasonPos;
    out << data.name;
    out << data.url;
    unsigned char rate = data.rate;
    out << rate;
    return out;
}

QDataStream& operator>>(QDataStream& in, Series::SaveItem& data)
{
    // Reading the Series::SaveItem from the data stream.
    in >> data.serieID;
    in >> data.seriePos;
    in >> data.episodePos;
    in >> data.seasonPos;
    in >> data.name;
    in >> data.url;
    unsigned char rate;
    in >> rate;
    data.rate = rate;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Series::ColumnsSize& data)
{
    // Writing the size of the columns of the table view.
    out << data.name;
    out << data.episode;
    out << data.season;
    out << data.categories;
    out << data.directors;
    out << data.actors;
    out << data.production;
    out << data.music;
    out << data.services;
    out << data.sensitiveContent;
    out << data.rate;
    return out;
}

QDataStream& operator>>(QDataStream& in, Series::ColumnsSize& data)
{
    // Reading the size of the columns of the table view.
    in >> data.name;
    in >> data.episode;
    in >> data.season;
    in >> data.categories;
    in >> data.directors;
    in >> data.actors;
    in >> data.production;
    in >> data.music;
    in >> data.services;
    in >> data.sensitiveContent;
    in >> data.rate;
    return in;
}