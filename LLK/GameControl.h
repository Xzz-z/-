#pragma once

#include "Global.h"
#include "GameLogic.h"

class CGameControl
{
public:
    static int s_nRows;
    static int s_nCols;
    static int s_nPicNum;

public:
    CGameControl();
    ~CGameControl();

    void StartGame();
    int GetElement(int nRow, int nCol) const;
    bool HasMap() const;
    bool RemovePair(const Vertex& first, const Vertex& second);
    bool FindHint(Vertex& first, Vertex& second) const;
    bool FindMatchForVertex(const Vertex& selected, Vertex& match) const;
    bool IsCleared() const;

private:
    int** m_pGameMap;
    CGameLogic m_gameLogic;
};
