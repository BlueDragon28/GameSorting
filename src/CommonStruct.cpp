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