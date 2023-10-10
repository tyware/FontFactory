#pragma once
#include "ApiHeader.h"

#define DLL_SO_EXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C"
{
#endif

    // Font Engine
    DLL_SO_EXPORT int FE_Init_FontEngine();
    DLL_SO_EXPORT int FE_Close_FontEngine();

    DLL_SO_EXPORT int FE_GetError();

    // Font
    DLL_SO_EXPORT FE_Font FE_LoadFontFromFile(wchar_t *fileName);
    DLL_SO_EXPORT FE_Font FE_LoadFontFromBuffer(char* buffer, size_t len, FONT_TYPE fontType);
    DLL_SO_EXPORT void FE_UnloadFont(FE_Font hFont);
    DLL_SO_EXPORT void FE_UnloadFontByName(wchar_t *fontName);

    DLL_SO_EXPORT void FE_GetFontName(FE_Font hFont, wchar_t **fontName);

    // Font Face
    DLL_SO_EXPORT FE_FontFace FE_CreateFontFace(FE_Font hFont, int fontSize, int dpi = 72, BYTE flags = 0, int platformId = FE_PLATFORM_ID_Default, int encodingId = -1);
    DLL_SO_EXPORT FE_FontFace FE_CreateFontFaceByName(wchar_t *fontName, int fontSize, int dpi = 72, BYTE flags = 0, int platformId = FE_PLATFORM_ID_Default, int encodingId = -1);
    DLL_SO_EXPORT void FE_DeleteFontFace(FE_FontFace face);
    //DLL_SO_EXPORT FE_CMap GetCMap(FE_Font hFont, int &platformId, int &encodingId);
    DLL_SO_EXPORT void FE_GetFaceBBox(FE_FontFace face, FE_Rect &bbox);


    DLL_SO_EXPORT int FE_GetNumCharacters(FE_FontFace face);
    DLL_SO_EXPORT int FE_GetFirstCharacter(FE_FontFace face);
    DLL_SO_EXPORT int FE_GetNextCharacter(FE_FontFace face);
    DLL_SO_EXPORT int FE_GetMaxAdvanceWidth(FE_FontFace face);
    DLL_SO_EXPORT int FE_GetFirstGlyph(FE_FontFace face);
    DLL_SO_EXPORT int FE_GetNextGlyph(FE_FontFace face);
    DLL_SO_EXPORT FE_Glyph FE_GetGlyph(FE_FontFace face, int cid);
    DLL_SO_EXPORT FE_Glyph FE_GetGlyphByIndex(FE_FontFace face, int glyphIndex);

    DLL_SO_EXPORT FE_DebugInfo FE_Glyph_Debug_Start(FE_FontFace face, int cid, int set);
    DLL_SO_EXPORT FE_DebugInfo FE_Glyph_Debug_Restart(FE_FontFace face, int cid, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_Continue(FE_FontFace face, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_StepOver(FE_FontFace face, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_StepIn(FE_FontFace face, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_StepOut(FE_FontFace face, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_End(FE_FontFace face, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_SetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_DelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);
    DLL_SO_EXPORT void* FE_GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo);
    DLL_SO_EXPORT BOOL FE_GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void *breakpoints);
    DLL_SO_EXPORT BOOL FE_Glyph_Debug_IsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);


    // Glyph
    DLL_SO_EXPORT void FE_GetGlyphBBox(FE_Glyph glyph, FE_Rect &rect);
    DLL_SO_EXPORT FE_Outlines* FE_GetOutlines(FE_Glyph glyph);
    DLL_SO_EXPORT FE_Outlines* FE_GetGlyphOutlines(FE_FontFace face, int cid);
    DLL_SO_EXPORT void FE_Delete_Outlines(FE_Outlines *pOutlines);
    DLL_SO_EXPORT FE_Bitmap* FE_GetBmap(FE_Glyph glyph);
    DLL_SO_EXPORT void FE_Delete_Bitmap(FE_Bitmap *pBmap);
    DLL_SO_EXPORT float FE_GetGlyphLsb(FE_Glyph glyph);
    DLL_SO_EXPORT float FE_GetGlyphRsb(FE_Glyph glyph);
    DLL_SO_EXPORT float FE_GetGlyphAdvanceWidth(FE_Glyph glyph);

#ifdef __cplusplus
};
#endif
