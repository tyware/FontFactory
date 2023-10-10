#pragma once
#include "FontFace.h"
#include <vector>
#include <string>
#include <map>

#pragma warning( disable : 4251 )

namespace FF_Font_Factory
{
    extern char* Wchar2Char(const wchar_t* src, bool bFreeSrc);
    extern wchar_t* Char2Wchar(const char* src, bool bFreeSrc);

    template class __declspec(dllexport) std::vector<CFontFace*>;

    typedef enum
    {
        PF_OFFCURVE    = 0,
        PF_ONCURVE     = 1,
        PF_TOUCHED_X   = 2,
        PF_TOUCHED_Y   = 4,
        PF_HELP = 8
    }POINT_FLAGS;

    class __declspec(dllexport) CGlyphCoordinate
    {
    public:
        CGlyphCoordinate() : bOnCurve(PF_OFFCURVE), x(0), y(0){};
        CGlyphCoordinate(unsigned int flag, float _x, float _y) : bOnCurve(flag), x(_x), y(_y){};
        void operator=(const CGlyphCoordinate &s)
        {
            bOnCurve = s.bOnCurve;
            x = s.x;
            y = s.y;
        }
        BOOL operator!=(const CGlyphCoordinate &s)
        {
            return(x != s.x || y != s.y);
        }
        BOOL operator==(const CGlyphCoordinate &s)
        {
            return(x == s.x && y == s.y);
        }

    public:
        unsigned int bOnCurve;
        float x, y;
    };

    class __declspec(dllexport) CGlyphBmap
    {
    public:
        CGlyphBmap() : width(0), height(0), bwidth(0), zeroX(0), zeroY(0), data(0) {};
        ~CGlyphBmap()
        {
            if(data) delete[] data;
        }

    public:
        int width, height, bwidth;
        float zeroX, zeroY;
        char *data;
    };

    class __declspec(dllexport) CGlyphOutlines
    {
    public:
        CGlyphOutlines():pEndPtsOfContours(0), pCoordinates(0){};
        ~CGlyphOutlines()
        {
            if(pEndPtsOfContours)
                delete[] pEndPtsOfContours;
            if(pCoordinates)
                delete[] pCoordinates;
        };
        CGlyphOutlines *Copy(int extraElems = 0)
        {
            CGlyphOutlines *pOutlines = new CGlyphOutlines();
            pOutlines->numContours = numContours;
            if(pOutlines->numContours > 0)
            {
                pOutlines->pEndPtsOfContours = new unsigned int[pOutlines->numContours];
                memcpy(pOutlines->pEndPtsOfContours, pEndPtsOfContours, pOutlines->numContours * sizeof(unsigned int));
            }
            pOutlines->numPoints = numPoints;
            if(pOutlines->numPoints > 0)
            {
                pOutlines->pCoordinates = new CGlyphCoordinate[pOutlines->numPoints + extraElems];
                memcpy(pOutlines->pCoordinates, pCoordinates, pOutlines->numPoints * sizeof(CGlyphCoordinate));
            }

            return pOutlines;
        }

    public:
        int numContours;
        unsigned int *pEndPtsOfContours;
        int numPoints;
        CGlyphCoordinate *pCoordinates; // number of coordinates is the last value of pEndPtsOfContours + 1
    } ;

    class __declspec(dllexport) CGlyphDebugInfo
    {
    public:
        class CNameValue
        {
        public:
            std::wstring name;
            std::wstring value;
        };
    public:
        CGlyphDebugInfo() : cid(-1), pOutlines(0), pOrigOutlines(0), pData(0), pData1(0), pData2(0), currentPos(0), bRefillInstructions(true)
            , bEnd(false), nCvt(0), pCvt(0), intructionSet(0), nTwilights(0), pTwilights(0), curSubGlyph(0)
        {
        };
        ~CGlyphDebugInfo()
        {
            for (size_t i = 0; i < stateVariables.size(); i++)
                delete stateVariables[i];
            stateVariables.clear();
            if (pOutlines) delete pOutlines;
            instructions.clear();
            instrMap.clear();
            storage.clear();
            subglyphs.clear();
        };

        int cid;
        int intructionSet; // font, face, or glyph
        std::vector<CNameValue*> stateVariables;
        std::vector<UINT32> stack;
        std::vector<UINT32> storage;
        CGlyphOutlines *pOutlines;
        CGlyphOutlines *pOrigOutlines;
        UINT32 nTwilights;
        CGlyphCoordinate *pTwilights;
        UINT32 nCvt;
        INT32 *pCvt;
        std::vector<std::wstring> instructions;
        std::map<int, int> instrMap;
        int currentPos;
        bool bRefillInstructions;
        bool bEnd;
        std::vector<int> subglyphs;
        int curSubGlyph;
        void *pData; // font special data
        void *pData1; // font special data
        void *pData2; // font special data
    };


    class __declspec(dllexport) CBaseFont
    {
    public:
        CBaseFont();
        virtual ~CBaseFont();

        static CBaseFont* Create(FONT_TYPE fontType);
        static CGlyphBmap* GenerateGlyphBmap(const float lsb, const CFeRect &bbox, const CGlyphOutlines *pOutlines);

        // override functions
        virtual BOOL LoadFontFile(wchar_t *fileName);
        virtual BOOL LoadFont(char *buffer, int buflen);
        virtual CFontFace* CreateFontFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
        virtual void DeleteFontFace(CFontFace* pFace);
        virtual unsigned short GetUpem(){return 0; };
        virtual wchar_t* GetFontName() {return m_pFileName;};

        virtual FE_CMap GetCMap(int &platformId, int &encodingId) { return 0; };
        virtual BOOL IndexedByCid() { return true; };
        virtual int GetFirstCharacter(FE_CMap pCMap) { return 0; };
        virtual int GetNextCharacter(FE_CMap pCMap) { return 0; };
        virtual int GetGlyphIndex(FE_CMap pCMap, unsigned int cid) { return 0; };
        virtual int GetFirstGlyph() { return 0; };
        virtual int GetNextGlyph() { return 0; };
        virtual CGlyphOutlines* GetOutlines(int glyphIndex) { return 0; };
        virtual CGlyphOutlines* GetOutlines(FE_CMap pCMap, unsigned int cid) { return 0; };
        //virtual CGlyphOutlines* Scaler(FE_FontFace hFace, int glyphIndex, int dpi, int pointSize, bool bConvertCurveToPoints ) { return 0; };
        static CGlyphOutlines* ConvertOutlinesToPoints(CGlyphOutlines *pRawOutlines, float scale = 1.0f);// { return 0; };
        virtual CGlyphOutlines* Scaler(FE_FontFace hFace, int glyphIndex) { return 0; };
        virtual int GetNumGlyphs(FE_CMap pCMap) { return 0; };
        virtual int GetMaxAdvanceWidth() { return 0; };
        virtual int GetNumCharacters(FE_CMap pCMap) { return 0; };
        virtual int GetAdvanceWidth(int glyphIndex) { return 0; };
        virtual int GetAdvanceWidth(FE_CMap pCMap, unsigned int cid) { return 0; };
        virtual int GetAdvanceHeight(int glyphIndex) { return 0; };
        virtual int GetAdvanceHeight(FE_CMap pCMap, unsigned int cid) { return 0; };
        virtual int GetLineGap(int glyphIndex) { return 0; };
        virtual int GetLineGap(FE_CMap pCMap, unsigned int cid) { return 0; };
        virtual int GetLeftSideBearing(int glyphIndex) { return 0; };
        virtual int GetLeftSideBearing(FE_CMap pCMap, unsigned int cid) { return 0; };
        virtual int GetRightSideBearing(int glyphIndex) { return 0; };
        virtual int GetRightSideBearing(FE_CMap pCMap, unsigned int cid) { return 0; };
        virtual BOOL GetFontBBox(CFeRect &rect) { return 0; };
        virtual BOOL GetGlyphBBox(FE_CMap pCMap, unsigned int cid, CFeRect &rect) { return 0; };
        virtual BOOL GetGlyphBBox(int glyphIndex, CFeRect &rect) { return 0; };
        CFontFace* FindFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId) { return 0; };
        virtual CFeGlyph* GetGlyph(FE_FontFace face, unsigned int cid) { return 0; };
        virtual CFeGlyph* GetGlyphByIndex(FE_FontFace face, int glyphIndex) { return 0; };

        virtual FE_DebugInfo GlyphDebugStart(FE_FontFace face, int cid, int set) { return 0; };
        virtual BOOL GlyphDebugContinue(FE_FontFace face, FE_DebugInfo debugInfo) { return TRUE; };
        virtual BOOL GlyphDebugStepOver(FE_FontFace face, FE_DebugInfo debugInfo) { return TRUE; };
        virtual BOOL GlyphDebugStepIn(FE_FontFace face, FE_DebugInfo debugInfo) { return TRUE; };
        virtual BOOL GlyphDebugStepOut(FE_FontFace face, FE_DebugInfo debugInfo) { return TRUE; };
        virtual BOOL GlyphDebugEnd(FE_FontFace face, FE_DebugInfo debugInfo) { return TRUE; };
        virtual BOOL GlyphDebugSetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos) { return TRUE; };
        virtual BOOL GlyphDebugDelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos) { return TRUE; };
        virtual void* GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo) { return 0; };
        virtual BOOL GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void *breakpoints) { return TRUE; };
        virtual BOOL GlyphDebugIsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos) { return TRUE; };


        float GetScale(int dpi, int pointSize) { return 1.0f * Calculate_ppem(dpi, pointSize) / GetUpem();};
        short Calculate_ppem(int dpi, int pointSize) { return (short)(pointSize * dpi / 72.0f); };


    public:
        size_t ReadBytes(char *buffer, size_t count);
        int Seek(long _Offset, int _Origin);
        long Tell();
        size_t GetNumFaces();
        CFontFace* GetFace(size_t index);
        void AppendFace(CFontFace *pFace);

    public:
        FONT_TYPE m_nFontType;
        wchar_t *m_pFileName;
        char *m_pBuffer;
        size_t m_nLength;
        size_t m_nOffset;
        wchar_t *m_pFontName;
        int m_nRefCount;
        std::vector<CFontFace*> m_arrayFaces;

    protected:
        int m_nCurGlyphIndex;
    };

};
