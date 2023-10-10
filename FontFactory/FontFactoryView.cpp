
// FontFactoryView.cpp : implementation of the CFontFactoryView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FontFactory.h"
#endif

#include "FontFactoryDoc.h"
#include "FontFactoryView.h"
//#include "BaseFont.h"
#include "NonZeroWinding.h"
#include "BezierCurve.h"
//#include "ApiHeader.h"

#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFontFactoryView

IMPLEMENT_DYNCREATE(CFontFactoryView, CScrollView)

BEGIN_MESSAGE_MAP(CFontFactoryView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFontFactoryView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
    ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CFontFactoryView construction/destruction

CFontFactoryView::CFontFactoryView()
{
	// TODO: add construction code here
	m_hlCol = 0;
	m_hlRow = 0;
    m_bInitialized = false;
}

CFontFactoryView::~CFontFactoryView()
{
}

BOOL CFontFactoryView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CFontFactoryView drawing
#define MARGIN                 10
#define MARGIN_INSIDE          2
#define HEADER_HEIGHT          20

void CFontFactoryView::OnDraw(CDC* pDC)
{
	CFontFactoryDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    //int dpi = pDC->GetDeviceCaps(LOGPIXELSX);

	// TODO: add draw code for native data here
    if(pDoc->m_hFont == NULL || pDoc->m_hFontFace == NULL) return;

    CRect rect;
    GetClientRect(&rect);

    CPen pen(PS_SOLID, 1, RGB(0,0,255));
    CPen *pOldPen = pDC->SelectObject(&pen);

    // draw table cells
    for(int i = 0; i<m_nRows; i++)
    {
        pDC->MoveTo(MARGIN, MARGIN + i * m_nHeight);
        pDC->LineTo(MARGIN + m_nWidth * m_nCols, MARGIN + i * m_nHeight);
        pDC->MoveTo(MARGIN, MARGIN + i * m_nHeight + HEADER_HEIGHT);
        pDC->LineTo(MARGIN + m_nWidth * m_nCols, MARGIN + i * m_nHeight + HEADER_HEIGHT);
    }
    pDC->MoveTo(MARGIN, MARGIN + m_nRows * m_nHeight);
    pDC->LineTo(MARGIN + m_nWidth * m_nCols, MARGIN + m_nRows * m_nHeight);
    for(int i = 0; i<m_nCols; i++)
    {
        pDC->MoveTo(MARGIN + i * m_nWidth, MARGIN);
        pDC->LineTo(MARGIN + i * m_nWidth, MARGIN + m_nRows * m_nHeight);
    }
    pDC->MoveTo(MARGIN + m_nCols * m_nWidth, MARGIN);
    pDC->LineTo(MARGIN + m_nCols * m_nWidth, MARGIN + m_nRows * m_nHeight);

    pDC->SelectObject(pOldPen);

    Gdiplus::Graphics g(pDC->m_hDC);
    Gdiplus::Font font(_T("Arial"), 10.0f, 0, Gdiplus::UnitPoint);
    Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
    Gdiplus::Pen gPen(Gdiplus::Color(255, 0, 0, 255), 0);

    CString cs;
    int nRow = 0;
    int nCol = 0;
    int x = 0, y = 0;
    int count = 0;
    bool bDrawOutline = pDoc->m_bDrawOutlineOnly;
    bool bByGlyph = false;

    for(size_t i = 0; i < pDoc->m_pCells.size(); i++)
    {
        //if (i != 1415) continue;
        cs.Format(_T("%d(0x%04X)"), pDoc->m_pCells[i]->m_nIndex, pDoc->m_pCells[i]->m_nIndex);
        g.DrawString(cs, cs.GetLength(), &font, Gdiplus::PointF((float)(MARGIN + nCol * m_nWidth/* + m_nWidth / 3*/), (float)(nRow * m_nHeight + MARGIN + MARGIN_INSIDE)), &brush); 

        //if (!pDoc->m_bDebug)
        {
            FE_Outlines *pOutline = FE_GetOutlines(pDoc->m_pCells[i]->m_hGlyph);
            if (pOutline && pOutline->numContours > 0)// && count++ == 3)
            {
                if (bDrawOutline)
                {
                    DrawOutlines(g, gPen, pOutline, nCol * m_nWidth + m_nWidth / 2 - m_nWidth / 5, nRow * m_nHeight + HEADER_HEIGHT + MARGIN + m_nWidth / 2);
                }
                else
                {
                    FE_Bitmap *pBmap = FE_GetBmap(pDoc->m_pCells[i]->m_hGlyph);
                    DrawGlyphBmap(g, nCol * m_nWidth + m_nWidth / 2, nRow * m_nHeight + HEADER_HEIGHT + MARGIN + m_nWidth / 2, pBmap);
                }
            }
        }
        nCol++;
        if(nCol >= m_nCols)
        {
            nCol = 0;
            nRow++;
        }
    }
	// draw highlight cell
	Gdiplus::Pen hHlPen(Gdiplus::Color(255, 0, 0), 2);
	g.DrawRectangle(&hHlPen, MARGIN + m_hlCol * m_nWidth, MARGIN + m_hlRow * (m_nWidth + HEADER_HEIGHT), m_nWidth, m_nWidth + HEADER_HEIGHT);
    return;

}

void CFontFactoryView::DrawGlyphBmap(Gdiplus::Graphics &g, int x, int y, void *pBmp)
{
    if(pBmp == NULL) return;

    FE_Bitmap *pBmap = (FE_Bitmap*)pBmp;

    Gdiplus::Bitmap gdiBmap(pBmap->width, pBmap->height, -pBmap->bwidth, PixelFormat8bppIndexed, (BYTE*)(pBmap->data + pBmap->bwidth * (pBmap->height - 1)));
    g.DrawImage(&gdiBmap, (float)x + pBmap->zeroX + 1, (float)y - pBmap->height - pBmap->zeroY + 1);
}

void CFontFactoryView::DrawOutlines(Gdiplus::Graphics &g, Gdiplus::Pen &pen, void *outlines, int x, int y)
{
    //Gdiplus::GraphicsContainer gContainer = g.BeginContainer();
    //Gdiplus::Matrix mat;//(1, 0, 0, -1, 0, m_nMaxAdvanceWidth);
    //Gdiplus::Matrix mat(1, 0, 0, 1, 0, m_nMaxAdvanceWidth * scale / 2);
    //g.SetTransform(&mat);

    FF_Font_Factory::CGlyphOutlines *pPointOutlines = FF_Font_Factory::CBaseFont::ConvertOutlinesToPoints((FF_Font_Factory::CGlyphOutlines*)outlines);
    if (pPointOutlines)
    {
        int count = 0;
        FF_Font_Factory::CGlyphCoordinate prevPoint;
        for (int i = 0; i < pPointOutlines->numContours; i++)
        {
            int first = true;
            //if(i == 1)
            for (unsigned int j = count; pPointOutlines->pEndPtsOfContours[i] != 0xFFFF && j <= pPointOutlines->pEndPtsOfContours[i]; j++)
            {
                if (first)
                    first = false;
                else
                    g.DrawLine(&pen, prevPoint.x + x, -prevPoint.y + y, pPointOutlines->pCoordinates[j].x + x, -pPointOutlines->pCoordinates[j].y + y);
                prevPoint.x = pPointOutlines->pCoordinates[j].x;
                prevPoint.y = pPointOutlines->pCoordinates[j].y;
            }
            count = pPointOutlines->pEndPtsOfContours[i] + 1;
        }
        delete pPointOutlines;
    }

    //g.EndContainer(gContainer);
}

void CFontFactoryView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

    m_bInitialized = true;

 //   CRect rc;
 //   GetClientRect(&rc);

	//CSize sizeTotal;
	//// TODO: calculate the total size of this view
 //   sizeTotal.cx = (LONG)rc.Width() + 1;
 //   sizeTotal.cy = (LONG)rc.Height() + 1;
	//SetScrollSizes(MM_TEXT, sizeTotal);

}


// CFontFactoryView printing


void CFontFactoryView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CFontFactoryView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFontFactoryView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFontFactoryView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFontFactoryView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFontFactoryView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFontFactoryView diagnostics

#ifdef _DEBUG
void CFontFactoryView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CFontFactoryView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CFontFactoryDoc* CFontFactoryView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFontFactoryDoc)));
	return (CFontFactoryDoc*)m_pDocument;
}
#endif //_DEBUG


// CFontFactoryView message handlers


void CFontFactoryView::OnSize(UINT nType, int cx, int cy)
{
    CScrollView::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if (m_bInitialized)
        AfxGetMainWnd()->PostMessageW(WM_GLYPH_VIEW_RESIZE, 0, 0);

    CalculateTable();

}

void CFontFactoryView::CalculateTable()
{
    CRect rc;
    GetClientRect(&rc);

    CFontFactoryDoc *pDoc = GetDocument();
    if(pDoc->m_hFontFace)
    {
        m_pCMap = (void*)1;//pDoc->m_pFont->GetCMap(pDoc->m_nPlatformId, pDoc->m_nEncodingId);
        if(m_pCMap == NULL) goto RESET_CALCULATE;
        m_nNumCharacters = FE_GetNumCharacters(pDoc->m_hFontFace);//pDoc->m_pFont->GetNumCharacters(m_pCMap);
        m_nMaxAdvanceWidth = FE_GetMaxAdvanceWidth(pDoc->m_hFontFace);
        m_fScale = 1.0f;//pDoc->m_pFont->GetScale(pDoc->m_nDPI, pDoc->m_nFontSize);
        m_nWidth =  (int)(m_nMaxAdvanceWidth) + MARGIN * 2;
        {
            CDC *pDC = GetDC();
            Gdiplus::Graphics g(pDC->m_hDC);
            Gdiplus::Font font(_T("Arial"), 10.0f, 0, Gdiplus::UnitPoint);
            Gdiplus::RectF boundingBox;
            g.MeasureString(_T("00000(0x0000)"), 13, &font, Gdiplus::PointF(0.0f,0.0f), &boundingBox);
            if(m_nWidth < (int)boundingBox.Width) m_nWidth = (int)boundingBox.Width;
        }
        m_nHeight = m_nWidth + HEADER_HEIGHT;// + MARGIN;
        m_nCols = (rc.Width() - MARGIN * 2) / m_nWidth;
        m_nRows = m_nNumCharacters / m_nCols;
        if(m_nCols * m_nRows < m_nNumCharacters) m_nRows++;
    }
    else
    {
RESET_CALCULATE:
        m_pCMap = NULL;
        m_nNumCharacters = 1;
        m_nMaxAdvanceWidth = rc.Width() + 1;
        m_fScale = 1;
        m_nWidth =  m_nMaxAdvanceWidth;
        m_nHeight = rc.Height() + 1;
        m_nCols = 1;
        m_nRows = 1;
    }

    CSize sizeTotal;
    sizeTotal.cx = MARGIN + m_nCols * m_nWidth + MARGIN;
    sizeTotal.cy = MARGIN + m_nRows * m_nHeight + MARGIN;

    SetScrollSizes(MM_TEXT, sizeTotal);
}



void CFontFactoryView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CFontFactoryDoc *pDoc = GetDocument();

	int scrollPos = GetScrollPos(SB_VERT);
	int oldHlRow = m_hlRow;
	int oldHlCol = m_hlCol;
	m_hlRow = (point.y + scrollPos - MARGIN) / (HEADER_HEIGHT + m_nWidth);
	m_hlCol = (point.x - MARGIN) / m_nWidth;
	int charIndex = m_hlRow * m_nCols + m_hlCol;
	if(point.x < MARGIN || point.y + scrollPos < MARGIN || m_hlCol >= m_nCols || m_hlRow >= m_nRows || charIndex >= pDoc->m_nNumChars)
	{
		m_hlRow = oldHlRow;
		m_hlCol = oldHlCol;
		CScrollView::OnLButtonDown(nFlags, point);
		return;
	}
	
	CRect rect;
	// clear old highlight
	rect.left = MARGIN + oldHlCol * m_nWidth - 2;
	rect.top =  MARGIN + oldHlRow * (m_nWidth + HEADER_HEIGHT) - scrollPos - 2;
	rect.right = rect.left + m_nWidth + 4;
	rect.bottom = rect.top + m_nWidth + HEADER_HEIGHT + 4;
	InvalidateRect(rect);
	// draw new highlight
	rect.left = MARGIN + m_hlCol * m_nWidth - 2;
	rect.top =  MARGIN + m_hlRow * (m_nWidth + HEADER_HEIGHT) - scrollPos - 2;
	rect.right = rect.left + m_nWidth + 4;
	rect.bottom = rect.top + m_nWidth + HEADER_HEIGHT + 4;
	InvalidateRect(rect);

    if(pDoc->m_bDebug) AfxGetMainWnd()->PostMessageW(WM_GLYPH_VIEW_CHARACTER_CHANGE, 1, (LPARAM)pDoc->m_pCells[charIndex]);
    else AfxGetMainWnd()->PostMessageW(WM_GLYPH_VIEW_CHARACTER_CHANGE, 0, (LPARAM)pDoc->m_pCells[charIndex]);

	CScrollView::OnLButtonDown(nFlags, point);
}
