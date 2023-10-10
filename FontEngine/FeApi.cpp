#include "stdafx.h"
#include "FeApi.h"
#include "FontEngine.h"
#include "ApiHeader.h"
#include "FontFace.h"

extern FF_Font_Factory::CFontEngine *g_pFE_FontEngine;
extern int g_FE_Error;

extern void Delete_Outlines(FE_Outlines *pOutlines);
extern void Delete_Bitmap(FE_Bitmap *pBmap);


#ifdef __cplusplus
extern "C"
{
#endif

    DLL_SO_EXPORT int FE_Init_FontEngine()
    {
        g_FE_Error = FE_ERROR_None;

        if(g_pFE_FontEngine == NULL)
            g_pFE_FontEngine = new FF_Font_Factory::CFontEngine();
        
        g_pFE_FontEngine->Init();

        return g_FE_Error;
    }

    DLL_SO_EXPORT int FE_Close_FontEngine()
    {
        if(g_pFE_FontEngine)
        {
            delete g_pFE_FontEngine;
            g_pFE_FontEngine = NULL;
        }
        g_FE_Error = FE_ERROR_None;

        return g_FE_Error;
    }

    DLL_SO_EXPORT int FE_GetError()
    {
        return g_FE_Error;
    }

    DLL_SO_EXPORT FE_Font FE_LoadFontFromFile(wchar_t *fileName)
    {
        return g_pFE_FontEngine->LoadFontFile(fileName);
    }

    DLL_SO_EXPORT FE_Font FE_LoadFontFromBuffer(char* buffer, size_t len, FONT_TYPE fontType)
    {
        return g_pFE_FontEngine->LoadFont(buffer, (int)len, fontType);
    }

    DLL_SO_EXPORT void FE_UnloadFont(FE_Font hFont)
    {
        g_pFE_FontEngine->UnloadFont(hFont);
    }

    DLL_SO_EXPORT void FE_UnloadFontByName(wchar_t *fontName)
    {
        g_pFE_FontEngine->UnloadFont(fontName);
    }

    DLL_SO_EXPORT void FE_GetFontName(FE_Font hFont, wchar_t **fontName)
    {
        g_pFE_FontEngine->GetFontName(hFont, fontName);
    }


    DLL_SO_EXPORT FE_FontFace FE_CreateFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
    {
        return g_pFE_FontEngine->CreateFontFace(hFont, fontSize, dpi, flags, platformId, encodingId);
    }

    DLL_SO_EXPORT FE_FontFace FE_CreateFontFaceByName(wchar_t *fontName, int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
    {
        return g_pFE_FontEngine->CreateFontFace(fontName, fontSize, dpi, flags, platformId, encodingId);
    }

    DLL_SO_EXPORT void FE_DeleteFontFace(FE_FontFace face)
    {
        g_pFE_FontEngine->DeleteFontFace(face);
    }

    //DLL_SO_EXPORT FE_CMap GetCMap(FE_Font hFont, int &platformId, int &encodingId)
    //{
    //    g_pFE_FontEngine->GetCMap(hFont, platformId, encodingId);
    //}

    DLL_SO_EXPORT void FE_GetFaceBBox(FE_FontFace face, FE_Rect &bbox)
    {
        if(face)  g_pFE_FontEngine->FE_GetFaceBBox(face, bbox);
    }

    DLL_SO_EXPORT int FE_GetNumCharacters(FE_FontFace face)
    {
        return g_pFE_FontEngine->GetNumCharacters(face);
    }

    DLL_SO_EXPORT int FE_GetFirstCharacter(FE_FontFace face)
    {
        return g_pFE_FontEngine->GetFirstCharacter(face);
    }

    DLL_SO_EXPORT int FE_GetNextCharacter(FE_FontFace face)
    {
        return g_pFE_FontEngine->GetNextCharacter(face);
    }
    DLL_SO_EXPORT int FE_GetFirstGlyph(FE_FontFace face)
    {
        return g_pFE_FontEngine->GetFirstGlyph(face);
    }

    DLL_SO_EXPORT int FE_GetNextGlyph(FE_FontFace face)
    {
        return g_pFE_FontEngine->GetNextGlyph(face);
    }

    DLL_SO_EXPORT int FE_GetMaxAdvanceWidth(FE_FontFace face)
    {
        return g_pFE_FontEngine->GetMaxAdvanceWidth(face);
    }

    DLL_SO_EXPORT FE_Glyph FE_GetGlyph(FE_FontFace face, int cid)
    {
        return g_pFE_FontEngine->GetGlyph(face, cid);
    }
    DLL_SO_EXPORT FE_Glyph FE_GetGlyphByIndex(FE_FontFace face, int glyphIndex)
    {
        return g_pFE_FontEngine->GetGlyphByIndex(face, glyphIndex);
    }
    DLL_SO_EXPORT FE_DebugInfo FE_Glyph_Debug_Start(FE_FontFace face, int cid, int set)
    {
        return g_pFE_FontEngine->GlyphDebugStart(face, cid, set);
    }
    DLL_SO_EXPORT FE_DebugInfo FE_Glyph_Debug_Restart(FE_FontFace face, int cid, FE_DebugInfo debugInfo)
    {
        int set = ((FF_Font_Factory::CGlyphDebugInfo*)debugInfo)->intructionSet;
        void *breakpoints = FE_GlyphDebugCopyBreakPoints(face, debugInfo);
        FE_Glyph_Debug_End(face, debugInfo);
        FE_DebugInfo dinfo = FE_Glyph_Debug_Start(face, cid, set);
        FE_GlyphDebugSetBreakPoints(face, dinfo, breakpoints);
        return dinfo;
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_Continue(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return g_pFE_FontEngine->GlyphDebugContinue(face, debugInfo);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_StepOver(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return g_pFE_FontEngine->GlyphDebugStepOver(face, debugInfo);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_StepIn(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return g_pFE_FontEngine->GlyphDebugStepIn(face, debugInfo);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_StepOut(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return g_pFE_FontEngine->GlyphDebugStepOut(face, debugInfo);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_End(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return g_pFE_FontEngine->GlyphDebugEnd(face, debugInfo);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_SetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
    {
        return g_pFE_FontEngine->GlyphDebugSetBreakPoint(face, debugInfo, pos);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_DelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
    {
        return g_pFE_FontEngine->GlyphDebugDelBreakPoint(face, debugInfo, pos);
    }
    DLL_SO_EXPORT void* FE_GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo)
    {
        return g_pFE_FontEngine->GlyphDebugCopyBreakPoints(face, debugInfo);
    }
    DLL_SO_EXPORT BOOL FE_GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void *breakpoints)
    {
        return g_pFE_FontEngine->GlyphDebugSetBreakPoints(face, debugInfo, breakpoints);
    }
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_IsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
    {
        return g_pFE_FontEngine->GlyphDebugIsBreakPoint(face, debugInfo, pos);
    }

    DLL_SO_EXPORT void FE_GetGlyphBBox(FE_Glyph glyph, FE_Rect &rect)
    {
        g_pFE_FontEngine->GetGlyphBBox(glyph, rect);
    }
    DLL_SO_EXPORT float FE_GetGlyphLsb(FE_Glyph glyph)
    {
        return g_pFE_FontEngine->GetGlyphLsb(glyph);
    }
    DLL_SO_EXPORT float FE_GetGlyphRsb(FE_Glyph glyph)
    {
        return g_pFE_FontEngine->GetGlyphRsb(glyph);
    }
    DLL_SO_EXPORT float FE_GetGlyphAdvanceWidth(FE_Glyph glyph)
    {
        return g_pFE_FontEngine->GetGlyphAdvanceWidth(glyph);
    }
    DLL_SO_EXPORT FE_Outlines* FE_GetGlyphOutlines(FE_FontFace face, int cid)
    {
        return g_pFE_FontEngine->GetGlyphOutlines(face, cid);
    }
    DLL_SO_EXPORT FE_Outlines* FE_GetOutlines(FE_Glyph glyph)
    {
        return g_pFE_FontEngine->GetGlyphOutlines(glyph);
    }

    DLL_SO_EXPORT void FE_Delete_Outlines(FE_Outlines *pOutlines)
    {
        Delete_Outlines(pOutlines);
    }

    DLL_SO_EXPORT FE_Bitmap* FE_GetBmap(FE_Glyph glyph)
    {
        return g_pFE_FontEngine->FE_GetBmap(glyph);
    }

    DLL_SO_EXPORT void FE_Delete_Bitmap(FE_Bitmap *pBmap)
    {
        Delete_Bitmap(pBmap);
    }

#ifdef __cplusplus
};
#endif
