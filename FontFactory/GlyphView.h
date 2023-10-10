#pragma once
#include "afxdockablepane.h"
#include "FeApi.h"
#include "FontFactoryDoc.h"

class CGlyphViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CGlyphView : public CDockablePane
{
// Construction
public:
	CGlyphView(void);
	virtual ~CGlyphView(void);

	void AdjustLayout();

    void SetFontInfo(FE_Font hFont, FE_FontFace hFontFace, int nFontSize);
	void ShowGlyph(int debug, void *pCellInfo);

    void GetGlyphInfo(FE_Font &hFont, FE_FontFace &hFontFace, FE_Glyph &hGlyph, int &nGlyphId);

    void DrawPoint(Gdiplus::Graphics &g, Gdiplus::Font *pFont, Gdiplus::SolidBrush *pBrush, float x, float y, CString &text);

    void SetScale();
    void OnResize();

// Attributes
protected:

	//CGlyphViewToolBar m_wndToolBar;

protected:
    //CStatic m_staticGlyphId;
    //CStatic m_staticFontSize;
	CEdit   m_editGlyphId;
	CEdit   m_editScale;
    CButton m_cbMarkPoint;
	CButton m_btnOk;
	int m_nGlyphId;
	float m_nScale;
    bool m_bMarkPoint;

    FE_Font m_hFont;
    FE_FontFace m_hFontFace;
    CRect m_rectFaceBBox;

    CCharCell *m_pCellInfo;
    FE_Glyph m_hGlyph;
    FE_Outlines *m_pOutlines;
    CRect m_rectOutline;

    CRect m_rectGlyph;
    CRect m_rectWinGlyph;

    int m_nEmX, m_nEmY;
    Gdiplus::PointF m_ptLeftBottom;
    Gdiplus::Matrix m_matGlyph;
    float m_fLsb, m_fRsb;

// Implementation
public:
    void MyPaint(CPaintDC &dc);
    void CalculatMatrix();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDummyCompile();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnGlyphViewShow();
	afx_msg void OnUpdateGlyphViewShow(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnGlyphViewMarkPoints();
    afx_msg void OnUpdateGlyphViewMarkPoints(CCmdUI *pCmdUI);
};

