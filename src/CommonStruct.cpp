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

#include "CommonStruct.h"

QDataStream& operator<<(QDataStream& out, const GameListViewSortIndicator& value)
{
	out.writeRawData(reinterpret_cast<const char*>(&value), sizeof(bool) + sizeof(quint8) * 2);
	return out;
}

QDataStream& operator>>(QDataStream& in, GameListViewSortIndicator& value)
{
	in.readRawData(reinterpret_cast<char*>(&value), sizeof(bool) + sizeof(quint8) * 2);
	return in;
}

void removeEmptyStrings(QStringList& strList)
{
	for (QStringList::reverse_iterator it = strList.rbegin(); it != strList.rend(); it++)
	{
		if ((*it).isEmpty())
			strList.erase(it.base());
	}
}