#ifndef GAMESORTING_DATASTRUCT_H_
#define GAMESORTING_DATASTRUCT_H_

#include <QString>

namespace GameSorting
{
    enum ListType
    {
        GAMELIST
    };
}

struct GameItem
{
    int gameID;
    int gamePos;
    QString name;
    int categories;
    int developpers;
    int publishers;
    int platform;
    int services;
    int sensitiveContent;
    QString url;
    int rate;
};

#endif // GAMESORTING_DATASTRUCT_H_