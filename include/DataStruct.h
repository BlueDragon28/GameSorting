#ifndef GAMESORTING_DATASTRUCT_H_
#define GAMESORTING_DATASTRUCT_H_

#include <QString>

struct GameItem
{
    int gameID;
    QString name;
    int categories;
    int developpers;
    int publishers;
    int platform;
    int services;
    QString url;
    int rate;
};

#endif // GAMESORTING_DATASTRUCT_H_