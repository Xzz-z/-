#include "pch.h"
#include "GameDlg.h"

#include "Leaderboard.h"

namespace
{
constexpr UINT_PTR kGameTimerId = 1;
constexpr UINT_PTR kHintBlinkTimerId = 2;

bool FileExists(const CString& path)
{
    return ::GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
}
}

CGameDlg::CGameDlg(GameMode gameMode, CWnd* pParent)
    : CDialogEx(IDD_GAME_DIALOG, pParent),
    m_hIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME)),
    m_ptGameTop(GAME_LEFT, GAME_TOP),
    m_sizeElem(ELEM_WIDTH, ELEM_HEIGHT),
    m_gameMode(gameMode),
    m_bGameStarted(false),
    m_bPaused(false),
    m_bCompletionRecorded(false),
    m_bHasSelection(false),
    m_bHasHintPair(false),
    m_bHintBlinkVisible(false),
    m_nElapsedBeforePauseMs(0),
    m_nLastResumeTickMs(0),
    m_selectedVertex(),
    m_hintFirst(),
    m_hintSecond()
{
    m_rtGameRect.SetRect(
        m_ptGameTop.x,
        m_ptGameTop.y,
        m_ptGameTop.x + CGameControl::s_nCols * m_sizeElem.cx,
        m_ptGameTop.y + CGameControl::s_nRows * m_sizeElem.cy);
}

void CGameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGameDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_START, &CGameDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_PAUSE, &CGameDlg::OnBnClickedBtnPause)
    ON_BN_CLICKED(IDC_BTN_HINT, &CGameDlg::OnBnClickedBtnHint)
    ON_BN_CLICKED(IDC_BTN_REFRESH, &CGameDlg::OnBnClickedBtnRefresh)
    ON_BN_CLICKED(IDC_BTN_GAME_SETTING, &CGameDlg::OnBnClickedBtnGameSetting)
    ON_BN_CLICKED(IDC_BTN_GAME_HELP, &CGameDlg::OnBnClickedBtnGameHelp)
END_MESSAGE_MAP()

BOOL CGameDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    SetWindowText(BuildWindowTitle());

    UpdateWindowSize();
    LayoutControls();
    GetDlgItem(IDC_BTN_START)->SetWindowText(_T("开始游戏"));
    GetDlgItem(IDC_BTN_PAUSE)->SetWindowText(_T("暂停"));
    GetDlgItem(IDC_BTN_HINT)->SetWindowText(_T("提示"));
    GetDlgItem(IDC_BTN_REFRESH)->SetWindowText(_T("重排"));
    GetDlgItem(IDC_BTN_GAME_SETTING)->SetWindowText(_T("返回主界面"));
    GetDlgItem(IDC_BTN_GAME_HELP)->SetWindowText(_T("帮助"));

    try {
        InitBackground();
        InitElement();
        UpdateMap();

        if (IsTimerVisible()) {
            SetTimer(kGameTimerId, 1000, nullptr);
        }
    }
    catch (const CGameException& ex) {
        AfxMessageBox(ex.GetErrorMessage(), MB_ICONERROR);
        EndDialog(IDCANCEL);
        return FALSE;
    }

    return TRUE;
}

void CGameDlg::OnCancel()
{
    ExitToMainMenu();
}

void CGameDlg::OnOK()
{
}

CString CGameDlg::BuildWindowTitle() const
{
    if (m_gameMode == GameMode::Relax) {
        return _T("欢乐连连看");
    }

    CString strTitle;
    strTitle.Format(_T("欢乐连连看--%s"), GetGameModeDisplayName(m_gameMode).GetString());
    return strTitle;
}

bool CGameDlg::IsTimerVisible() const
{
    return m_gameMode == GameMode::Basic;
}

void CGameDlg::ResetTimingState()
{
    m_nElapsedBeforePauseMs = 0;
    m_nLastResumeTickMs = 0;
}

void CGameDlg::PauseTiming()
{
    if (m_nLastResumeTickMs == 0) {
        return;
    }

    m_nElapsedBeforePauseMs += ::GetTickCount64() - m_nLastResumeTickMs;
    m_nLastResumeTickMs = 0;
}

void CGameDlg::ResumeTiming()
{
    if (!m_bGameStarted || m_bPaused || m_gameControl.IsCleared()) {
        return;
    }

    if (m_nLastResumeTickMs == 0) {
        m_nLastResumeTickMs = ::GetTickCount64();
    }
}

ULONGLONG CGameDlg::GetElapsedMilliseconds() const
{
    ULONGLONG nElapsedMs = m_nElapsedBeforePauseMs;
    if (m_bGameStarted && !m_bPaused && m_nLastResumeTickMs != 0) {
        nElapsedMs += ::GetTickCount64() - m_nLastResumeTickMs;
    }

    return nElapsedMs;
}

int CGameDlg::GetElapsedSeconds() const
{
    return static_cast<int>(GetElapsedMilliseconds() / 1000ULL);
}

CString CGameDlg::BuildThemePath(LPCTSTR pszRelativePath) const
{
    TCHAR szModulePath[MAX_PATH] = {};
    ::GetModuleFileName(nullptr, szModulePath, MAX_PATH);

    CString strExeDir(szModulePath);
    const int nPos = strExeDir.ReverseFind(_T('\\'));
    if (nPos >= 0) {
        strExeDir = strExeDir.Left(nPos);
    }

    TCHAR szCurrentDir[MAX_PATH] = {};
    ::GetCurrentDirectory(MAX_PATH, szCurrentDir);

    std::vector<CString> candidates;
    candidates.push_back(CString(szCurrentDir) + _T("\\") + pszRelativePath);
    candidates.push_back(strExeDir + _T("\\") + pszRelativePath);
    candidates.push_back(strExeDir + _T("\\..\\") + pszRelativePath);
    candidates.push_back(strExeDir + _T("\\..\\..\\") + pszRelativePath);
    candidates.push_back(strExeDir + _T("\\..\\..\\..\\") + pszRelativePath);

    for (const CString& candidate : candidates) {
        if (FileExists(candidate)) {
            return candidate;
        }
    }

    return pszRelativePath;
}

void CGameDlg::InitBackground()
{
    CClientDC dc(this);

    if (m_dcBG.GetSafeHdc() == nullptr) {
        m_dcBG.CreateCompatibleDC(&dc);
    }

    if (m_dcMem.GetSafeHdc() == nullptr) {
        m_dcMem.CreateCompatibleDC(&dc);
    }

    if (m_bmpMem.GetSafeHandle() == nullptr) {
        m_bmpMem.CreateCompatibleBitmap(&dc, GAMEWND_WIDTH, GAMEWND_HEIGHT);
        m_dcMem.SelectObject(&m_bmpMem);
    }

    HANDLE hBmp = ::LoadImage(
        nullptr,
        BuildThemePath(_T("theme\\picture\\game_bg.bmp")),
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hBmp == nullptr) {
        throw CGameException(_T("游戏背景图片加载失败！"));
    }

    if (m_bmpBackground.GetSafeHandle() != nullptr) {
        m_bmpBackground.DeleteObject();
    }
    m_bmpBackground.Attach(static_cast<HBITMAP>(hBmp));
    m_dcBG.SelectObject(&m_bmpBackground);
}

void CGameDlg::InitElement()
{
    CClientDC dc(this);

    if (m_dcElement.GetSafeHdc() == nullptr) {
        m_dcElement.CreateCompatibleDC(&dc);
    }

    if (m_dcMask.GetSafeHdc() == nullptr) {
        m_dcMask.CreateCompatibleDC(&dc);
    }

    HANDLE hBmp = ::LoadImage(
        nullptr,
        BuildThemePath(_T("theme\\picture\\fruit_element.bmp")),
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    HANDLE hMask = ::LoadImage(
        nullptr,
        BuildThemePath(_T("theme\\picture\\fruit_mask.bmp")),
        IMAGE_BITMAP,
        0,
        0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hBmp == nullptr || hMask == nullptr) {
        if (hBmp != nullptr) {
            ::DeleteObject(hBmp);
        }
        if (hMask != nullptr) {
            ::DeleteObject(hMask);
        }
        throw CGameException(_T("游戏元素图片或掩码图片加载失败！"));
    }

    if (m_bmpElement.GetSafeHandle() != nullptr) {
        m_bmpElement.DeleteObject();
    }
    if (m_bmpMask.GetSafeHandle() != nullptr) {
        m_bmpMask.DeleteObject();
    }

    m_bmpElement.Attach(static_cast<HBITMAP>(hBmp));
    m_bmpMask.Attach(static_cast<HBITMAP>(hMask));
    m_dcElement.SelectObject(&m_bmpElement);
    m_dcMask.SelectObject(&m_bmpMask);
}

void CGameDlg::UpdateWindowSize()
{
    CRect rtWin;
    CRect rtClient;
    GetWindowRect(rtWin);
    GetClientRect(rtClient);

    const int nSpanWidth = rtWin.Width() - rtClient.Width();
    const int nSpanHeight = rtWin.Height() - rtClient.Height();
    MoveWindow(0, 0, GAMEWND_WIDTH + nSpanWidth, GAMEWND_HEIGHT + nSpanHeight);
    CenterWindow();
}

void CGameDlg::LayoutControls()
{
    GetDlgItem(IDC_BTN_START)->MoveWindow(710, 100, 70, 30);
    GetDlgItem(IDC_BTN_PAUSE)->MoveWindow(710, 160, 70, 30);
    GetDlgItem(IDC_BTN_HINT)->MoveWindow(710, 220, 70, 30);
    GetDlgItem(IDC_BTN_REFRESH)->MoveWindow(710, 280, 70, 30);
    GetDlgItem(IDC_BTN_GAME_SETTING)->MoveWindow(710, 470, 70, 30);
    GetDlgItem(IDC_BTN_GAME_HELP)->MoveWindow(710, 520, 70, 30);
}

void CGameDlg::UpdateMap()
{
    if (m_dcBG.GetSafeHdc() == nullptr || m_dcMem.GetSafeHdc() == nullptr) {
        return;
    }

    m_dcMem.BitBlt(0, 0, GAMEWND_WIDTH, GAMEWND_HEIGHT, &m_dcBG, 0, 0, SRCCOPY);
    DrawBoardGrid();

    if (m_bGameStarted && m_gameControl.HasMap()) {
        for (int i = 0; i < CGameControl::s_nRows; ++i) {
            for (int j = 0; j < CGameControl::s_nCols; ++j) {
                const int nElemVal = m_gameControl.GetElement(i, j);
                if (nElemVal < 0) {
                    continue;
                }

                const int nLeft = m_ptGameTop.x + j * m_sizeElem.cx;
                const int nTop = m_ptGameTop.y + i * m_sizeElem.cy;
                const bool bIsHintCell =
                    m_bHasHintPair &&
                    !m_bPaused &&
                    ((i == m_hintFirst.row && j == m_hintFirst.col) ||
                        (i == m_hintSecond.row && j == m_hintSecond.col));

                if (bIsHintCell && !m_bHintBlinkVisible) {
                    CRect rtBlink(nLeft + 2, nTop + 2, nLeft + m_sizeElem.cx - 2, nTop + m_sizeElem.cy - 2);
                    m_dcMem.FillSolidRect(&rtBlink, RGB(255, 236, 140));
                    continue;
                }

                m_dcMem.BitBlt(
                    nLeft,
                    nTop,
                    m_sizeElem.cx,
                    m_sizeElem.cy,
                    &m_dcMask,
                    0,
                    nElemVal * m_sizeElem.cy,
                    SRCPAINT);

                m_dcMem.BitBlt(
                    nLeft,
                    nTop,
                    m_sizeElem.cx,
                    m_sizeElem.cy,
                    &m_dcElement,
                    0,
                    nElemVal * m_sizeElem.cy,
                    SRCAND);
            }
        }
    }

    if (m_bHasHintPair && !m_bPaused && m_bHintBlinkVisible) {
        DrawVertexFrame(m_hintFirst, RGB(96, 160, 96), 2);
        DrawVertexFrame(m_hintSecond, RGB(96, 160, 96), 2);
    }

    if (m_bHasSelection && !m_bPaused) {
        DrawVertexFrame(m_selectedVertex, RGB(255, 180, 0), 3);
    }

    if (m_bPaused) {
        CBrush brush(RGB(240, 240, 200));
        m_dcMem.FillRect(&m_rtGameRect, &brush);
        m_dcMem.SetBkMode(TRANSPARENT);
        m_dcMem.SetTextColor(RGB(60, 60, 60));
        m_dcMem.DrawText(_T("游戏已暂停"), &m_rtGameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    DrawStatusPanel();
    Invalidate(FALSE);
}

void CGameDlg::DrawStatusPanel()
{
    if (!IsTimerVisible()) {
        return;
    }

    CRect rtPanel(700, 24, 790, 84);
    m_dcMem.FillSolidRect(&rtPanel, RGB(255, 247, 229));
    m_dcMem.Draw3dRect(&rtPanel, RGB(255, 252, 241), RGB(182, 150, 104));

    m_dcMem.SetBkMode(TRANSPARENT);
    m_dcMem.SetTextColor(RGB(88, 62, 28));

    CRect rtLabel(rtPanel);
    rtLabel.DeflateRect(10, 8, 10, 30);
    m_dcMem.DrawText(_T("计时"), &rtLabel, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    CString strElapsed = FormatElapsedSeconds(GetElapsedSeconds());
    CRect rtElapsed(rtPanel);
    rtElapsed.DeflateRect(8, 28, 8, 8);
    m_dcMem.DrawText(strElapsed, &rtElapsed, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void CGameDlg::DrawBoardGrid()
{
    const COLORREF clrBoardBorder = RGB(126, 94, 56);
    const COLORREF clrCellDark = RGB(244, 231, 201);
    const COLORREF clrCellLight = RGB(252, 243, 220);
    const COLORREF clrCellEdgeLight = RGB(255, 251, 236);
    const COLORREF clrCellEdgeDark = RGB(207, 180, 128);

    m_dcMem.Draw3dRect(&m_rtGameRect, clrCellEdgeLight, clrBoardBorder);

    for (int i = 0; i < CGameControl::s_nRows; ++i) {
        for (int j = 0; j < CGameControl::s_nCols; ++j) {
            CRect rtCell(
                m_ptGameTop.x + j * m_sizeElem.cx,
                m_ptGameTop.y + i * m_sizeElem.cy,
                m_ptGameTop.x + (j + 1) * m_sizeElem.cx,
                m_ptGameTop.y + (i + 1) * m_sizeElem.cy);

            const COLORREF clrCell = ((i + j) % 2 == 0) ? clrCellLight : clrCellDark;
            m_dcMem.FillSolidRect(&rtCell, clrCell);
            m_dcMem.Draw3dRect(&rtCell, clrCellEdgeLight, clrCellEdgeDark);
        }
    }
}

void CGameDlg::DrawVertexFrame(const Vertex& vertex, COLORREF clrFrame, int nPenWidth)
{
    if (vertex.row < 0 || vertex.row >= CGameControl::s_nRows ||
        vertex.col < 0 || vertex.col >= CGameControl::s_nCols ||
        vertex.kind < 0) {
        return;
    }

    const int nLeft = m_ptGameTop.x + vertex.col * m_sizeElem.cx;
    const int nTop = m_ptGameTop.y + vertex.row * m_sizeElem.cy;
    CPen pen(PS_SOLID, nPenWidth, clrFrame);
    CPen* pOldPen = m_dcMem.SelectObject(&pen);
    CBrush* pOldBrush = static_cast<CBrush*>(m_dcMem.SelectStockObject(HOLLOW_BRUSH));

    m_dcMem.Rectangle(nLeft + 1, nTop + 1, nLeft + m_sizeElem.cx - 1, nTop + m_sizeElem.cy - 1);

    m_dcMem.SelectObject(pOldBrush);
    m_dcMem.SelectObject(pOldPen);
}

void CGameDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        const int cxIcon = GetSystemMetrics(SM_CXICON);
        const int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        const int x = (rect.Width() - cxIcon + 1) / 2;
        const int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
        return;
    }

    CPaintDC dc(this);
    dc.BitBlt(0, 0, GAMEWND_WIDTH, GAMEWND_HEIGHT, &m_dcMem, 0, 0, SRCCOPY);
}

HCURSOR CGameDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

BOOL CGameDlg::OnEraseBkgnd(CDC* pDC)
{
    UNREFERENCED_PARAMETER(pDC);
    return TRUE;
}

void CGameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    Vertex vertex;
    if (TryBuildVertexFromPoint(point, vertex)) {
        HandleBoardClick(vertex);
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

void CGameDlg::OnClose()
{
    ExitToMainMenu();
}

void CGameDlg::OnDestroy()
{
    KillTimer(kGameTimerId);
    KillTimer(kHintBlinkTimerId);
    CDialogEx::OnDestroy();
}

void CGameDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == kGameTimerId && IsTimerVisible() && m_bGameStarted && !m_bPaused) {
        UpdateMap();
        return;
    }

    if (nIDEvent == kHintBlinkTimerId && m_bHasHintPair && !m_bPaused) {
        m_bHintBlinkVisible = !m_bHintBlinkVisible;
        UpdateMap();
        return;
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CGameDlg::OnBnClickedBtnStart()
{
    try {
        m_gameControl.StartGame();
        m_bGameStarted = true;
        m_bPaused = false;
        m_bCompletionRecorded = false;
        ResetTimingState();
        ResumeTiming();
        ResetSelection();
        ResetHintPair();
        GetDlgItem(IDC_BTN_PAUSE)->SetWindowText(_T("暂停"));
        UpdateMap();
    }
    catch (const CGameException& ex) {
        AfxMessageBox(ex.GetErrorMessage(), MB_ICONERROR);
    }
}

void CGameDlg::OnBnClickedBtnPause()
{
    if (!m_bGameStarted) {
        AfxMessageBox(_T("请先点击“开始游戏”按钮生成地图。"), MB_ICONINFORMATION);
        return;
    }

    if (!m_bPaused) {
        PauseTiming();
        m_bPaused = true;
        ResetSelection();
        ResetHintPair();
    }
    else {
        m_bPaused = false;
        ResumeTiming();
    }

    GetDlgItem(IDC_BTN_PAUSE)->SetWindowText(m_bPaused ? _T("继续") : _T("暂停"));
    UpdateMap();
}

void CGameDlg::OnBnClickedBtnHint()
{
    if (!m_bGameStarted || !m_gameControl.HasMap()) {
        AfxMessageBox(_T("请先开始游戏，再使用提示功能。"), MB_ICONINFORMATION);
        return;
    }

    if (m_bPaused) {
        AfxMessageBox(_T("暂停状态下无法使用提示，请先继续游戏。"), MB_ICONINFORMATION);
        return;
    }

    if (m_gameControl.IsCleared()) {
        ResetSelection();
        ResetHintPair();
        UpdateMap();
        AfxMessageBox(_T("当前地图已经全部消除，无需再使用提示。"), MB_ICONINFORMATION);
        return;
    }

    const bool bHadSelection = m_bHasSelection && m_selectedVertex.kind >= 0;
    Vertex first;
    Vertex second;
    if (bHadSelection) {
        first = m_selectedVertex;
        if (!m_gameControl.FindMatchForVertex(m_selectedVertex, second)) {
            ResetHintPair();
            UpdateMap();
            AfxMessageBox(_T("当前选中的图片没有可直接消除的配对，请重新选择或点击“重排”。"), MB_ICONINFORMATION);
            return;
        }
    }
    else {
        if (!m_gameControl.FindHint(first, second)) {
            ResetHintPair();
            UpdateMap();
            AfxMessageBox(_T("当前地图中没有可直接消除的图片组合，请点击“重排”。"), MB_ICONINFORMATION);
            return;
        }

        m_selectedVertex = first;
        m_bHasSelection = true;
    }

    m_hintFirst = first;
    m_hintSecond = second;
    m_bHasHintPair = true;
    StartHintBlink();
    UpdateMap();

    if (bHadSelection) {
        AfxMessageBox(_T("已为当前选中的图片高亮合法配对。"), MB_ICONINFORMATION);
        return;
    }

    AfxMessageBox(_T("已按游戏规则选中一张图片，并高亮其合法配对。"), MB_ICONINFORMATION);
}

void CGameDlg::OnBnClickedBtnRefresh()
{
    OnBnClickedBtnStart();
}

void CGameDlg::OnBnClickedBtnGameSetting()
{
    ExitToMainMenu();
}

void CGameDlg::OnBnClickedBtnGameHelp()
{
    CString strHelp;
    strHelp += _T("游戏操作说明：\n");
    strHelp += _T("1. 点击“开始游戏”生成 10 行 16 列地图；\n");
    strHelp += _T("2. 点击两张能在两次转弯内连通的相同图片即可消除；\n");
    strHelp += _T("3. “提示”会高亮一组可消除图片，“重排”会重新生成地图；\n");
    if (IsTimerVisible()) {
        strHelp += _T("4. 基本模式会显示计时，暂停时计时也会同步暂停；\n");
    }
    else {
        strHelp += _T("4. 当前模式不显示计时器，但完成本局时仍会记录用时到排行榜；\n");
    }
    strHelp += _T("5. 点击“返回主界”或按 Esc 可返回主界面。");
    AfxMessageBox(strHelp, MB_ICONINFORMATION);
}

bool CGameDlg::TryBuildVertexFromPoint(const CPoint& point, Vertex& vertex) const
{
    vertex = Vertex();

    if (!m_rtGameRect.PtInRect(point)) {
        return false;
    }

    const int nCol = (point.x - m_ptGameTop.x) / m_sizeElem.cx;
    const int nRow = (point.y - m_ptGameTop.y) / m_sizeElem.cy;
    if (nRow < 0 || nRow >= CGameControl::s_nRows || nCol < 0 || nCol >= CGameControl::s_nCols) {
        return false;
    }

    vertex = Vertex(nRow, nCol, m_gameControl.GetElement(nRow, nCol));
    return true;
}

void CGameDlg::ResetSelection()
{
    m_bHasSelection = false;
    m_selectedVertex = Vertex();
}

void CGameDlg::ResetHintPair()
{
    m_bHasHintPair = false;
    m_hintFirst = Vertex();
    m_hintSecond = Vertex();
    StopHintBlink();
}

void CGameDlg::StartHintBlink()
{
    m_bHintBlinkVisible = true;
    SetTimer(kHintBlinkTimerId, 250, nullptr);
}

void CGameDlg::StopHintBlink()
{
    KillTimer(kHintBlinkTimerId);
    m_bHintBlinkVisible = false;
}

void CGameDlg::FinalizeCompletedGame()
{
    PauseTiming();

    if (!m_bCompletionRecorded) {
        m_bCompletionRecorded = AppendLeaderboardEntry(m_gameMode, GetElapsedSeconds());
    }

    m_bGameStarted = false;
    m_bPaused = false;
    ResetSelection();
    ResetHintPair();
    GetDlgItem(IDC_BTN_PAUSE)->SetWindowText(_T("暂停"));
}

void CGameDlg::HandleBoardClick(const Vertex& vertex)
{
    if (!m_bGameStarted || !m_gameControl.HasMap() || m_bPaused) {
        return;
    }

    ResetHintPair();

    if (vertex.kind < 0) {
        if (m_bHasSelection) {
            ResetSelection();
            UpdateMap();
        }
        return;
    }

    if (!m_bHasSelection) {
        m_selectedVertex = vertex;
        m_bHasSelection = true;
        UpdateMap();
        return;
    }

    if (m_selectedVertex.row == vertex.row && m_selectedVertex.col == vertex.col) {
        ResetSelection();
        UpdateMap();
        return;
    }

    if (m_selectedVertex.kind == vertex.kind && m_gameControl.RemovePair(m_selectedVertex, vertex)) {
        ResetSelection();
        UpdateMap();

        if (m_gameControl.IsCleared()) {
            const CString strModeName = GetGameModeDisplayName(m_gameMode);
            const CString strElapsed = FormatElapsedSeconds(GetElapsedSeconds());
            FinalizeCompletedGame();

            CString strMessage;
            strMessage.Format(
                _T("恭喜，%s已完成！\n本局记录用时：%s\n%s\n是否返回主界面？"),
                strModeName.GetString(),
                strElapsed.GetString(),
                m_bCompletionRecorded ? _T("本局成绩已计入排行榜。") : _T("排行榜写入失败，请检查记录文件。"));
            const int nResult = AfxMessageBox(
                strMessage,
                MB_ICONINFORMATION | MB_YESNO);
            if (nResult == IDYES) {
                EndDialog(IDOK);
            }
            UpdateMap();
        }
        return;
    }

    m_selectedVertex = vertex;
    m_bHasSelection = true;
    UpdateMap();
}

void CGameDlg::ExitToMainMenu()
{
    if (m_bGameStarted && !m_gameControl.IsCleared()) {
        const int nResult = AfxMessageBox(
            _T("返回主界面会结束当前对局，是否继续？"),
            MB_ICONQUESTION | MB_YESNO);
        if (nResult != IDYES) {
            return;
        }
    }

    EndDialog(IDOK);
}
