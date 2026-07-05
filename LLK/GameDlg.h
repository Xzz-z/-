#pragma once

#include <afxwin.h>

#include "GameControl.h"
#include "Resource.h"

class CGameDlg : public CDialogEx
{
public:
    CGameDlg(GameMode gameMode = GameMode::Basic, CWnd* pParent = nullptr);

    enum { IDD = IDD_GAME_DIALOG };

protected:
    void DoDataExchange(CDataExchange* pDX) override;
    BOOL OnInitDialog() override;
    void OnCancel() override;
    void OnOK() override;

    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedBtnStart();
    afx_msg void OnBnClickedBtnPause();
    afx_msg void OnBnClickedBtnHint();
    afx_msg void OnBnClickedBtnRefresh();
    afx_msg void OnBnClickedBtnGameSetting();
    afx_msg void OnBnClickedBtnGameHelp();
    DECLARE_MESSAGE_MAP()

private:
    CBitmap m_bmpMem;
    CBitmap m_bmpBackground;
    CBitmap m_bmpElement;
    CBitmap m_bmpMask;
    CDC m_dcMem;
    CDC m_dcBG;
    CDC m_dcElement;
    CDC m_dcMask;
    HICON m_hIcon;
    CPoint m_ptGameTop;
    CSize m_sizeElem;
    CRect m_rtGameRect;
    GameMode m_gameMode;
    CGameControl m_gameControl;
    bool m_bGameStarted;
    bool m_bPaused;
    bool m_bCompletionRecorded;
    bool m_bHasSelection;
    bool m_bHasHintPair;
    bool m_bHintBlinkVisible;
    ULONGLONG m_nElapsedBeforePauseMs;
    ULONGLONG m_nLastResumeTickMs;
    Vertex m_selectedVertex;
    Vertex m_hintFirst;
    Vertex m_hintSecond;

    CString BuildThemePath(LPCTSTR pszRelativePath) const;
    void InitBackground();
    void InitElement();
    void UpdateWindowSize();
    void LayoutControls();
    void UpdateMap();
    void DrawStatusPanel();
    void DrawBoardGrid();
    void DrawVertexFrame(const Vertex& vertex, COLORREF clrFrame, int nPenWidth);
    CString BuildWindowTitle() const;
    bool IsTimerVisible() const;
    void ResetTimingState();
    void PauseTiming();
    void ResumeTiming();
    ULONGLONG GetElapsedMilliseconds() const;
    int GetElapsedSeconds() const;
    void FinalizeCompletedGame();
    void StartHintBlink();
    void StopHintBlink();
    bool TryBuildVertexFromPoint(const CPoint& point, Vertex& vertex) const;
    void ResetSelection();
    void ResetHintPair();
    void HandleBoardClick(const Vertex& vertex);
    void ExitToMainMenu();
};
