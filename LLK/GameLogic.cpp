#include "pch.h"
#include "GameLogic.h"

#include "GameControl.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <ctime>
#include <vector>

namespace
{
constexpr int kMaxTurns = 2;
constexpr std::array<int, 4> kRowOffset = { -1, 1, 0, 0 };
constexpr std::array<int, 4> kColOffset = { 0, 0, -1, 1 };

struct SearchState
{
    int row;
    int col;
    int direction;
    int turns;
};

}

int** CGameLogic::InitMap() const
{
    const int nRows = CGameControl::s_nRows;
    const int nCols = CGameControl::s_nCols;
    const int nPicNum = CGameControl::s_nPicNum;

    if ((nRows * nCols) % (nPicNum * 2) != 0) {
        throw CGameException(_T("游戏花色与游戏地图大小不匹配！"));
    }

    int** pGameMap = nullptr;

    try {
        pGameMap = new int* [nRows];
        for (int i = 0; i < nRows; ++i) {
            pGameMap[i] = nullptr;
        }

        for (int i = 0; i < nRows; ++i) {
            pGameMap[i] = new int[nCols];
            memset(pGameMap[i], 0, sizeof(int) * nCols);
        }
    }
    catch (...) {
        ReleaseMap(pGameMap);
        throw CGameException(_T("内存操作异常！"));
    }

    const int nRepeatNum = nRows * nCols / nPicNum;
    int nCount = 0;
    for (int i = 0; i < nPicNum; ++i) {
        for (int j = 0; j < nRepeatNum; ++j) {
            pGameMap[nCount / nCols][nCount % nCols] = i;
            ++nCount;
        }
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const int nVertexNum = nRows * nCols;
    for (int i = 0; i < nVertexNum; ++i) {
        const int nIndex1 = std::rand() % nVertexNum;
        const int nIndex2 = std::rand() % nVertexNum;

        const int nTmp = pGameMap[nIndex1 / nCols][nIndex1 % nCols];
        pGameMap[nIndex1 / nCols][nIndex1 % nCols] = pGameMap[nIndex2 / nCols][nIndex2 % nCols];
        pGameMap[nIndex2 / nCols][nIndex2 % nCols] = nTmp;
    }

    return pGameMap;
}

void CGameLogic::ReleaseMap(int**& pGameMap) const
{
    if (pGameMap == nullptr) {
        return;
    }

    for (int i = 0; i < CGameControl::s_nRows; ++i) {
        delete[] pGameMap[i];
        pGameMap[i] = nullptr;
    }

    delete[] pGameMap;
    pGameMap = nullptr;
}

bool CGameLogic::CanLink(int** pGameMap, const Vertex& first, const Vertex& second) const
{
    if (pGameMap == nullptr) {
        return false;
    }

    if (first.row == second.row && first.col == second.col) {
        return false;
    }

    if (first.row < 0 || first.row >= CGameControl::s_nRows ||
        first.col < 0 || first.col >= CGameControl::s_nCols ||
        second.row < 0 || second.row >= CGameControl::s_nRows ||
        second.col < 0 || second.col >= CGameControl::s_nCols) {
        return false;
    }

    const int nFirstKind = pGameMap[first.row][first.col];
    const int nSecondKind = pGameMap[second.row][second.col];
    if (nFirstKind < 0 || nSecondKind < 0 || nFirstKind != nSecondKind) {
        return false;
    }

    const int nRows = CGameControl::s_nRows;
    const int nCols = CGameControl::s_nCols;
    const int nStartRow = first.row;
    const int nStartCol = first.col;
    const int nTargetRow = second.row;
    const int nTargetCol = second.col;
    const int nUnvisited = kMaxTurns + 1;

    auto isBlocked = [&](int nRow, int nCol) -> bool {
        if (nRow < 0 || nRow >= nRows || nCol < 0 || nCol >= nCols) {
            return true;
        }

        if (nRow == nTargetRow && nCol == nTargetCol) {
            return false;
        }

        if (nRow == nStartRow && nCol == nStartCol) {
            return true;
        }

        return pGameMap[nRow][nCol] >= 0;
    };

    std::queue<SearchState> searchQueue;
    std::vector<std::vector<std::array<int, 4>>> bestTurns(
        nRows,
        std::vector<std::array<int, 4>>(nCols, { nUnvisited, nUnvisited, nUnvisited, nUnvisited }));

    for (int i = 0; i < 4; ++i) {
        const int nNextRow = nStartRow + kRowOffset[i];
        const int nNextCol = nStartCol + kColOffset[i];
        if (isBlocked(nNextRow, nNextCol)) {
            continue;
        }

        bestTurns[nNextRow][nNextCol][i] = 0;
        searchQueue.push({ nNextRow, nNextCol, i, 0 });
    }

    while (!searchQueue.empty()) {
        const SearchState current = searchQueue.front();
        searchQueue.pop();

        if (current.row == nTargetRow && current.col == nTargetCol) {
            return true;
        }

        for (int i = 0; i < 4; ++i) {
            const int nNextTurns = current.turns + (i == current.direction ? 0 : 1);
            if (nNextTurns > kMaxTurns) {
                continue;
            }

            const int nNextRow = current.row + kRowOffset[i];
            const int nNextCol = current.col + kColOffset[i];
            if (isBlocked(nNextRow, nNextCol)) {
                continue;
            }

            if (bestTurns[nNextRow][nNextCol][i] <= nNextTurns) {
                continue;
            }

            bestTurns[nNextRow][nNextCol][i] = nNextTurns;
            searchQueue.push({ nNextRow, nNextCol, i, nNextTurns });
        }
    }

    return false;
}

bool CGameLogic::FindHint(int** pGameMap, Vertex& first, Vertex& second) const
{
    first = Vertex();
    second = Vertex();

    if (pGameMap == nullptr) {
        return false;
    }

    for (int i = 0; i < CGameControl::s_nRows; ++i) {
        for (int j = 0; j < CGameControl::s_nCols; ++j) {
            const int nKind = pGameMap[i][j];
            if (nKind < 0) {
                continue;
            }

            const Vertex current(i, j, nKind);
            for (int m = i; m < CGameControl::s_nRows; ++m) {
                const int nStartCol = (m == i ? j + 1 : 0);
                for (int n = nStartCol; n < CGameControl::s_nCols; ++n) {
                    if (pGameMap[m][n] != nKind) {
                        continue;
                    }

                    const Vertex candidate(m, n, nKind);
                    if (CanLink(pGameMap, current, candidate)) {
                        first = current;
                        second = candidate;
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool CGameLogic::FindMatchForVertex(int** pGameMap, const Vertex& selected, Vertex& match) const
{
    match = Vertex();

    if (pGameMap == nullptr) {
        return false;
    }

    if (selected.row < 0 || selected.row >= CGameControl::s_nRows ||
        selected.col < 0 || selected.col >= CGameControl::s_nCols) {
        return false;
    }

    const int nKind = pGameMap[selected.row][selected.col];
    if (nKind < 0) {
        return false;
    }

    const Vertex current(selected.row, selected.col, nKind);
    for (int i = 0; i < CGameControl::s_nRows; ++i) {
        for (int j = 0; j < CGameControl::s_nCols; ++j) {
            if (i == current.row && j == current.col) {
                continue;
            }

            if (pGameMap[i][j] != nKind) {
                continue;
            }

            const Vertex candidate(i, j, nKind);
            if (CanLink(pGameMap, current, candidate)) {
                match = candidate;
                return true;
            }
        }
    }

    return false;
}
