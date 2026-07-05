#include "pch.h"
#include "Leaderboard.h"

#include <algorithm>

namespace
{
CString BuildLeaderboardPath()
{
    TCHAR szModulePath[MAX_PATH] = {};
    ::GetModuleFileName(nullptr, szModulePath, MAX_PATH);

    CString strModulePath(szModulePath);
    const int nPos = strModulePath.ReverseFind(_T('\\'));
    CString strExeDir = (nPos >= 0) ? strModulePath.Left(nPos) : CString(_T("."));

    std::vector<CString> candidateDirs;
    candidateDirs.push_back(strExeDir);
    candidateDirs.push_back(strExeDir + _T("\\.."));
    candidateDirs.push_back(strExeDir + _T("\\..\\.."));
    candidateDirs.push_back(strExeDir + _T("\\..\\..\\.."));

    for (const CString& candidateDir : candidateDirs) {
        const CString strProjectFile = candidateDir + _T("\\LLK.vcxproj");
        if (::GetFileAttributes(strProjectFile) != INVALID_FILE_ATTRIBUTES) {
            return candidateDir + _T("\\leaderboard.txt");
        }
    }

    return strExeDir + _T("\\leaderboard.txt");
}
}

CString GetGameModeDisplayName(GameMode mode)
{
    switch (mode) {
    case GameMode::Relax:
        return _T("休闲模式");
    case GameMode::Basic:
    default:
        return _T("基本模式");
    }
}

namespace
{
int GetGameModeStorageValue(GameMode mode)
{
    switch (mode) {
    case GameMode::Relax:
        return 1;
    case GameMode::Basic:
    default:
        return 0;
    }
}

GameMode ParseGameModeStorageValue(int value)
{
    return value == 1 ? GameMode::Relax : GameMode::Basic;
}
}

CString FormatElapsedSeconds(int elapsedSeconds)
{
    if (elapsedSeconds < 0) {
        elapsedSeconds = 0;
    }

    CString strElapsed;
    strElapsed.Format(_T("%02d:%02d"), elapsedSeconds / 60, elapsedSeconds % 60);
    return strElapsed;
}

CString GetLeaderboardFilePath()
{
    return BuildLeaderboardPath();
}

bool AppendLeaderboardEntry(GameMode mode, int elapsedSeconds)
{
    CFile file;
    if (!file.Open(GetLeaderboardFilePath(), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite)) {
        return false;
    }

    file.SeekToEnd();

    CStringA strLine;
    strLine.Format("%d\t%d\r\n", GetGameModeStorageValue(mode), elapsedSeconds);
    file.Write(strLine.GetString(), static_cast<UINT>(strLine.GetLength()));
    file.Close();
    return true;
}

std::vector<LeaderboardEntry> LoadLeaderboardEntries()
{
    std::vector<LeaderboardEntry> entries;

    CStdioFile file;
    if (!file.Open(GetLeaderboardFilePath(), CFile::modeRead | CFile::typeText)) {
        return entries;
    }

    CString strLine;
    while (file.ReadString(strLine)) {
        strLine.Trim();
        if (strLine.IsEmpty()) {
            continue;
        }

        int nTokenPos = 0;
        const CString strMode = strLine.Tokenize(_T("\t"), nTokenPos);
        const CString strElapsed = strLine.Tokenize(_T("\t"), nTokenPos);

        if (strMode.IsEmpty() || strElapsed.IsEmpty()) {
            continue;
        }

        bool bNumericMode = true;
        for (int i = 0; i < strMode.GetLength(); ++i) {
            if (!_istdigit(strMode[i])) {
                bNumericMode = false;
                break;
            }
        }

        CString strModeName = strMode;
        if (bNumericMode) {
            strModeName = GetGameModeDisplayName(ParseGameModeStorageValue(_ttoi(strMode)));
        }

        entries.push_back({ strModeName, _ttoi(strElapsed) });
    }

    std::sort(entries.begin(), entries.end(),
        [](const LeaderboardEntry& lhs, const LeaderboardEntry& rhs) {
            return lhs.elapsedSeconds < rhs.elapsedSeconds;
        });

    return entries;
}
