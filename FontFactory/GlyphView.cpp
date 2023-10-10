#include "stdafx.h"
#include "GlyphView.h"
#include "resource.h"
#include "BaseFont.h"
#include "MainFrm.h"
#include "FontFace.h"
//#include "ApiHeader.h"
#include "Raster.h"

#define GLYPH_VIEW_DEFUALT_SCALE                12

class CGlyphViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CGlyphViewMenuButton)

public:
	CGlyphViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CGlyphViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlyphView::CGlyphView(void)
{
    m_hFont = 0;
    m_hFontFace = 0;
    m_pCellInfo = nullptr;
	m_nGlyphId = -1;
    m_nScale = GLYPH_VIEW_DEFUALT_SCALE;
    m_bMarkPoint = true;

    m_hGlyph = 0;
    m_pOutlines = 0;

    m_rectGlyph = CRect(28, 107, 228, 307);
	m_rectWinGlyph = CRect(28, 353, 228, 553);

    m_nEmX = 30;
    m_nEmY = 124;
}


CGlyphView::~CGlyphView(void)
{
}

BEGIN_MESSAGE_MAP(CGlyphView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
    ON_COMMAND(ID_GLYPH_VIEW_SHOW, &CGlyphView::OnGlyphViewShow)
    ON_UPDATE_COMMAND_UI(ID_GLYPH_VIEW_SHOW, &CGlyphView::OnUpdateGlyphViewShow)
    ON_COMMAND(ID_GLYPH_VIEW_MARK_POINTS, &CGlyphView::OnGlyphViewMarkPoints)
    ON_UPDATE_COMMAND_UI(ID_GLYPH_VIEW_MARK_POINTS, &CGlyphView::OnUpdateGlyphViewMarkPoints)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int CGlyphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    //m_staticGlyphId.Create(_T("Glyph Id: "), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_CENTER, rectDummy, this);
    //m_staticFontSize.Create(_T("FontSize: "), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_CENTER, rectDummy, this);
    m_editGlyphId.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | WS_BORDER | ES_CENTER, rectDummy, this, ID_GLYPH_VIEW_GLYPH_ID);
    m_editScale.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | WS_BORDER | ES_CENTER | ES_READONLY, rectDummy, this, ID_GLYPH_VIEW_FONT_SIZE);
    m_cbMarkPoint.Create(_T("Mark Points"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_CHECKBOX, rectDummy, this, ID_GLYPH_VIEW_MARK_POINTS);
	CString cs;
	cs.Format(L"%d", m_nGlyphId);
	m_editGlyphId.SetWindowTextW(cs);
	cs.Format(L"%.02f", m_nScale);
	m_editScale.SetWindowTextW(cs);

    m_btnOk.Create(L"Show", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_CENTER, rectDummy, this, ID_GLYPH_VIEW_SHOW);

    m_cbMarkPoint.SetCheck(m_bMarkPoint ? BST_CHECKED : BST_UNCHECKED);

	return 0;
}

void CGlyphView::OnContextMenu(CWnd* pWnd, CPoint point)
{
}

void CGlyphView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int static_width = 80;
	int edit_width = 50;

	rectClient.left += 3;
	rectClient.top += 3;
	//m_staticGlyphId.SetWindowPos(NULL, rectClient.left, rectClient.top, static_width, 30, SWP_NOACTIVATE | SWP_NOZORDER);
	m_editGlyphId.SetWindowPos(NULL, rectClient.left + static_width + 3, rectClient.top, static_width, 30, SWP_NOACTIVATE | SWP_NOZORDER);

    m_cbMarkPoint.SetWindowPos(NULL, rectClient.left + static_width + 120, rectClient.top, static_width + 50, 30, SWP_NOACTIVATE | SWP_NOZORDER);

	rectClient.top += 30 + 3;
	//m_staticFontSize.SetWindowPos(NULL, rectClient.left, rectClient.top, static_width, 30, SWP_NOACTIVATE | SWP_NOZORDER);
	m_editScale.SetWindowPos(NULL, rectClient.left + static_width + 3, rectClient.top, static_width, 30, SWP_NOACTIVATE | SWP_NOZORDER);

	//rectClient.top += 30 + 3;
	//m_btnOk.SetWindowPos(NULL, rectClient.left + 30 + 3, rectClient.top, static_width, 30, SWP_NOACTIVATE | SWP_NOZORDER);
	m_btnOk.SetWindowPos(NULL, rectClient.left + static_width + static_width + 15, rectClient.top, static_width, 30, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CGlyphView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CGlyphView::MyPaint(CPaintDC &dc)
{
}

void CGlyphView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

    CRect rect;
    GetClientRect(&rect);

    CBrush bg(RGB(255,255,255));
    dc.FillRect(&rect, &bg);

	CRect rectClient(rect);
	rectClient.left += 3;
	rectClient.top += 8;

    Gdiplus::Graphics g(dc.m_hDC);
    
    Gdiplus::GraphicsContainer gContainer = g.BeginContainer();

    Gdiplus::Font textFont(L"Arial", 10, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush textBrush(Gdiplus::Color(0,0,0));
    g.DrawString(L"Glyph Id:", 9, &textFont, Gdiplus::PointF((float)rectClient.left,(float)rectClient.top), &textBrush);
	rectClient.top += 33;
    g.DrawString(L"Scale:", 6, &textFont, Gdiplus::PointF((float)rectClient.left, (float)rectClient.top), &textBrush);
	rectClient.top += 33;
    g.DrawString(L"Glyph by FontFactory:", 21, &textFont, Gdiplus::PointF((float)rectClient.left, (float)rectClient.top), &textBrush);

    g.SetTransform(&m_matGlyph);

    Gdiplus::Pen gridPen(Gdiplus::Color(200,200,200), 0.0f);
    g.DrawRectangle(&gridPen, m_rectFaceBBox.left, m_rectFaceBBox.top,  m_rectFaceBBox.Width(), m_rectFaceBBox.Height());
    g.DrawLine(&gridPen, m_rectFaceBBox.left, 0, m_rectFaceBBox.right, 0); // y-axis
    g.DrawLine(&gridPen, 0, m_rectFaceBBox.top, 0, m_rectFaceBBox.bottom); // x-axis

	// draw glyph
    if(m_pOutlines && m_pOutlines->numContours > 0)
    {
        Gdiplus::Pen gPen(Gdiplus::Color(255, 0, 0, 255), 0);
        FF_Font_Factory::CGlyphOutlines *pPointOutlines = FF_Font_Factory::CBaseFont::ConvertOutlinesToPoints((FF_Font_Factory::CGlyphOutlines*)m_pOutlines,(float)m_nScale);
        int count = 0;
        FF_Font_Factory::CGlyphCoordinate prevPoint;
        for(int i = 0; i< pPointOutlines->numContours; i++)
        {
            int first = true;
            for(unsigned int j = count; pPointOutlines->pEndPtsOfContours[i]!= 0xFFFF && j <= pPointOutlines->pEndPtsOfContours[i]; j++)
            {
                if(first)
                    first = false;
                else
                    g.DrawLine(&gPen, prevPoint.x, prevPoint.y, pPointOutlines->pCoordinates[j].x, pPointOutlines->pCoordinates[j].y);
                prevPoint.x = pPointOutlines->pCoordinates[j].x;
                prevPoint.y = pPointOutlines->pCoordinates[j].y;
            }
            count = pPointOutlines->pEndPtsOfContours[i] + 1;
        }
        delete pPointOutlines;
    }

    g.EndContainer(gContainer);


    Gdiplus::PointF zeroPts[4];
    zeroPts[0] = Gdiplus::PointF((float)m_rectFaceBBox.left, 0); // y-axis
    zeroPts[1] = Gdiplus::PointF(0, (float)m_rectFaceBBox.top);  // x-axis
    zeroPts[2] = Gdiplus::PointF(m_rectFaceBBox.left + m_fLsb, 0);  // lsb
    zeroPts[3] = Gdiplus::PointF(m_rectFaceBBox.left + m_fRsb, 0);  // rsb
    m_matGlyph.TransformPoints(zeroPts, 4);

    Gdiplus::Font unitFont(L"Arial", 7.0f, Gdiplus::FontStyleItalic, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush unitBrush(Gdiplus::Color(200,200,200));
    g.DrawString(L"0", 1, &unitFont, Gdiplus::PointF(zeroPts[0].X - 12, zeroPts[0].Y - 6), &unitBrush);
    g.DrawString(L"0", 1, &unitFont, Gdiplus::PointF(zeroPts[1].X - 6, zeroPts[1].Y), &unitBrush);
    //g.DrawString(L"lsb", 3, &unitFont, Gdiplus::PointF(zeroPts[2].X - 12, zeroPts[2].Y - 6), &unitBrush);
    //g.DrawString(L"rsb", 3, &unitFont, Gdiplus::PointF(zeroPts[3].X - 6, zeroPts[3].Y), &unitBrush);

    // Mark point number on glyph
    if (m_bMarkPoint && m_pOutlines)
    {
        CString cs;
        Gdiplus::SolidBrush unitBrush1(Gdiplus::Color(170, 170, 170));
        for (size_t i = 0; i < m_pOutlines->numPoints; i++)
        {
            cs.Format(L"%d", i);
            zeroPts[0].X = m_pOutlines->pCoordinates[i].x;
            zeroPts[0].Y = m_pOutlines->pCoordinates[i].y;
            m_matGlyph.TransformPoints(zeroPts, 1);
            DrawPoint(g, &unitFont, &unitBrush1, zeroPts[0].X, zeroPts[0].Y, cs);
        }
    }
}

void CGlyphView::DrawPoint(Gdiplus::Graphics &g, Gdiplus::Font *pFont, Gdiplus::SolidBrush *pBrush, float x, float y, CString &text)
{
    g.DrawString(text, text.GetLength(), pFont, Gdiplus::PointF(x - 7, y - 16), pBrush);
    Gdiplus::RectF rect(x - 3, y - 3, 7, 7);
    g.FillEllipse(pBrush, rect);
}

void CGlyphView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_editGlyphId.SetFocus();
}

void CGlyphView::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    GetClientRect(&m_rectGlyph);
    m_rectGlyph.top = 50;

    AdjustLayout();
}

void CGlyphView::OnResize()
{
    SetScale();

    CString cs;
    cs.Format(L"%.2f", m_nScale);
    m_editScale.SetWindowTextW(cs);
    //m_editScale.GetWindowTextW(cs);
    //m_nScale = _wtoi(cs);

    m_editGlyphId.GetWindowTextW(cs);
    int id = _wtoi(cs);
    if (id != m_nGlyphId && id != -1)
    {
        // request new glyph
        m_hGlyph = ((CMainFrame*)AfxGetMainWnd())->GetGlyph(id);
        if (m_hGlyph)
            m_pOutlines = FE_GetOutlines(m_hGlyph);
    }

    CalculatMatrix();

    Invalidate();
}

void CGlyphView::SetScale()
{
    GetClientRect(&m_rectGlyph);
    m_rectGlyph.top = 50;
    m_rectGlyph.bottom -= 90;
    m_rectGlyph.left = 10;
    m_rectGlyph.right -= 30;
    float ratiox = m_rectFaceBBox.Width() == 0 ? 1 : 1.0f * m_rectGlyph.Width() / m_rectFaceBBox.Width();
    float ratioy = m_rectFaceBBox.Height() == 0 ? 1 : 1.0f * m_rectGlyph.Height() / m_rectFaceBBox.Height();
    if (ratiox < ratioy) m_nScale = ratiox;
    else m_nScale = ratioy;
}

void CGlyphView::OnGlyphViewShow()
{
    OnResize();
}

void CGlyphView::OnUpdateGlyphViewShow(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CGlyphView::SetFontInfo(FE_Font hFont, FE_FontFace hFontFace, int nFontSize)
{
    m_hFont = hFont;
    m_hFontFace = hFontFace;
    FE_Rect bbox;
    FE_GetFaceBBox(m_hFontFace, bbox);
    m_rectFaceBBox.left = (LONG)bbox.left;
    m_rectFaceBBox.right = (LONG)bbox.right;
    m_rectFaceBBox.top = (LONG)bbox.top;
    m_rectFaceBBox.bottom = (LONG)bbox.bottom;

    SetScale();

    m_pOutlines = 0;

    CalculatMatrix();

    Invalidate();

}

void CGlyphView::CalculatMatrix()
{
    m_matGlyph.SetElements(1, 0, 0, -1, 0, (float)m_rectFaceBBox.Height());
    m_matGlyph.Scale((float)m_nScale, (float)m_nScale, Gdiplus::MatrixOrderAppend);

    m_ptLeftBottom = Gdiplus::PointF((float)m_rectFaceBBox.left, (float)m_rectFaceBBox.bottom);
    m_matGlyph.TransformPoints(&m_ptLeftBottom, 1);
    m_matGlyph.Translate((float)m_nEmX - m_ptLeftBottom.X, (float)m_nEmY - m_ptLeftBottom.Y, Gdiplus::MatrixOrderAppend);

    //Gdiplus::PointF pts[2];
    //pts[0].X = (float)m_rectFaceBBox.left-10; 
    //pts[0].Y = (float)m_rectFaceBBox.top -10; 
    //pts[1].X = (float)m_rectFaceBBox.right + 10; 
    //pts[1].Y = (float)m_rectFaceBBox.bottom + 10; 
    //m_matGlyph.TransformPoints(pts, 2);

    //m_rectGlyph.SetRect((int)pts[0].X, (int)pts[0].Y, (int)pts[1].X, (int)pts[1].Y);
}

void CGlyphView::ShowGlyph(int debug, void *pCellInfo)
{
    m_pCellInfo = (CCharCell*)pCellInfo;
    m_nGlyphId = m_pCellInfo->m_nIndex;
	CString cs;
	cs.Format(L"%d", m_nGlyphId);
	m_editGlyphId.SetWindowTextW(cs);

    m_hGlyph = m_pCellInfo->m_hGlyph;
    //if (debug == 0)
    {
        if (m_hGlyph)
        {
            m_rectOutline.SetRectEmpty();
            m_pOutlines = FE_GetOutlines(m_hGlyph);
            if (m_pOutlines)
            {
                for (int i = 0; i< m_pOutlines->numContours; i++)
                {
                    for (unsigned int j = 0; m_pOutlines->pEndPtsOfContours[i] != 0xFFFF && j <= m_pOutlines->pEndPtsOfContours[i]; j++)
                    {
                        if (m_pOutlines->pCoordinates[j].x > m_rectOutline.right) m_rectOutline.right = (LONG)m_pOutlines->pCoordinates[j].x;
                        if (m_pOutlines->pCoordinates[j].x < m_rectOutline.left) m_rectOutline.left = (LONG)m_pOutlines->pCoordinates[j].x;
                        if (m_pOutlines->pCoordinates[j].y > m_rectOutline.bottom) m_rectOutline.bottom = (LONG)m_pOutlines->pCoordinates[j].y;
                        if (m_pOutlines->pCoordinates[j].y < m_rectOutline.top) m_rectOutline.top = (LONG)m_pOutlines->pCoordinates[j].y;
                    }
                }
            }
        }
        m_fLsb = FE_GetGlyphLsb(m_hGlyph);
        m_fRsb = FE_GetGlyphRsb(m_hGlyph);
        float aw = FE_GetGlyphAdvanceWidth(m_hGlyph);

        FE_Rect rc;
        FE_GetGlyphBBox(m_hGlyph, rc);

        InvalidateRect(m_rectGlyph);
        InvalidateRect(m_rectWinGlyph);
    }
}

void CGlyphView::GetGlyphInfo(FE_Font &hFont, FE_FontFace &hFontFace, FE_Glyph &hGlyph, int &nGlyphId)
{
    hFont = m_hFont;
    hFontFace = m_hFontFace;
    hGlyph = m_hGlyph;
    nGlyphId = m_nGlyphId;
}



void CGlyphView::OnGlyphViewMarkPoints()
{
    m_bMarkPoint = !m_bMarkPoint;
    m_cbMarkPoint.SetCheck(m_bMarkPoint ? BST_CHECKED : BST_UNCHECKED);
    InvalidateRect(m_rectGlyph);
}


void CGlyphView::OnUpdateGlyphViewMarkPoints(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(true);
}
