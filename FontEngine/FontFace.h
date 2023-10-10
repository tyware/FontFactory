#pragma once
//#pragma warning(disable:4251)

#include "Rect.h"
#include "ApiHeader.h"
#include <map>

namespace FF_Font_Factory
{

    class __declspec(dllexport) CBaseFont;
    class __declspec(dllexport) CGlyphOutlines;
    class __declspec(dllexport) CGlyphBmap;

    class __declspec(dllexport) CFeGlyph
    {
    public:
        CFeGlyph();
        ~CFeGlyph();
        FE_Bitmap* GetBmap();

    public:
        CFeRect m_bbox;
        float m_lsb; // left side bearing
        float m_rsb; // right side bearing = advanceWidth - left side bearing - bbox.width
        float m_advanceWidth; // m_advanceWidth = lsb + (xMax - xMin) + rsb

        FE_Outlines *outlines;
        FE_Bitmap *bmap;
    };

    template class __declspec(dllexport) std::map<int, CFeGlyph*>;

    class __declspec(dllexport) CFontFace
    {
    public:
        CFontFace(void);
        CFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags);
        CFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
        virtual ~CFontFace(void);

        int GetNumGlyphs();
        FE_Outlines* GetGlyphOutlines(int cid);
        void DeleteGlyphs();
        CFeGlyph* GetGlyph(int cid);
        CFeGlyph* GetGlyphByIndex(int glyphIndex);
        int GetGlyphIndex(int cid);

    public:
        //void* GetGlyphArray();

    public:
        int m_DPI;
        int m_fontSize;
        BYTE m_flags;
        int m_nPlatformId;
        int m_nEncodingId;
        FE_CMap m_pCMap;

        float m_fScale; // : fontSize * desired dpi / 72.0f / m_unitsPerEM;
        unsigned int m_unitsPerEM;
        CFeRect m_bbox; // face bbox
        int m_numGlyphs;

    public:
        CBaseFont *m_pFont;
        std::map<int, CFeGlyph*> m_arrayGlyphs; // map cid to CFeGlyph*
        std::map<int, CFeGlyph*> m_arrayGid2Glyphs; // map gid to CFeGlyph*
        int m_nRefCount;

    private:

    };

};

