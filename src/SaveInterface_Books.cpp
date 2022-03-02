/*
* MIT Licence
*
* This file is part of the GameSorting
*
* Copyright © 2022 Erwan Saclier de la Bâtie (BlueDragon28)
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

bool SaveInterface::saveBooksList(const QString& filePath, const QVariant& variant)
{
    // Retrieving the data from the QVariant.
    Books::SaveData data = qvariant_cast<Books::SaveData>(variant);

    // Opening the file in write mode.
    QSaveFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);

        // Headers
        // Primary indentifier.
        const char primaryIdentifier[PRIMARY_IDENTIFIER_SIZE] = PRIMARY_IDENTIFIER;
        out.writeRawData(primaryIdentifier, PRIMARY_IDENTIFIER_SIZE-1);

        // Identifier
        const char* fileIdentifier = BLD_IDENTIFIER;
        out.writeRawData(fileIdentifier, 3);

        // Version
        int fileVersion = BLD_VERSION;
        out << fileVersion;

        // Writing books data.
        out << data;

        // OxFF at the end of the file.
        const unsigned char endCheck = 0xFF;
        out << endCheck;

        if (!file.commit())
            return false;
        return true;
    }
    else
    {
#ifndef NDEBUG
    std::cerr << "Cannot save the books list into the file: " << filePath.toLocal8Bit().constData() << std::endl;
#endif
        return false;
    }
}

bool SaveInterface::openBooksList(QDataStream* in, QVariant& variant)
{
    // Reading the books list file.
    if (in->atEnd())
        return false;
    
    // Version
    int fileVersion;
    *in >> fileVersion;
    if (fileVersion >= BLD_VERSION && fileVersion < BLD_VERSION_MAX_SUPPORT)
    {
        if (in->atEnd())
            return false;
        
        // Data
        Books::SaveData data = {};
        *in >> data;
        if (in->atEnd())
            return false;
        
        // 0xFF at the end of the file.
        unsigned char endCheck;
        *in >> endCheck;
        if (endCheck != 0xFF)
            return false;
        
        // Store the data into the variant.
        variant = QVariant::fromValue(data);
        return true;
    }
    else
        return false;
}

QDataStream& operator<<(QDataStream& out, const Books::SaveUtilityData& data)
{
    // Writing the SQL Utility data to the data stream.
    long long int count = data.series.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.series.at(i);

    count = data.categories.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.categories.at(i);
    
    count = data.authors.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.authors.at(i);
    
    count = data.publishers.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.publishers.at(i);
    
    count = data.services.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.services.at(i);
    
    return out;
}

QDataStream& operator>>(QDataStream& in, Books::SaveUtilityData& data)
{
    // Reading the SQL Utility data from the data stream.
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.series.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.series[i];
    }

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
        data.authors.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.authors[i];
    }

    in >> count;
    if (count > 0)
    {
        data.publishers.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.publishers[i];
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

QDataStream& operator<<(QDataStream& out, const Books::SaveUtilityInterfaceData& data)
{
    // Writing the books list utility interface data.
    long long int count = data.series.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.series.at(i);

    count = data.categories.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.categories.at(i);
    
    count = data.authors.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.authors.at(i);
    
    count = data.publishers.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.publishers.at(i);
    
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

QDataStream& operator>>(QDataStream& in, Books::SaveUtilityInterfaceData& data)
{
    // Reading the books list utility interface data.
    long long int count;
    in >> count;
    if (count > 0)
    {
        data.series.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.series[i];
    }

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
        data.authors.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.authors[i];
    }

    in >> count;
    if (count > 0)
    {
        data.publishers.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.publishers[i];
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

QDataStream& operator<<(QDataStream& out, const Books::SaveData& data)
{
    // Wrinting the Books::SaveData into the data stream.
    int count = data.booksTables.size();
    out << count;
    for (int i = 0; i < count; i++)
        out << data.booksTables.at(i);
    
    out << data.utilityData;

    return out;
}

QDataStream& operator>>(QDataStream& in, Books::SaveData& data)
{
    // Reading the Books::SaveData from the data stream.
    int count;
    in >> count;
    if (count > 0)
    {
        data.booksTables.resize(count);
        for (int i = 0; i < count; i++)
            in >> data.booksTables[i];
    }

    in >> data.utilityData;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Books::SaveDataTable& data)
{
    // Wrinting the Books::SaveDataTable into the data stream.
    out << data.tableName;
    long long int count = data.booksList.size();
    out << count;
    for (long long int i = 0; i < count; i++)
        out << data.booksList.at(i);
    out << data.interface;
    out << data.viewColumnsSize;
    out << data.columnSort;
    out << data.sortOrder;

    return out;
}

QDataStream& operator>>(QDataStream& in, Books::SaveDataTable& data)
{
    // Readig the Books::SaveDataTable from the data stream.
    in >> data.tableName;
    long long int  count;
    in >> count;
    if (count > 0)
    {
        data.booksList.resize(count);
        for (long long int i = 0; i < count; i++)
            in >> data.booksList[i];
    }
    in >> data.interface;
    in >> data.viewColumnsSize;
    in >> data.columnSort;
    in >> data.sortOrder;

    return in;
}

QDataStream& operator<<(QDataStream& out, const Books::SaveItem& data)
{
    // Writing the Books::SaveItem into the data stream.
    out << data.bookID;
    out << data.bookPos;
    out << data.name;
    out << data.url;
    unsigned char rate = data.rate;
    out << rate;
    return out;
}

QDataStream& operator>>(QDataStream& in, Books::SaveItem& data)
{
    // Reading the Books::SaveItem from the data stream.
    in >> data.bookID;
    in >> data.bookPos;
    in >> data.name;
    in >> data.url;
    unsigned char rate;
    in >> rate;
    data.rate = rate;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Books::ColumnsSize& data)
{
    // Writing the size of the columns of the table view.
    out << data.name;
    out << data.series;
    out << data.categories;
    out << data.authors;
    out << data.publishers;
    out << data.services;
    out << data.sensitiveContent;
    out << data.rate;
    return out;
}

QDataStream& operator>>(QDataStream&  in, Books::ColumnsSize& data)
{
    // Reading the size of the columns of the table view.
    in >> data.name;
    in >> data.series;
    in >> data.categories;
    in >> data.authors;
    in >> data.publishers;
    in >> data.services;
    in >> data.sensitiveContent;
    in >> data.rate;
    return in;
}