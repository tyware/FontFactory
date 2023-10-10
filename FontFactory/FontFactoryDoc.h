
// FontFactoryDoc.h : interface of the CFontFactoryDoc class
//

#pragma once

#include "BaseFont.h"
#include "FeApi.h"
#include <vector>

class CCharCell
{
public:
    CCharCell() : m_nIndex(-1), m_hGlyph(0) { };
    ~CCharCell()
    {
        //if(m_hGlyph)
        //    delete m_hGlyph;
    };

public:
    int m_nIndex;
    FE_Glyph m_hGlyph;
};

class CFontFactoryDoc : public CDocument
{
protected: // create from serialization only
	CFontFactoryDoc();
	DECLARE_DYNCREATE(CFontFactoryDoc)

// Attributes
public:
    CString m_csFileName;
    FE_Font m_hFont;
    FE_FontFace m_hFontFace;
    FONT_TYPE m_nFontType;

    int m_nPlatformId;
    int m_nEncodingId;
    int m_nDPI;
    int m_nFontSize;
    int m_nCharNum;
	int m_nNumChars;
    bool m_bDrawOutlineOnly;
    bool m_bDebug;
    //FF_Font_Factory::CBaseFont *m_pFont;

    std::vector<CCharCell*> m_pCells;

// Operations
public:
    void Clear();
    BOOL ReloadFont(LPCTSTR lpszPathName);
    FE_Glyph GetGlyph(int cid);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CFontFactoryDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    //BOOL OpenFile(LPCTSTR lpszPathName, FF_Font_Factory::FONT_TYPE fontType = FF_Font_Factory::FT_TRUETYPE );

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
