#include "pch.h"
#include "GameControl.h"

int CGameControl::s_nRows = GAME_ROWS;
int CGameControl::s_nCols = GAME_COLS;
int CGameControl::s_nPicNum = GAME_PIC_NUM;

CGameControl::CGameControl()
    : m_pGameMap(nullptr)
{
}

CGameControl::~CGameControl()
{
    m_gameLogic.ReleaseMap(m_pGameMap);
}

void CGameControl::StartGame()
{
    m_gameLogic.ReleaseMap(m_pGameMap);
    m_pGameMap = m_gameLogic.InitMap();
}

int CGameControl::GetElement(int nRow, int nCol) const
{
    if (m_pGameMap == nullptr || nRow < 0 || nRow >= s_nRows || nCol < 0 || nCol >= s_nCols) {
        return -1;
    }

    return m_pGameMap[nRow][nCol];
}

bool CGameControl::HasMap() const
{
    return m_pGameMap != nullptr;
}

bool CGameControl::RemovePair(const Vertex& first, const Vertex& second)
{
    if (!m_gameLogic.CanLink(m_pGameMap, first, second)) {
        return false;
    }

    m_pGameMap[first.row][first.col] = -1;
    m_pGameMap[second.row][second.col] = -1;
    return true;
}

bool CGameControl::FindHint(Vertex& first, Vertex& second) const
{
    return m_gameLogic.FindHint(m_pGameMap, first, second);
}

bool CGameControl::FindMatchForVertex(const Vertex& selected, Vertex& match) const
{
    return m_gameLogic.FindMatchForVertex(m_pGameMap, selected, match);
}

bool CGameControl::IsCleared() const
{
    if (m_pGameMap == nullptr) {
        return true;
    }

    for (int i = 0; i < s_nRows; ++i) {
        for (int j = 0; j < s_nCols; ++j) {
            if (m_pGameMap[i][j] >= 0) {
                return false;
            }
        }
    }

    return true;
}
