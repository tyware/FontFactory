
// FontFactoryDoc.cpp : implementation of the CFontFactoryDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FontFactory.h"
#endif

#include "FontFactoryDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFontFactoryDoc

IMPLEMENT_DYNCREATE(CFontFactoryDoc, CDocument)

BEGIN_MESSAGE_MAP(CFontFactoryDoc, CDocument)
END_MESSAGE_MAP()


// CFontFactoryDoc construction/destruction

CFontFactoryDoc::CFontFactoryDoc()
{
	// TODO: add one-time construction code here
    m_csFileName = L"";
    m_hFont = NULL;
    m_hFontFace = NULL;


   m_nFontSize = 36;
   m_nDPI = 72;// 96;
   m_bDrawOutlineOnly = true;
   m_nCharNum = -1;
}

CFontFactoryDoc::~CFontFactoryDoc()
{
    Clear();
}
void CFontFactoryDoc::Clear()
{
    //if(m_pFont) delete m_pFont;
    for(size_t i = 0; i<m_pCells.size(); i++)
    {
        delete m_pCells[i];
    }
    m_pCells.clear();
}

BOOL CFontFactoryDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CFontFactoryDoc serialization

void CFontFactoryDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CFontFactoryDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CFontFactoryDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CFontFactoryDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CFontFactoryDoc diagnostics

#ifdef _DEBUG
void CFontFactoryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFontFactoryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CFontFactoryDoc::ReloadFont(LPCTSTR lpszPathName)
{
    if(!m_csFileName.IsEmpty())
    {
        Clear();
        if(m_hFont) FE_UnloadFont(m_hFont);

        m_hFont = FE_LoadFontFromFile(m_csFileName.GetBuffer());
        if(m_hFont)
        {
            m_nPlatformId = 3;
            m_nEncodingId = 1;

            //pDoc->m_hFontFace = FE_CreateFontFace(pDoc->m_hFont, pDoc->m_nFontSize, pDoc->m_nDPI, 0, FE_PLATFORM_ID_Default, -1);
            m_hFontFace = FE_CreateFontFace(m_hFont, m_nFontSize, m_nDPI, 0, m_nPlatformId, m_nEncodingId);
            if (!m_hFontFace) return false;
            m_nNumChars = FE_GetNumCharacters(m_hFontFace);

            // load all glyphs
            FE_Glyph hGlyph;
            bool bByGlyph = false;
            int cid = FE_GetFirstCharacter(m_hFontFace);
            if(cid == -1) // no map code to character code, so character code is same as glyph indiex
            {
                bByGlyph = true;
                cid = FE_GetFirstGlyph(m_hFontFace);
            }
            while(cid != -1)
            {
                CCharCell *pCell = new CCharCell();
                pCell->m_nIndex = cid;
                //if (!m_bDebug)
                {
                    if (bByGlyph) hGlyph = FE_GetGlyphByIndex(m_hFontFace, cid);
                    else hGlyph = FE_GetGlyph(m_hFontFace, cid);
                    if (hGlyph)
                        pCell->m_hGlyph = hGlyph;
                }
                m_pCells.push_back(pCell);
                if (bByGlyph) cid = FE_GetNextGlyph(m_hFontFace);
                else cid = FE_GetNextCharacter(m_hFontFace);
            }

        }

        //wchar_t fontName[256];
        //FE_GetFontName(m_hFont, (wchar_t**)&fontName);

        return TRUE;
    }
    return FALSE;
}

// CFontFactoryDoc commands
BOOL CFontFactoryDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    if (!CDocument::OnOpenDocument(lpszPathName))
        return FALSE;

    // TODO:  Add your specialized creation code here
    return ReloadFont(lpszPathName);
}

FE_Glyph CFontFactoryDoc::GetGlyph(int cid)
{
    FE_Glyph hGlyph = FE_GetGlyph(m_hFontFace, cid);
    if(!hGlyph)
        hGlyph = FE_GetGlyphByIndex(m_hFontFace, cid);
	return hGlyph;
}
