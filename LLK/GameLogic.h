#pragma once

#include "Global.h"
#include "GameException.h"

class CGameLogic
{
public:
    int** InitMap() const;
    void ReleaseMap(int**& pGameMap) const;
    bool CanLink(int** pGameMap, const Vertex& first, const Vertex& second) const;
    bool FindHint(int** pGameMap, Vertex& first, Vertex& second) const;
    bool FindMatchForVertex(int** pGameMap, const Vertex& selected, Vertex& match) const;
};
