#include "pch.h"
#include "LeaderboardDlg.h"

#include "Leaderboard.h"

CLeaderboardDlg::CLeaderboardDlg(CWnd* pParent)
    : CDialogEx(IDD_LEADERBOARD_DIALOG, pParent)
{
}

void CLeaderboardDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LEADERBOARD, m_listLeaderboard);
}

BEGIN_MESSAGE_MAP(CLeaderboardDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_LEADERBOARD_CLOSE, &CLeaderboardDlg::OnBnClickedBtnClose)
END_MESSAGE_MAP()

BOOL CLeaderboardDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetWindowText(_T("排行榜"));
    GetDlgItem(IDC_BTN_LEADERBOARD_CLOSE)->SetWindowText(_T("关闭"));

    m_listLeaderboard.SetExtendedStyle(
        m_listLeaderboard.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    InitColumns();
    PopulateEntries();
    CenterWindow();

    return TRUE;
}

void CLeaderboardDlg::InitColumns()
{
    m_listLeaderboard.InsertColumn(0, _T("名次"), LVCFMT_CENTER, 60);
    m_listLeaderboard.InsertColumn(1, _T("模式"), LVCFMT_CENTER, 100);
    m_listLeaderboard.InsertColumn(2, _T("用时"), LVCFMT_CENTER, 120);
}

void CLeaderboardDlg::PopulateEntries()
{
    const std::vector<LeaderboardEntry> entries = LoadLeaderboardEntries();
    if (entries.empty()) {
        const int nItem = m_listLeaderboard.InsertItem(0, _T("-"));
        m_listLeaderboard.SetItemText(nItem, 1, _T("暂无记录"));
        m_listLeaderboard.SetItemText(nItem, 2, _T("-"));
        return;
    }

    const int nDisplayCount = (std::min)(static_cast<int>(entries.size()), 50);
    for (int i = 0; i < nDisplayCount; ++i) {
        CString strRank;
        strRank.Format(_T("%d"), i + 1);

        const int nItem = m_listLeaderboard.InsertItem(i, strRank);
        m_listLeaderboard.SetItemText(nItem, 1, entries[i].modeName);
        m_listLeaderboard.SetItemText(nItem, 2, FormatElapsedSeconds(entries[i].elapsedSeconds));
    }
}

void CLeaderboardDlg::OnBnClickedBtnClose()
{
    EndDialog(IDOK);
}
