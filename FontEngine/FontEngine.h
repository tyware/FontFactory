// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FONTENGINE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FONTENGINE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "FeHeader.h"
#include <vector>
#include "BaseFont.h"
#include "ApiHeader.h"

extern FONTENGINE_API int g_FE_Error;


namespace FF_Font_Factory
{
    // This class is exported from the FontEngine.dll
    class FONTENGINE_API CFontEngine {
    public:
	    CFontEngine(void);
        ~CFontEngine();


        void Init();
        FE_Font LoadFontFile(wchar_t *fileName);
        FE_Font LoadFontFile(wchar_t *fileName, FONT_TYPE fontType);
        FE_Font LoadFontFile(char *fileName);
        FE_Font LoadFontFile(char *fileName, FONT_TYPE fontType);
        FE_Font LoadFont(char *buffer, int buflen, FONT_TYPE fontType);
        void UnloadFont(FE_Font font);
        void UnloadFont(wchar_t *fontName);
        void GetFontName(FE_Font hFont, wchar_t **fontName);
        FE_FontFace CreateFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
        FE_FontFace CreateFontFace(wchar_t *fontName, int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
        void DeleteFontFace(FE_FontFace face);

        //FE_CMap GetCMap(FE_Font hFont, int &platformId, int &encodingId);
        void FE_GetFaceBBox(FE_FontFace face, FE_Rect &bbox);
        int GetNumCharacters(FE_FontFace face);
        int GetFirstCharacter(FE_FontFace face);
        int GetNextCharacter(FE_FontFace face);
        int GetFirstGlyph(FE_FontFace face);
        int GetNextGlyph(FE_FontFace face);
        int GetMaxAdvanceWidth(FE_FontFace face);
        FE_Outlines* GetGlyphOutlines(FE_FontFace face, int cid);
        FE_Outlines* GetGlyphOutlines(FE_Glyph glyph);
        FE_Glyph GetGlyph(FE_FontFace face, int cid);
        FE_Glyph GetGlyphByIndex(FE_FontFace face, int glyphIndex);

        FE_DebugInfo GlyphDebugStart(FE_FontFace face, int cid, int set);
        BOOL GlyphDebugContinue(FE_FontFace face, FE_DebugInfo debugInfo);
        BOOL GlyphDebugStepOver(FE_FontFace face, FE_DebugInfo debugInfo);
        BOOL GlyphDebugStepIn(FE_FontFace face, FE_DebugInfo debugInfo);
        BOOL GlyphDebugStepOut(FE_FontFace face, FE_DebugInfo debugInfo);
        BOOL GlyphDebugEnd(FE_FontFace face, FE_DebugInfo debugInfo);
        BOOL GlyphDebugSetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);
        BOOL GlyphDebugDelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);
        void* GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo);
        BOOL GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void *breakpoints);
        BOOL GlyphDebugIsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);

        void GetGlyphBBox(FE_Glyph glyph, FE_Rect &rect);
        FE_Bitmap* FE_GetBmap(FE_Glyph glyph);
        float GetGlyphLsb(FE_Glyph glyph);
        float GetGlyphRsb(FE_Glyph glyph);
        float GetGlyphAdvanceWidth(FE_Glyph glyph);

    public:
        std::vector<CBaseFont*> m_arrayFonts;

    private:
        FONT_TYPE GetFontTypeFromFileName(wchar_t *fileName);
        CBaseFont *FindFont(wchar_t *pFontName);
    };

};



//FONTENGINE_API int fnFontEngine(void);
