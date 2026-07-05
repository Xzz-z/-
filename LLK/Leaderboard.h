#pragma once

#include "Global.h"

struct LeaderboardEntry
{
    CString modeName;
    int elapsedSeconds;
};

CString GetGameModeDisplayName(GameMode mode);
CString FormatElapsedSeconds(int elapsedSeconds);
CString GetLeaderboardFilePath();
bool AppendLeaderboardEntry(GameMode mode, int elapsedSeconds);
std::vector<LeaderboardEntry> LoadLeaderboardEntries();
