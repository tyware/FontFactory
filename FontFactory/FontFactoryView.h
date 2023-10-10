
// FontFactoryView.h : interface of the CFontFactoryView class
//

#pragma once


class CFontFactoryView : public CScrollView
{
protected: // create from serialization only
	CFontFactoryView();
	DECLARE_DYNCREATE(CFontFactoryView)

// Attributes
public:
	CFontFactoryDoc* GetDocument() const;

    bool m_bInitialized;

    void *m_pCMap;
    int m_nNumCharacters;
    int m_nMaxAdvanceWidth;
    float m_fScale;
    int m_nWidth;
    int m_nHeight;
    int m_nCols;
    int m_nRows;

	int m_hlCol;
	int m_hlRow;

// Operations
public:
    void CalculateTable();
    void DrawOutlines(Gdiplus::Graphics &g, Gdiplus::Pen &pen, void *outlines, int x, int y);
    void DrawGlyphBmap(Gdiplus::Graphics &g, int x, int y, void *pBmp);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CFontFactoryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in FontFactoryView.cpp
inline CFontFactoryDoc* CFontFactoryView::GetDocument() const
   { return reinterpret_cast<CFontFactoryDoc*>(m_pDocument); }
#endif

