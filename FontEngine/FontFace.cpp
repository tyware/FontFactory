#include "stdafx.h"
#include "FontFace.h"
#include "BaseFont.h"
#include "Instruction.h"

extern int g_FE_Platform;

void Delete_Outlines(FE_Outlines *pOutlines)
{
    if(pOutlines)
    {
        if(pOutlines->numContours > 0 && pOutlines->pEndPtsOfContours) delete[] pOutlines->pEndPtsOfContours;
        if(pOutlines->numPoints > 0 && pOutlines->pCoordinates) delete[] pOutlines->pCoordinates;
        delete pOutlines;
    }
}

void Delete_Bitmap(FE_Bitmap *pBmap)
{
    if(pBmap)
    {
        if(pBmap->data) delete[] pBmap->data;
        delete pBmap;
    }
}

namespace FF_Font_Factory
{
    ///////////////////////////////////////////////
    // CFeGlyph
    ///////////////////////////////////////////////
    CFeGlyph::CFeGlyph()
        :m_lsb(0), m_rsb(0), m_advanceWidth(0), outlines(0), bmap(0)
    {
    }
    CFeGlyph::~CFeGlyph()
    {
        if(outlines) Delete_Outlines(outlines);
        if(bmap) Delete_Bitmap(bmap);
    }

    FE_Bitmap* CFeGlyph::GetBmap()
    {
        if(bmap) return bmap;
        bmap = (FE_Bitmap*)CBaseFont::GenerateGlyphBmap(m_lsb, m_bbox, (FF_Font_Factory::CGlyphOutlines*)outlines);
        return bmap;
    }


    ///////////////////////////////////////////////
    // CFontFace
    ///////////////////////////////////////////////
    CFontFace::CFontFace(void)
    {
        m_pFont = 0;
        m_DPI = 72;
        m_fontSize = 9;
        m_flags = 0;
        m_fScale = 1.0f;
        m_numGlyphs = 0;
        m_nRefCount = 0;

        if(g_FE_Platform == FE_PLATFORM_ID_Windows)
            m_nEncodingId = FE_WIN_ENCODING_UCS2; // default to UCS2 (wchar_t) for Windows
        else
            m_nEncodingId = FE_UCS_ENCODING_Default; // default to UCS2 (wchar_t) for Unicode

        m_pCMap = NULL;
    }


    CFontFace::~CFontFace(void)
    {
        DeleteGlyphs();
    }

    CFontFace::CFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags)
    {
        m_pFont = (CBaseFont*)hFont;
        m_DPI = dpi;
        m_fontSize = fontSize;
        m_flags = flags;
        m_nRefCount = 0;

        m_unitsPerEM = m_pFont->GetUpem();
        m_fScale = fontSize * dpi / 72.0f / m_unitsPerEM;

        m_pFont->GetFontBBox(m_bbox);
        m_bbox.m_left *= m_fScale;
        m_bbox.m_right *= m_fScale;
        m_bbox.m_top *= m_fScale;
        m_bbox.m_bottom *= m_fScale;
 
        m_nPlatformId = g_FE_Platform;

        if(m_nPlatformId == FE_PLATFORM_ID_Windows)
            m_nEncodingId = FE_WIN_ENCODING_UCS2; // default to UCS2 (wchar_t) for Windows
        else
            m_nEncodingId = FE_UCS_ENCODING_Default; // default to UCS2 (wchar_t) for Unicode

        m_pCMap = m_pFont->GetCMap(m_nPlatformId, m_nEncodingId);

        m_numGlyphs = m_pFont->GetNumGlyphs(m_pCMap);

    }

    CFontFace::CFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
    {
        m_pFont = (CBaseFont*)hFont;
        m_DPI = dpi;
        m_fontSize = fontSize;
        m_flags = flags;
        m_nRefCount = 0;

        m_unitsPerEM = m_pFont->GetUpem();
        m_fScale = fontSize * dpi / 72.0f / m_unitsPerEM;

        m_pFont->GetFontBBox(m_bbox);
        m_bbox.m_left *= m_fScale;
        m_bbox.m_right *= m_fScale;
        m_bbox.m_top *= m_fScale;
        m_bbox.m_bottom *= m_fScale;
 
        m_nPlatformId = platformId;
        m_nEncodingId = encodingId;

        m_pCMap = m_pFont->GetCMap(m_nPlatformId, m_nEncodingId);

        m_numGlyphs = m_pFont->GetNumGlyphs(m_pCMap);
    }

    void CFontFace::DeleteGlyphs()
    {
        std::map<int, CFeGlyph*>::iterator it;
        if(m_arrayGid2Glyphs.size() > 0)
        {
            for(it = m_arrayGid2Glyphs.begin(); it!= m_arrayGid2Glyphs.end(); it++)
            {
                std::map<int, CFeGlyph*>::iterator iti;
                for(iti = m_arrayGlyphs.begin(); iti != m_arrayGlyphs.end(); iti++)
                {
                    if(it->second == iti->second)
                    {
                        m_arrayGlyphs.erase(iti);
                        break;
                    }
                }
                delete it->second;
            }
            m_arrayGid2Glyphs.clear();
        }
        if(m_arrayGlyphs.size() > 0)
        {
            for(it = m_arrayGlyphs.begin(); it!= m_arrayGlyphs.end(); it++)
                delete it->second;
            m_arrayGlyphs.clear();
        }
    }

    int CFontFace::GetNumGlyphs()
    {
        return m_numGlyphs;
    }

    int CFontFace::GetGlyphIndex(int cid)
    {
        if(m_pCMap)
            return m_pFont->GetGlyphIndex(m_pCMap, cid);
        else
            return -1;
    }

    CFeGlyph* CFontFace::GetGlyphByIndex(int glyphIndex)
    {
        if (glyphIndex == -1) return NULL;
        
        std::map<int, CFeGlyph*>::iterator it = m_arrayGid2Glyphs.find(glyphIndex);
        if (it != m_arrayGlyphs.end())
            return it->second;

        CFeGlyph *pGlyph = new CFeGlyph();
        m_pFont->GetGlyphBBox(glyphIndex, pGlyph->m_bbox);
        pGlyph->m_bbox.SetLeft(pGlyph->m_bbox.GetLeft() * m_fScale);
        pGlyph->m_bbox.SetRight(pGlyph->m_bbox.GetRight() * m_fScale);
        pGlyph->m_bbox.SetTop(pGlyph->m_bbox.GetTop() * m_fScale);
        pGlyph->m_bbox.SetBottom(pGlyph->m_bbox.GetBottom() * m_fScale);
        if (m_pCMap)
        {
            pGlyph->m_lsb = (m_pFont->GetLeftSideBearing(glyphIndex) * m_fScale);
            pGlyph->m_advanceWidth = (m_pFont->GetAdvanceWidth(glyphIndex) * m_fScale);
        }
        else
        {
            pGlyph->m_lsb = 0;
            pGlyph->m_advanceWidth = 0;
        }
        pGlyph->m_rsb = (pGlyph->m_advanceWidth - pGlyph->m_lsb - pGlyph->m_bbox.GetWidth());
        CGlyphOutlines *pOutlines = m_pFont->Scaler(this, glyphIndex);
        pGlyph->bmap = NULL;//(FE_Bitmap*)m_pFont->GenerateGlyphBmap(0, pGlyph->m_bbox, pOutlines);
        pGlyph->outlines = (FE_Outlines*)pOutlines;

        m_arrayGid2Glyphs.insert(std::pair<int, CFeGlyph*>(glyphIndex, pGlyph));

        return pGlyph;

    }

    CFeGlyph* CFontFace::GetGlyph(int cid)
    {
        std::map<int, CFeGlyph*>::iterator it = m_arrayGlyphs.find(cid);
        if(it != m_arrayGlyphs.end())
            return it->second;

        int glyphIndex = GetGlyphIndex(cid);
        if(glyphIndex == -1) return NULL;

        CFeGlyph *pGlyph = new CFeGlyph();
        m_pFont->GetGlyphBBox(glyphIndex, pGlyph->m_bbox);
        pGlyph->m_bbox.SetLeft(pGlyph->m_bbox.GetLeft() * m_fScale);
        pGlyph->m_bbox.SetRight(pGlyph->m_bbox.GetRight() * m_fScale);
        pGlyph->m_bbox.SetTop(pGlyph->m_bbox.GetTop() * m_fScale);
        pGlyph->m_bbox.SetBottom(pGlyph->m_bbox.GetBottom() * m_fScale);
        if(m_pCMap)
        {
            pGlyph->m_lsb = (m_pFont->GetLeftSideBearing(m_pCMap, cid) * m_fScale);
            pGlyph->m_advanceWidth = (m_pFont->GetAdvanceWidth(m_pCMap, cid) * m_fScale);
        }
        else
        {
            pGlyph->m_lsb = 0;
            pGlyph->m_advanceWidth = 0;
        }
        pGlyph->m_rsb = (pGlyph->m_advanceWidth - pGlyph->m_lsb - pGlyph->m_bbox.GetWidth());
        CGlyphOutlines *pOutlines = m_pFont->Scaler(this, glyphIndex);
        pGlyph->bmap = NULL;//(FE_Bitmap*)m_pFont->GenerateGlyphBmap(0, pGlyph->m_bbox, pOutlines);
        pGlyph->outlines = (FE_Outlines*)pOutlines;

        m_arrayGlyphs.insert(std::pair<int, CFeGlyph*>(cid, pGlyph));

        return pGlyph;
    }

    FE_Outlines* CFontFace::GetGlyphOutlines(int cid)
    {
        CFeGlyph* pGlyph = GetGlyph(cid);
        if(pGlyph == NULL) return NULL;

        return pGlyph->outlines;
    }

    //void* CFontFace::GetGlyphArray()
    //{
    //    return &m_arrayGlyphs;
    //}


};

