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

#include "Common.h"
#include <QFileInfo>

QString removeFirstSpaces(const QString& str)
{
	// Removing the first spaces in a string.
	if (str.isEmpty()) return str;

	QString copyStr = str;
	const QChar cSpace(' ');
	int index(-1);
	int count(0);

	while (++index < copyStr.size() && copyStr.at(index) == cSpace)
		count++;

	if (count > 0)
		copyStr.remove(0, count);

	return copyStr;
}

QString removeLastSpaces(const QString& str)
{
	// Removing the last spaces in a string.
	if (str.isEmpty()) return str;

	QString copyStr = str;
	const QChar cSpace(' ');
	int index(copyStr.size());
	int from(copyStr.size());
	int count(0);

	while (--index >= 0 && copyStr.at(index) == cSpace)
	{
		count++;
		from--;
	}

	if (count > 0)
		copyStr.remove(from, count);

	return copyStr;
}

QString removeFirtAndLastSpaces(const QString& str)
{
	// Removing the first and last spaces in a string.
	return removeFirstSpaces(removeLastSpaces(str));
}

QString replaceUnderscoreBySpace(const QString& str)
{
	// Replacing all the underscores in a string by spaces.
	QString copyStr = str;
	QChar cUnderscore('_');
	QChar cSpace(' ');

	for (QString::iterator it = copyStr.begin(); it != copyStr.end(); it++)
	{
		if ((*it) == cUnderscore)
			(*it) = cSpace;
	}

	return copyStr;
}

QString replaceSpaceByUnderscore(const QString& str)
{
	// Replacing all the spaces in a string by underscores.
	QString copyStr = str;
	QChar cSpace(' ');
	QChar cUnderscore('_');

	for (QString::iterator it = copyStr.begin(); it != copyStr.end(); it++)
	{
		if ((*it) == cSpace)
			(*it) = cUnderscore;
	}

	return copyStr;
}

QString replaceMultipleSpaceByOne(const QString& str)
{
	// Removing duplicate spaces in a string.
	if (!str.isEmpty())
	{
		const QChar cSpace(' ');
		QString copyStr = str;

		for (int i = 0; i < copyStr.size(); i++)
		{
			if (copyStr.at(i) == cSpace)
			{
				int index = i;
				int from = i+1;
				int count = 0;

				while (++index < copyStr.size())
				{
					if (copyStr.at(index) == cSpace)
						count++;
					else
						break;
				}
				
				if (count > 0)
					copyStr.remove(from, count);
			}
		}

		return copyStr;
	}

	return QString();
}

template<typename T>
T inRange(T value, T min, T max)
{
	// Lock the value between min and max.
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}
template int inRange(int value, int min, int max);
template float inRange(float value, float min, float max);

RecentFileData getRecentFileData(const QString& filePath)
{
	// Return a struct with the file path and the file name of (filePath).
	RecentFileData data = {};
	data.filePath = filePath;
	data.fileName = QFileInfo(filePath).fileName();
	return data;
}