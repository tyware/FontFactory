// FontEngine.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FontEngine.h"


FONTENGINE_API int g_FE_Error = 0;

FF_Font_Factory::CFontEngine *g_pFE_FontEngine = NULL;

#ifdef WIN32
    int g_FE_Platform           = FE_PLATFORM_ID_Windows;
    int g_FE_PlatformEncodingId = FE_WIN_ENCODING_UCS2;
#else
    int g_FE_Platform           = FE_PLATFORM_ID_Unicode;
    int g_FE_PlatformEncodingId = FE_UCS_ENCODING_Default;
#endif


//// This is an example of an exported function.
//FONTENGINE_API int fnFontEngine(void)
//{
//	return 42;
//}

namespace FF_Font_Factory
{

    // This is the constructor of a class that has been exported.
    // see FontEngine.h for the class definition
    CFontEngine::CFontEngine()
    {
	    return;
    }

    CFontEngine::~CFontEngine()
    {
        size_t size = m_arrayFonts.size();
        for(int i=0; i<size; i++)
            delete m_arrayFonts[i];
        m_arrayFonts.clear();
    }

    void CFontEngine::Init()
    {
    }

    CBaseFont* CFontEngine::FindFont(wchar_t *pFontName)
    {
        for(size_t i = 0; i<m_arrayFonts.size(); i++)
        {
            if(wcscmp(pFontName, m_arrayFonts[i]->GetFontName()) == 0)
                return m_arrayFonts[i];
        }
        return NULL;
    }

    FE_Font CFontEngine::LoadFontFile(wchar_t *fileName, FONT_TYPE fontType)
    {
        CBaseFont* pBaseFont = CBaseFont::Create(fontType);
        if(pBaseFont == NULL) return NULL;

        if(pBaseFont->LoadFontFile(fileName) == false)
        {
            //g_FE_Error = FE_ERROR_Load_File_Fail;
            delete pBaseFont;
            return NULL;
        }

        wchar_t *pFontName = pBaseFont->GetFontName();
        CBaseFont *pFont = FindFont(pFontName);
        if(pFont)
        {
            delete pBaseFont;
            pFont->m_nRefCount++;
            return pFont;
        }

        m_arrayFonts.push_back(pBaseFont);

        return pBaseFont;
    }

    FE_Font CFontEngine::LoadFontFile(wchar_t *fileName)
    {
        FONT_TYPE fontType = GetFontTypeFromFileName(fileName);
        if(fontType == FT_TYPE_NONE) return NULL;

        return LoadFontFile(fileName, fontType);
    }

    FE_Font CFontEngine::LoadFontFile(char *fileName, FONT_TYPE fontType)
    {
        wchar_t *pFileName = Char2Wchar(fileName, false);

        FE_Font hFont = LoadFontFile(pFileName, fontType);
        delete[] pFileName;

        return hFont;
    }

    FE_Font CFontEngine::LoadFontFile(char *fileName)
    {
        wchar_t *pFileName = Char2Wchar(fileName, false);

        FE_Font hFont = LoadFontFile(pFileName);
        delete[] pFileName;

        return hFont;
    }

    FE_Font CFontEngine::LoadFont(char *buffer, int buflen, FONT_TYPE fontType)
    {
        CBaseFont* pBaseFont = CBaseFont::Create(fontType);
        if(pBaseFont == NULL) return NULL;

        if(pBaseFont->LoadFont(buffer, buflen) == false)
        {
            //g_FE_Error = FE_ERROR_Load_File_Fail;
            delete pBaseFont;
            return NULL;
        }

        wchar_t *pFontName = pBaseFont->GetFontName();
        CBaseFont *pFont = FindFont(pFontName);
        if(pFont)
        {
            delete pBaseFont;
            pFont->m_nRefCount++;
            return pFont;
        }

        m_arrayFonts.push_back(pBaseFont);

        return pBaseFont;
    }

    FONT_TYPE CFontEngine::GetFontTypeFromFileName(wchar_t *fileName)
    {
        size_t len = wcslen(fileName) - 4;
        if(len <= 0) return FT_TYPE_NONE;
        wchar_t *ext = &fileName[len];
        if(wcscmp(ext, L".ttf") == 0) // true type font
            return FT_TRUETYPE;
        else
            return FT_TYPE_NONE;
    }

    void CFontEngine::UnloadFont(FE_Font font)
    {
        if(font)
        {
            CBaseFont *pFont = (CBaseFont*)font;
            pFont->m_nRefCount--;
            if(pFont->m_nRefCount <= 0)
            {
                for(size_t i = 0; i<m_arrayFonts.size(); i++)
                {
                    if(pFont == m_arrayFonts[i])
                        m_arrayFonts.erase(m_arrayFonts.begin() + i);
                }
                delete pFont;
            }
        }
    }

    void CFontEngine::UnloadFont(wchar_t *fontName)
    {
        CBaseFont *pFont = FindFont(fontName);
        if(pFont)
            UnloadFont((FE_Font)pFont);
    }

    void CFontEngine::GetFontName(FE_Font hFont, wchar_t **fontName)
    {
        if(fontName) *fontName = nullptr;
        if(!hFont) return;

        if(fontName)
            *fontName = ((CBaseFont*)hFont)->GetFontName();
    }

    FE_FontFace CFontEngine::CreateFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
    {
        if(hFont)
        {
            CBaseFont *pFont = (CBaseFont*)hFont;
            CFontFace *pFontFace = pFont->CreateFontFace(fontSize, dpi, flags, platformId, encodingId);
            return pFontFace;
        }

        return NULL;
    }
    FE_FontFace CFontEngine::CreateFontFace(wchar_t *fontName, int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
    {
        CBaseFont *pFont = FindFont(fontName);
        if(pFont)
        {
            CFontFace *pFontFace = pFont->CreateFontFace(fontSize, dpi, flags, platformId, encodingId);
            //pFont->m_arrayFaces.push_back(pFontFace);
            pFont->AppendFace(pFontFace);
            return pFontFace;
        }

        return NULL;
    }

    void CFontEngine::DeleteFontFace(FE_FontFace face)
    {
        if(face)
        {
            CFontFace *pFace = (CFontFace*)face;
            ((CBaseFont*)pFace->m_pFont)->DeleteFontFace(pFace);
        }
    }

    //FE_CMap CFontEngine::GetCMap(FE_Font hFont, int &platformId, int &encodingId)
    //{
    //    if(hFont == NULL) return NULL;

    //    return ((CBaseFont*)hFont)->GetCMap(platformId, encodingId);
    //}
    void CFontEngine::FE_GetFaceBBox(FE_FontFace face, FE_Rect &bbox)
    {
        CFontFace *pFace = (CFontFace*)face;
        bbox.left =pFace->m_bbox.m_left;
        bbox.right =pFace->m_bbox.m_right;
        bbox.top =pFace->m_bbox.m_top;
        bbox.bottom =pFace->m_bbox.m_bottom;
    }
    int CFontEngine::GetNumCharacters(FE_FontFace face)
    {
        return ((CFontFace*)face)->m_pFont->GetNumCharacters(((CFontFace*)face)->m_pCMap);
    }
    int CFontEngine::GetFirstCharacter(FE_FontFace face)
    {
        return ((CFontFace*)face)->m_pFont->GetFirstCharacter(((CFontFace*)face)->m_pCMap);
    }
    int CFontEngine::GetNextCharacter(FE_FontFace face)
    {
        return ((CFontFace*)face)->m_pFont->GetNextCharacter(((CFontFace*)face)->m_pCMap);
    }
    int CFontEngine::GetFirstGlyph(FE_FontFace face)
    {
        return ((CFontFace*)face)->m_pFont->GetFirstGlyph();
    }
    int CFontEngine::GetNextGlyph(FE_FontFace face)
    {
        return ((CFontFace*)face)->m_pFont->GetNextGlyph();
    }
    int CFontEngine::GetMaxAdvanceWidth(FE_FontFace face)
    {
        return (int)(((CFontFace*)face)->m_pFont->GetMaxAdvanceWidth() * ((CFontFace*)face)->m_fScale);
    }

    FE_Glyph CFontEngine::GetGlyph(FE_FontFace face, int cid)
    {
        //return ((CFontFace*)face)->GetGlyph(cid);
        return ((CFontFace*)face)->m_pFont->GetGlyph(face, cid);
    }
    FE_Glyph CFontEngine::GetGlyphByIndex(FE_FontFace face, int glyphIndex)
    {
        return ((CFontFace*)face)->m_pFont->GetGlyphByIndex(face, glyphIndex);
    }

    FE_DebugInfo CFontEngine::GlyphDebugStart(FE_FontFace face, int cid, int set)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugStart(face, cid, set);
    }
    BOOL CFontEngine::GlyphDebugContinue(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugContinue(face, debugInfo);
    }
    BOOL CFontEngine::GlyphDebugStepOver(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugStepOver(face, debugInfo);
    }
    BOOL CFontEngine::GlyphDebugStepIn(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugStepIn(face, debugInfo);
    }
    BOOL CFontEngine::GlyphDebugStepOut(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugStepOut(face, debugInfo);
    }
    BOOL CFontEngine::GlyphDebugEnd(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugEnd(face, debugInfo);
    }
    BOOL CFontEngine::GlyphDebugSetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugSetBreakPoint(face, debugInfo, pos);
    }
    BOOL CFontEngine::GlyphDebugDelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugDelBreakPoint(face, debugInfo, pos);
    }
    BOOL CFontEngine::GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void *breakpoints)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugSetBreakPoints(face, debugInfo, breakpoints);
    }
    void* CFontEngine::GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugCopyBreakPoints(face, debugInfo);
    }
    BOOL CFontEngine::GlyphDebugIsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
    {
        return ((CFontFace*)face)->m_pFont->GlyphDebugIsBreakPoint(face, debugInfo, pos);
    }

    void CFontEngine::GetGlyphBBox(FE_Glyph glyph, FE_Rect &rect)
    {
        CFeGlyph *pGlyph = (CFeGlyph*)glyph;
        //rect.left = (int)pGlyph->m_bbox.m_left;
        //rect.top = (int)pGlyph->m_bbox.m_top;
        //rect.right = (int)pGlyph->m_bbox.m_right;
        //rect.bottom = (int)pGlyph->m_bbox.m_bottom;
        rect.left = pGlyph->m_bbox.m_left;
        rect.top = pGlyph->m_bbox.m_top;
        rect.right = pGlyph->m_bbox.m_right;
        rect.bottom = pGlyph->m_bbox.m_bottom;
    }

    float CFontEngine::GetGlyphLsb(FE_Glyph glyph)
    {
        return ((CFeGlyph*)glyph)->m_lsb;
    }
    float CFontEngine::GetGlyphRsb(FE_Glyph glyph)
    {
        return ((CFeGlyph*)glyph)->m_rsb;
    }
    float CFontEngine::GetGlyphAdvanceWidth(FE_Glyph glyph)
    {
        return ((CFeGlyph*)glyph)->m_advanceWidth;
    }

    FE_Outlines* CFontEngine::GetGlyphOutlines(FE_Glyph glyph)
    {
        return ((CFeGlyph*)glyph)->outlines;
    }
    FE_Outlines* CFontEngine::GetGlyphOutlines(FE_FontFace face, int cid)
    {
        return ((CFontFace*)face)->GetGlyphOutlines(cid);
    }

    FE_Bitmap* CFontEngine::FE_GetBmap(FE_Glyph glyph)
    {
        return ((CFeGlyph*)glyph)->GetBmap();
    }
};



