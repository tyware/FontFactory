// TTDebug.cpp : implementation file
//

#include "stdafx.h"
#include "FontFactory.h"
#include "TTDebug.h"
#include "afxdialogex.h"
#include "BaseFont.h"
#include "resource.h"


// CTTDebug dialog

IMPLEMENT_DYNAMIC(CTTDebug, CDialogEx)

CTTDebug::CTTDebug(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTTDebug::IDD, pParent)
{
    m_bInitialized = false;

    m_hFont = 0;
    m_hFontFace = 0;
    m_hGlyph = 0;
    m_nGlyphId = 0;
    m_hDebugInfo = 0;
}

CTTDebug::~CTTDebug()
{
}

void CTTDebug::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_GRAPHICSTATE, m_listGraphicState);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_GLYPH_POINTS, m_listGlyphPoints);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_GLYPH_STACK, m_listStack);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_GLYPH_INSTRUCTION, m_listInstruction);
    DDX_Control(pDX, IDC_STATIC_DEBUG_CID, m_staticGid);
    DDX_Control(pDX, IDC_CHECK_TTDEBUG_TWILIGHT, m_chkTwilight);
    DDX_Control(pDX, IDC_RADIO_TTDEBUG_CURRENT, m_radioCurrent);
    DDX_Control(pDX, IDC_RADIO_TTDEBUG_ORIGINAL, m_radioOriginal);
    DDX_Control(pDX, IDC_RADIO_TTDEBUG_GRID, m_radioGrid);
    DDX_Control(pDX, IDC_RADIO_TTDEBUG_EM_UNIT, m_radioEmUnit);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_GLYPH_STORAGE1, m_listStorage);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_GLYPH_CVT, m_listCvt);
    DDX_Control(pDX, IDC_RADIO_DEBUG_FONT, m_radioFont);
    DDX_Control(pDX, IDC_RADIO_DEBUG_FONT_FACE, m_radioFace);
    DDX_Control(pDX, IDC_RADIO_DEBUG_GLYPH, m_radioGlyph);
    DDX_Control(pDX, IDC_LIST_TT_DEBUG_SUB_GLYPHS, m_listSubGlyphs);
}


BEGIN_MESSAGE_MAP(CTTDebug, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_STEP_OVER, &CTTDebug::OnBnClickedButtonStepOver)
    ON_BN_CLICKED(IDC_BUTTON_STEP_IN, &CTTDebug::OnBnClickedButtonStepIn)
    ON_BN_CLICKED(IDC_BUTTON_STEP_OUT, &CTTDebug::OnBnClickedButtonStepOut)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDOK, &CTTDebug::OnBnClickedOk)
    ON_BN_CLICKED(IDC_RADIO_TTDEBUG_CURRENT, &CTTDebug::OnBnClickedRadioTtdebugCurrent)
    ON_BN_CLICKED(IDC_RADIO_TTDEBUG_ORIGINAL, &CTTDebug::OnBnClickedRadioTtdebugOriginal)
    ON_BN_CLICKED(IDC_RADIO_TTDEBUG_GRID, &CTTDebug::OnBnClickedRadioTtdebugGrid)
    ON_BN_CLICKED(IDC_RADIO_TTDEBUG_EM_UNIT, &CTTDebug::OnBnClickedRadioTtdebugEmUnit)
    ON_BN_CLICKED(IDC_CHECK_TTDEBUG_TWILIGHT, &CTTDebug::OnBnClickedCheckTtdebugTwilight)
    ON_BN_CLICKED(IDC_RADIO_DEBUG_FONT, &CTTDebug::OnBnClickedRadioDebugFont)
    ON_BN_CLICKED(IDC_RADIO_DEBUG_FONT_FACE, &CTTDebug::OnBnClickedRadioDebugFontFace)
    ON_BN_CLICKED(IDC_RADIO_DEBUG_GLYPH, &CTTDebug::OnBnClickedRadioDebugGlyph)
    ON_BN_CLICKED(IDC_BUTTON_CONTINUE, &CTTDebug::OnBnClickedButtonContinue)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_TT_DEBUG_GLYPH_INSTRUCTION, &CTTDebug::OnDblclkListTtDebugGlyphInstruction)
    ON_BN_CLICKED(IDC_BUTTON_DEBUG_RESTART, &CTTDebug::OnBnClickedButtonDebugRestart)
END_MESSAGE_MAP()


// CTTDebug message handlers


BOOL CTTDebug::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here

    CString cs;
    cs.Format(L"%d(0x%04X)", m_nGlyphId, m_nGlyphId);
    m_staticGid.SetWindowTextW(cs);

    DWORD styles = m_listGraphicState.GetExtendedStyle();
    //styles = WS_CHILD | LVS_REPORT | LVS_EDITLABELS | LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT;
    m_listGraphicState.SetExtendedStyle(styles | LVS_EX_FULLROWSELECT);
    styles = m_listGlyphPoints.GetExtendedStyle();
    m_listGlyphPoints.SetExtendedStyle(styles | LVS_EX_FULLROWSELECT);
    styles = m_listInstruction.GetExtendedStyle();
    m_listInstruction.SetExtendedStyle(styles | LVS_EX_FULLROWSELECT);
    styles = m_listSubGlyphs.GetExtendedStyle();
    m_listSubGlyphs.SetExtendedStyle(styles | LVS_EX_FULLROWSELECT);


    m_listGraphicState.InsertColumn(0, L"Name", 0, 100);
    m_listGraphicState.InsertColumn(1, L"Value", 0, 120);

    m_listGlyphPoints.InsertColumn(0, L"ID", 0, 30);
    m_listGlyphPoints.InsertColumn(1, L"On", 0, 40);
    m_listGlyphPoints.InsertColumn(2, L"T", 0, 30);
    m_listGlyphPoints.InsertColumn(3, L"X", 0, 85);
    m_listGlyphPoints.InsertColumn(4, L"Y", 0, 85);
    m_listGlyphPoints.InsertColumn(5, L"C", 0, 33);

    m_radioCurrent.SetCheck(BST_CHECKED);
    m_radioGrid.SetCheck(BST_CHECKED);

    m_listStack.InsertColumn(0, L"ID", 0, 30);
    m_listStack.InsertColumn(1, L"Value", 0, 80);

    m_listStorage.InsertColumn(0, L"ID", 0, 30);
    m_listStorage.InsertColumn(1, L"Value", 0, 90);

    m_listCvt.InsertColumn(0, L"ID", 0, 30);
    m_listCvt.InsertColumn(1, L"Value", 0, 80);

    m_listInstruction.InsertColumn(0, L"Ln", 0, 45);
    m_listInstruction.InsertColumn(1, L"Cmd", 0, 90);
    m_listInstruction.InsertColumn(2, L"Brk", 0, 35);
    m_listInstruction.InsertColumn(3, L"Run", 0, 35);

    m_listSubGlyphs.InsertColumn(0, L"ID", 0, 30);
    m_listSubGlyphs.InsertColumn(1, L"GlyphId", 0, 75);

    m_radioGlyph.SetCheck(BST_CHECKED);
    //OnBnClickedRadioDebugGlyph();

    m_bInitialized = true;

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


void CTTDebug::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    AdjustLayout();
}

void CTTDebug::AdjustLayout()
{
    if (!m_bInitialized) return;

    CRect rectClient;
    GetClientRect(rectClient);

    CRect rect;

    m_listGraphicState.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_listGraphicState.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rectClient.bottom - rect.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);

    m_listGlyphPoints.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_listGlyphPoints.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rectClient.bottom - rect.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);

    m_listStack.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_listStack.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rectClient.bottom - rect.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);

    m_listStorage.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_listStorage.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rectClient.bottom - rect.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);

    m_listCvt.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_listCvt.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rectClient.bottom - rect.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);

    m_listInstruction.GetWindowRect(&rect);
    ScreenToClient(&rect);
    m_listInstruction.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rectClient.bottom - rect.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CTTDebug::OnBnClickedButtonStepOver()
{
    if (m_nStatus == -1)
    {
        MessageBox(L"There is an error in previous operation!");
        return;
    }
    if (FE_Glyph_Debug_StepOver(m_hFontFace, m_hDebugInfo) == FALSE)
        m_nStatus = -1;
    else
    {
        UpdatePoints();
        UpdateGraphicStates();
        UpdateStack();
        UpdateStorage();
        UpdateCvt();
        UpdateInstructions();
        UpdateSubglyphs();
    }
}


void CTTDebug::OnBnClickedButtonStepIn()
{
    if (m_nStatus == -1)
    {
        MessageBox(L"There is an error in previous operation!");
        return;
    }
    if (FE_Glyph_Debug_StepIn(m_hFontFace, m_hDebugInfo) == FALSE)
        m_nStatus = -1;
    else
    {
        UpdatePoints();
        UpdateGraphicStates();
        UpdateStack();
        UpdateStorage();
        UpdateCvt();
        UpdateInstructions();
        UpdateSubglyphs();
    }
}


void CTTDebug::OnBnClickedButtonStepOut()
{
    if (m_nStatus == -1)
    {
        MessageBox(L"There is an error in previous operation!");
        return;
    }
    if (FE_Glyph_Debug_StepOut(m_hFontFace, m_hDebugInfo) == FALSE)
        m_nStatus = -1;
    else
    {
        UpdatePoints();
        UpdateGraphicStates();
        UpdateStack();
        UpdateStorage();
        UpdateCvt();
        UpdateInstructions();
        UpdateSubglyphs();
    }
}

void CTTDebug::OnBnClickedButtonContinue()
{
    if (m_nStatus == -1)
    {
        MessageBox(L"There is an error in previous operation!");
        return;
    }

    if (FE_Glyph_Debug_Continue(m_hFontFace, m_hDebugInfo) == FALSE)
        m_nStatus = -1;
    else
    {
        UpdatePoints();
        UpdateGraphicStates();
        UpdateStack();
        UpdateStorage();
        UpdateCvt();
        UpdateInstructions();
        UpdateSubglyphs();
    }
}

void CTTDebug::OnBnClickedButtonDebugRestart()
{
    m_hDebugInfo = FE_Glyph_Debug_Restart(m_hFontFace, m_nGlyphId, m_hDebugInfo);

    if (m_hDebugInfo == 0)
    {
        MessageBox(L"Failed to create debug information!");
        m_nStatus = -1;
        return;
    }

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;
    if (pDebugInfo->intructionSet == 0 && pDebugInfo->pOutlines == nullptr)
    {
        FE_Glyph_Debug_End(m_hFontFace, m_hDebugInfo);
        m_hDebugInfo = 0;
        return;
    }

    UpdatePoints();
    UpdateGraphicStates();
    UpdateStack();
    UpdateStorage();
    UpdateCvt();
    UpdateInstructions();
    UpdateSubglyphs();
}

void CTTDebug::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    if (m_hDebugInfo)
    {
        FE_Glyph_Debug_End(m_hFontFace, m_hDebugInfo);
        m_hDebugInfo = 0;
    }

    CDialogEx::OnClose();
}


void CTTDebug::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    if (m_hDebugInfo)
    {
        FE_Glyph_Debug_End(m_hFontFace, m_hDebugInfo);
        m_hDebugInfo = 0;
    }

    CDialogEx::OnOK();
}


void CTTDebug::OnBnClickedRadioTtdebugCurrent()
{
    if (m_radioCurrent.GetCheck() == BST_CHECKED)
        m_radioOriginal.SetCheck(BST_UNCHECKED);
    UpdatePoints();
}
void CTTDebug::OnBnClickedRadioTtdebugOriginal()
{
    if (m_radioOriginal.GetCheck() == BST_CHECKED)
        m_radioCurrent.SetCheck(BST_UNCHECKED);
    UpdatePoints();
}
void CTTDebug::OnBnClickedRadioTtdebugGrid()
{
    if (m_radioGrid.GetCheck() == BST_CHECKED)
        m_radioEmUnit.SetCheck(BST_UNCHECKED);
    UpdatePoints();
}
void CTTDebug::OnBnClickedRadioTtdebugEmUnit()
{
    if (m_radioEmUnit.GetCheck() == BST_CHECKED)
        m_radioGrid.SetCheck(BST_UNCHECKED);
    UpdatePoints();
}


void CTTDebug::OnBnClickedCheckTtdebugTwilight()
{
    UpdatePoints();
}

void CTTDebug::UpdatePoints()
{
    if (m_hDebugInfo == 0) return;

    bool bTwilight = (m_chkTwilight.GetCheck() == BST_CHECKED);
    bool bCurrent = (m_radioCurrent.GetCheck() == BST_CHECKED);
    bool bGrid = (m_radioGrid.GetCheck() == BST_CHECKED);

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;

    if (bTwilight)
    {
        CString cs;
        m_listGlyphPoints.DeleteAllItems();
        for (UINT32 i = 0; i < pDebugInfo->nTwilights; i++)
        {
            cs.Format(L"%d", i);
            m_listGlyphPoints.InsertItem(i, cs);
            m_listGlyphPoints.SetItemText(i, 1, L"T");
            m_listGlyphPoints.SetItemText(i, 2, L"");

            cs.Format(L"%.2f", pDebugInfo->pTwilights[i].x);
            m_listGlyphPoints.SetItemText(i, 3, cs);
            cs.Format(L"%.2f", pDebugInfo->pTwilights[i].y);
            m_listGlyphPoints.SetItemText(i, 4, cs);
        }
    }
    else
    {
        FF_Font_Factory::CGlyphOutlines *pOutlines;
        if (bCurrent) pOutlines = pDebugInfo->pOutlines;
        else pOutlines = pDebugInfo->pOrigOutlines;

        if (pOutlines == nullptr)
        {
            m_listGlyphPoints.DeleteAllItems();
            return;
        }

        if (m_listGlyphPoints.GetItemCount() != pOutlines->numPoints)
            m_listGlyphPoints.DeleteAllItems();
        bool bNew = (m_listGlyphPoints.GetItemCount() == 0);
 
        float scale = 1.0f;
        if (!bGrid) scale = ((FF_Font_Factory::CFontFace*)m_hFontFace)->m_fScale;
        
        CString cs;
        //CString cs0;
        int contour = 0;
        m_listGlyphPoints.SetTextColor(RGB(0, 0, 255));
        for (int i = 0; i < pOutlines->numPoints; i++)
        {
            if (bNew)
            {
                cs.Format(L"%d", i);
                m_listGlyphPoints.InsertItem(i, cs);
            }
            if ((pOutlines->pCoordinates[i].bOnCurve & FF_Font_Factory::PF_TOUCHED_X) == FF_Font_Factory::PF_TOUCHED_X)
            {
                if ((pOutlines->pCoordinates[i].bOnCurve & FF_Font_Factory::PF_TOUCHED_Y) == FF_Font_Factory::PF_TOUCHED_Y)
                {
                    m_listGlyphPoints.SetItemText(i, 1, L"P"); // on curve
                    m_listGlyphPoints.SetItemText(i, 2, L"Z");
                }
                else
                {
                    m_listGlyphPoints.SetItemText(i, 1, L"P");
                    m_listGlyphPoints.SetItemText(i, 2, L"X");
                }
            }
            else if ((pOutlines->pCoordinates[i].bOnCurve & FF_Font_Factory::PF_TOUCHED_Y) == FF_Font_Factory::PF_TOUCHED_Y)
            {
                m_listGlyphPoints.SetItemText(i, 1, L"P");
                m_listGlyphPoints.SetItemText(i, 2, L"Y");
            }
            else if ((pOutlines->pCoordinates[i].bOnCurve & FF_Font_Factory::PF_ONCURVE) == FF_Font_Factory::PF_ONCURVE)
            {
                m_listGlyphPoints.SetItemText(i, 1, L"P");
                m_listGlyphPoints.SetItemText(i, 2, L"");
            }
            else if ((pOutlines->pCoordinates[i].bOnCurve & FF_Font_Factory::PF_HELP) == FF_Font_Factory::PF_HELP)
            {
                m_listGlyphPoints.SetItemText(i, 1, L"F");
                m_listGlyphPoints.SetItemText(i, 2, L"");
            }
            else
            {
                m_listGlyphPoints.SetItemText(i, 1, L"C"); // control point: off curve
                m_listGlyphPoints.SetItemText(i, 2, L"");
            }

            //cs0 = m_listGlyphPoints.GetItemText(i, 3);
            cs.Format(L"%.2f", pOutlines->pCoordinates[i].x / scale);
            //if (cs0!= L"" && cs0 != cs)
            //    m_listGlyphPoints.SetTextColor(RGB(255, 0, 0));
            m_listGlyphPoints.SetItemText(i, 3, cs);

            //cs0 = m_listGlyphPoints.GetItemText(i, 3);
            cs.Format(L"%.2f", pOutlines->pCoordinates[i].y / scale);
            //if (cs0 != L"" && cs0 == cs)
            //    m_listGlyphPoints.SetTextColor(RGB(0, 0, 0));
            m_listGlyphPoints.SetItemText(i, 4, cs);
            //m_listGlyphPoints.SetTextColor(RGB(0, 0, 0));

            if ((unsigned int) i > pOutlines->pEndPtsOfContours[contour]) contour++;
            cs.Format(L"%d", contour);
            m_listGlyphPoints.SetItemText(i, 5, cs);
        }
    }
}

void CTTDebug::UpdateGraphicStates()
{
    if (m_hDebugInfo == 0) return;

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;
    // states
    bool bNew = (m_listGraphicState.GetItemCount() == 0);
    for (size_t i = 0; i < pDebugInfo->stateVariables.size(); i++)
    {
        if (bNew)
            m_listGraphicState.InsertItem((int)i, pDebugInfo->stateVariables[i]->name.c_str());
        m_listGraphicState.SetItemText((int)i, 1, pDebugInfo->stateVariables[i]->value.c_str());
    }
}
void CTTDebug::UpdateStack()
{
    if (m_hDebugInfo == 0) return;

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;
    // states
    CString cs;
    bool bNew = false;
    int size = (int)pDebugInfo->stack.size();
    if (m_listStack.GetItemCount() != size)
    {
        bNew = true;
        m_listStack.DeleteAllItems();
    }
    for (int i = 0; i < pDebugInfo->stack.size(); i++)
    {
        if (bNew)
        {
            cs.Format(L"%d", size - i - 1);
            m_listStack.InsertItem(i, cs);
        }
        cs.Format(L"%d", pDebugInfo->stack[i]);
        m_listStack.SetItemText(i, 1, cs);
    }
}

void CTTDebug::UpdateStorage()
{
    if (m_hDebugInfo == 0) return;

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;

    CString cs;
    int size = (int)pDebugInfo->storage.size();
    bool bNew = false;
    if (m_listStorage.GetItemCount() != size)
    {
        bNew = true;
        m_listStorage.DeleteAllItems();
    }
    for (int i = 0; i < size; i++)
    {
        if (bNew)
        {
            cs.Format(L"%d", i);
            m_listStorage.InsertItem(i, cs);
        }
        if (pDebugInfo->storage[i] == 87654321)
            m_listStorage.SetItemText(i, 1, L"NA");
        else
        {
            cs.Format(L"%d", pDebugInfo->storage[i]);
            m_listStorage.SetItemText(i, 1, cs);
        }
    }
}

void CTTDebug::UpdateCvt()
{
    if (m_hDebugInfo == 0) return;

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;
    
    CString cs;
    bool bNew = false;
    int size = (int)pDebugInfo->nCvt;
    if (m_listCvt.GetItemCount() != size)
    {
        bNew = true;
        m_listCvt.DeleteAllItems();
    }
    for (int i = 0; i < size; i++)
    {
        if (bNew)
        {
            cs.Format(L"%d", i);
            m_listCvt.InsertItem(i, cs);
        }
        cs.Format(L"%d", pDebugInfo->pCvt[i]);
        m_listCvt.SetItemText(i, 1, cs);
    }
}

void CTTDebug::UpdateInstructions()
{
    if (m_hDebugInfo == 0) return;

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;

    if (pDebugInfo->bRefillInstructions)
    {
        m_listInstruction.DeleteAllItems();
    }
    CString cs;
    for (size_t i = 0; i < pDebugInfo->instructions.size(); i++)
    {
        if (pDebugInfo->bRefillInstructions)
        {
            cs.Format(L"%d", i);
            m_listInstruction.InsertItem((int)i, cs);
            m_listInstruction.SetItemData((int)i, 0);
        }
        if (FE_Glyph_Debug_IsBreakPoint(m_hFontFace, m_hDebugInfo, (int)i))
        {
            //if (m_listInstruction.GetItemData((int)i) == 1)
            m_listInstruction.SetItemText((int)i, 2, L"@");
            m_listInstruction.SetItemData((int)i, 1);
        }
        else
            m_listInstruction.SetItemText((int)i, 2, L"");

        m_listInstruction.SetItemText((int)i, 1, pDebugInfo->instructions[i].c_str());
        if (i == pDebugInfo->currentPos)
            m_listInstruction.SetItemText((int)i, 3, pDebugInfo->bEnd?L"end" : L"<<");
        else
            m_listInstruction.SetItemText((int)i, 3, L"");
    }

    m_listInstruction.SetItemState(pDebugInfo->currentPos, LVIS_SELECTED, LVIS_SELECTED);
    if (!m_listInstruction.IsItemVisible(pDebugInfo->currentPos))
    {
        CPoint pt1, pt2;
        m_listInstruction.GetItemPosition(m_listInstruction.GetTopIndex(), &pt1);
        m_listInstruction.GetItemPosition(pDebugInfo->currentPos, &pt2);
        m_listInstruction.Scroll(CSize(0, pt2.y - pt1.y));
    }
}

void CTTDebug::UpdateSubglyphs()
{
    if (m_hDebugInfo == 0) return;

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;

    m_listSubGlyphs.DeleteAllItems();

    CString cs;
    for (size_t i = 0; i < pDebugInfo->subglyphs.size(); i++)
    {
        cs.Format(L"%d", i);
        m_listSubGlyphs.InsertItem((int)i, cs);
        cs.Format(L"%d", pDebugInfo->subglyphs[i]);
        m_listSubGlyphs.SetItemText((int)i, 1, cs);
    }
    m_listSubGlyphs.SetItemState(pDebugInfo->curSubGlyph, LVIS_SELECTED, LVIS_SELECTED);
}


void CTTDebug::OnBnClickedRadioDebugFont()
{
    SetupDebug(2);
}


void CTTDebug::OnBnClickedRadioDebugFontFace()
{
    SetupDebug(1);
}


void CTTDebug::OnBnClickedRadioDebugGlyph()
{
    SetupDebug(0);
}

void CTTDebug::SetupDebug(int set)
{
    if (m_hDebugInfo)
        FE_Glyph_Debug_End(m_hFontFace, m_hDebugInfo);

    m_hDebugInfo = FE_Glyph_Debug_Start(m_hFontFace, m_nGlyphId, set);

    if (m_hDebugInfo == 0)
    {
        MessageBox(L"Failed to create debug information!");
        m_nStatus = -1;
        return;
    }

    FF_Font_Factory::CGlyphDebugInfo *pDebugInfo = (FF_Font_Factory::CGlyphDebugInfo*)m_hDebugInfo;
    if (set == 0 && pDebugInfo->pOutlines == nullptr)
    {
        FE_Glyph_Debug_End(m_hFontFace, m_hDebugInfo);
        m_hDebugInfo = 0;
        return;
    }

    UpdatePoints();
    UpdateGraphicStates();
    UpdateStack();
    UpdateStorage();
    UpdateCvt();
    UpdateInstructions();
    UpdateSubglyphs();
}


void CTTDebug::OnDblclkListTtDebugGlyphInstruction(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    if (m_listInstruction.GetItemData(pNMItemActivate->iItem) == 0)
    {
        m_listInstruction.SetItemData(pNMItemActivate->iItem, 1);
        m_listInstruction.SetItemText(pNMItemActivate->iItem, 2, L"@");
        FE_Glyph_Debug_SetBreakPoint(m_hFontFace, m_hDebugInfo, pNMItemActivate->iItem);
    }
    else
    {
        m_listInstruction.SetItemData(pNMItemActivate->iItem, 0);
        m_listInstruction.SetItemText(pNMItemActivate->iItem, 2, L"");
        FE_Glyph_Debug_DelBreakPoint(m_hFontFace, m_hDebugInfo, pNMItemActivate->iItem);
    }

    *pResult = 0;
}

