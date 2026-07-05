#pragma once

constexpr int MAINWND_WIDTH = 800;
constexpr int MAINWND_HEIGHT = 600;
constexpr int GAMEWND_WIDTH = 800;
constexpr int GAMEWND_HEIGHT = 600;

constexpr int GAME_TOP = 50;
constexpr int GAME_LEFT = 50;
constexpr int ELEM_WIDTH = 40;
constexpr int ELEM_HEIGHT = 40;

constexpr int GAME_ROWS = 10;
constexpr int GAME_COLS = 16;
constexpr int GAME_PIC_NUM = 16;

enum class GameMode
{
    Basic,
    Relax
};

struct Vertex
{
    int row;
    int col;
    int kind;

    Vertex(int nRow = -1, int nCol = -1, int nKind = -1)
        : row(nRow), col(nCol), kind(nKind)
    {
    }
};
