/*
* MIT License
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

#include "Common.h"

QString removeFirstsSpace(const QString& str)
{
	QString copyStr = str;
	QChar cSpace(' ');

	while (!copyStr.isEmpty() && copyStr.at(0) == cSpace)
		copyStr.remove(0, 1);

	return copyStr;
}

QString removeLastsSpace(const QString& str)
{
	QString copyStr = str;
	QChar cSpace(' ');

	while (!copyStr.isEmpty() && copyStr.at(copyStr.size()-1) == cSpace)
		copyStr.remove(copyStr.size()-1, 1);

	return copyStr;
}

QString removeFirtsAndLastsSpace(const QString& str)
{
	return removeFirstsSpace(removeLastsSpace(str));
}

QString replaceUnderscoreBySpace(const QString& str)
{
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