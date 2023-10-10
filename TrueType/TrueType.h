#pragma once
#include "BaseFont.h"
#include "ApiHeader.h"
#include "Rect.h"
#include "TTFontFace.h"
#include "TableMaxp.h"
#include "TableHead.h"
#include "TableName.h"
#include "TableHhea.h"
#include "TableHmtx.h"
#include "TableCMap.h"
#include "TableLoca.h"
#include "TableGlyf.h"
#include "TablePost.h"
#include "TableGasp.h"
#include "TableHdmx.h"
#include "TableKern.h"
#include "TableVhea.h"
#include "TableVmtx.h"
#include "TableOS2.h"
#include "Instruction.h"

namespace FF_Font_Factory
{
    namespace FF_Font_Factory_TrueType
    {

        class CTableEntry
        {
        public:
            CTableEntry() : tag(TAG_none), offset(0), length(0) {};
            TABLE_TAG tag;
            ULong offset;
            ULong length;
        };

        class CTrueTypeFont : public CBaseFont
        {
        public:
            CTrueTypeFont();
            ~CTrueTypeFont();

        public:
            virtual CTableEntry* FindTable(TABLE_TAG tag);
            virtual BOOL LoadFontFile(wchar_t *fileName);
            virtual CFontFace* CreateFontFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
            virtual BOOL IndexedByCid();
            virtual FE_CMap GetCMap(int &platformId, int &encodingId);
            virtual int GetFirstCharacter(FE_CMap pCMap);
            virtual int GetNextCharacter(FE_CMap pCMap);
            virtual int GetGlyphIndex(FE_CMap pCMap, unsigned int cid);
            virtual int GetFirstGlyph();
            virtual int GetNextGlyph();
            virtual CGlyphOutlines* GetOutlines(int glyphIndex);
            virtual CGlyphOutlines* GetOutlines(FE_CMap pCMap, unsigned int cid);
            //virtual CGlyphOutlines* Scaler(FE_FontFace hFace, int glyphIndex, int dpi, int pointSize, bool bConvertCurveToPoints );
            virtual wchar_t* GetFontName();
            //virtual CGlyphOutlines* ConvertOutlinesToPoints(CGlyphOutlines *pRawOutlines, float scale);
            virtual CGlyphOutlines* Scaler(FE_FontFace hFace, int glyphIndex);
            virtual unsigned short GetUpem();
            virtual int GetNumGlyphs(FE_CMap pCMap);
            virtual int GetMaxAdvanceWidth() ;
            virtual int GetNumCharacters(FE_CMap pCMap);
            virtual int GetAdvanceWidth(int glyphIndex);
            virtual int GetAdvanceWidth(FE_CMap pCMap, unsigned int cid);
            virtual int GetAdvanceHeight(int glyphIndex);
            virtual int GetAdvanceHeight(FE_CMap pCMap, unsigned int cid);
            virtual int GetLineGap(int glyphIndex);
            virtual int GetLineGap(FE_CMap pCMap, unsigned int cid);
            virtual int GetLeftSideBearing(int glyphIndex);
            virtual int GetLeftSideBearing(FE_CMap pCMap, unsigned int cid);
            virtual int GetRightSideBearing(int glyphIndex);
            virtual int GetRightSideBearing(FE_CMap pCMap, unsigned int cid);
            virtual BOOL GetFontBBox(CFeRect &rect);
            virtual BOOL GetGlyphBBox(FE_CMap pCMap, unsigned int cid, CFeRect &rect);
            virtual BOOL GetGlyphBBox(int glyphIndex, CFeRect &rect);
            CFontFace* FindFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
            virtual CFeGlyph* GetGlyph(FE_FontFace face, unsigned int cid);
            virtual CFeGlyph* GetGlyphByIndex(FE_FontFace face, int glyphIndex);

            virtual FE_DebugInfo GlyphDebugStart0(FE_FontFace face, int cid, int set);
            virtual BOOL GlyphDebugContinue0(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugStepOver0(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugStepIn0(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugStepOut0(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugEnd0(FE_FontFace face, FE_DebugInfo debugInfo);

            virtual FE_DebugInfo GlyphDebugStart(FE_FontFace face, int cid, int set);
            virtual BOOL GlyphDebugContinue(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugStepOver(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugStepIn(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugStepOut(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugEnd(FE_FontFace face, FE_DebugInfo debugInfo);

            virtual BOOL GlyphDebugSetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);
            virtual BOOL GlyphDebugDelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);
            virtual void* GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo);
            virtual BOOL GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void *breakpoints);
            virtual BOOL GlyphDebugIsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos);

            void SetDebugState(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo);
            void SetDebugStack(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo);
            void SetDebugStorage(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo);
            void SetDebugCvt(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo);

            float RoundToInt(float v);

        private:
            BOOL ReadTables(CTableEntry *pTable);

            //CGlyphOutlines* ScaleSimpleGlyph(CTTFontFace *pFace, int glyphIndex);
            //CGlyphOutlines* ScaleCompoundGlyph(CTTFontFace *pFace, int glyphIndex);
            //void RunGlyphInstruction(CTTFontFace *pFace, int glyphIndex, CGlyphOutlines *pOutlines);
            CGlyphOutlines* BuildGlyphOrigOutline(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef);// , bool bNoInstrScale = false);
            CGlyphOutlines* BuildGlyphOutline(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef, CMatrix3x3 &mat);
            CGlyphOutlines* PrepareSimpleGlyphOutlines(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef, CGlyphOutlines **pOrigOutlines);
            void PrepareOutlinesForInstrs(CGlyphOutlines *pOutlines, int advanceWidth, int lsb, int xMin, CGlyphOutlines **pOrigOutlines, float scale);
            //void RunInstrsOnOutlines(CGlyphOutlines * pOutlines);

            void GetSubglyphMatrix(CCompoundGlyph::CComponent *pComponent, CGlyphOutlines *pCompoundOutlines, CGlyphOutlines *pOutlines, CMatrix3x3 &mat);

            void AppendPhantomPointsAndScale(CGlyphOutlines *pOutlines, int advanceWidth, int lsb, int xMin, CGlyphOutlines **pOrigOutlines, float scale);

        public:
            // Table Directory //
            float sfntVersion;
            UShort numTables;
            UShort searchRange;
            UShort entrySelector;
            UShort rangeShift;

            CTableEntry *tableEntries;
            CTableHead *pTableHead;
            CTableMaxp *pTableMaxp;
            CTableName *pTableName;
            CTableHhea *pTableHhea;
            CTableHmtx *pTableHmtx;
            CTableCMap *pTableCMap;
            CTableLoca *pTableLoca;
            CTableGlyf *pTableGlyf;
            CTablePost *pTablePost;
            CTableGasp *pTableGasp;
            CTableHdmx *pTableHdmx;
            CTableKern *pTableKern;
            CTableVhea *pTableVhea;
            CTableVmtx *pTableVmtx;
            CTableOS2  *pTableOS2;

            ULong nCvt;
            Long *pCvt;
            ULong nPrep;
            Byte *pPrep;
            Long nFpgm;
            Byte *pFpgm;


            // run-time
            void *pStartGState;
            std::map<ULong, TT_NestedFunc*> functionDefs;

        public:
            UShort ToUShort(Byte *buf);
            Short ToShort(Byte *buf);
            ULong ToULong(Byte *buf);
            Long ToLong(Byte *buf);
            float FixedToFloat(Byte *buf);
            Long FloatToFixed(float f);
            float F2DOT14ToFloat(Byte *buf);
            Short FloatToF2DOT14(float f);
            Byte ReadByte();
            Char ReadChar();
            UShort ReadUShort();
            Short ReadShort();
            ULong Read3Bytes();
            ULong ReadULong();
            Long ReadLong();
            UShort ReadFUWord();
            Short ReadFWord();
            float ReadFixed();
            float ReadF2Dot14();


        };

    };

};
