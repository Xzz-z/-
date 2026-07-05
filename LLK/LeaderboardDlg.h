#pragma once

#include "Resource.h"

class CLeaderboardDlg : public CDialogEx
{
public:
    explicit CLeaderboardDlg(CWnd* pParent = nullptr);

    enum { IDD = IDD_LEADERBOARD_DIALOG };

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    BOOL OnInitDialog() override;

    afx_msg void OnBnClickedBtnClose();
    DECLARE_MESSAGE_MAP()

private:
    CListCtrl m_listLeaderboard;

    void InitColumns();
    void PopulateEntries();
};
