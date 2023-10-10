#pragma once
#include "Global.h"
#include <map>

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableCMap
        {
        public:
            CTableCMap();
            ~CTableCMap();

            void Load(void* ptr, Long offset);
            void* GetFormat(int platformId, int encodingId);
            int GetNumCharacters(void *pCMap);
            int GetFirstCharacter(void *pCMap);
            int GetNextCharacter(void *pCMap);
            Long GetGlyphIndex(void* pFormat, ULong chCode);
            Byte BytesInChar(void* pFormat, Char *s);

        public:
            class CMapFormat
            {
            public:
                CMapFormat():rtOff(0), rtPtr(0){};
                virtual ~CMapFormat(){};
                virtual int GetNextCharacter(){return -1;};

            public:
                UShort format;
                ULong length;
                ULong language;

            public: // run-time
                int rtOff;
                int rtOff1;
                void *rtPtr;
                void *rtPtr1;
            };

            class CMapFormat0 : public CMapFormat
            {
            public:
                CMapFormat0() { format = 0; };
                Byte glyphIdArray[256];
            };
            class CMapFormat2 : public CMapFormat
            {
            public:
                class CFormat2SubHeader
                {
                public:
                    UShort firstCode;
                    UShort entryCount;
                    Short idDelta;
                    UShort idRangeOffset;
                };
            public:
                CMapFormat2():numGlyphs(0), startOfGlyphs(0), pSubHeader(0), pGlyphIdArray(0){ format = 2; };
                virtual ~CMapFormat2()
                {
                    if(pSubHeader) delete[] pSubHeader;
                    if(pGlyphIdArray) delete[] pGlyphIdArray;
                }

            public:
                UShort subHeaderKeys[256];
                CFormat2SubHeader *pSubHeader;
                UShort *pGlyphIdArray;

                Long numGlyphs;
                Long startOfGlyphs; // for reading file
                Long maxNumHeaders;

            };
            class CMapFormat4 : public CMapFormat
            {
            public:
                CMapFormat4()
                    : /*endCodeOffset(0),*/ segCount(0), segCountX2(0), searchRange(0), entrySelector(0), rangeShift(0), pEndCount(0),
                    reservedPad(0), pStartCount(0), pIdDelta(0), pIdRangeOffset(0), pGlyphIdArray(0), pSegGlyphOffset(0)
                {
                    format = 4;
                };
                virtual ~CMapFormat4()
                {
                    if(pEndCount) delete[] pEndCount;
                    if(pStartCount) delete[] pStartCount;
                    if(pIdDelta) delete[] pIdDelta;
                    if(pIdRangeOffset) delete[] pIdRangeOffset;
                    if(pGlyphIdArray) delete[] pGlyphIdArray;
                    if(pSegGlyphOffset) delete[] pSegGlyphOffset;
                }

            public:
                UShort segCount;
                UShort segCountX2; // 2 x segCount
                UShort searchRange; // 2 x (2**floor(log2(segCount)))
                UShort entrySelector; // log2(searchRange/2)
                UShort rangeShift; // 2 x segCount - searchRange
                UShort *pEndCount; // endCount[segCount]	End characterCode for each segment, last =0xFFFF.
                UShort reservedPad; // Set to 0.
                UShort *pStartCount;  // startCount[segCount]	Start character code for each segment.
                Short *pIdDelta; // idDelta[segCount]	Delta for all character codes in segment.
                UShort *pIdRangeOffset; // idRangeOffset[segCount]	Offsets into glyphIdArray or 0
                UShort *pGlyphIdArray; //glyphIdArray[ ]	Glyph index array (arbitrary length)

                UShort *pSegGlyphOffset;

                Long numRangeGlyphs;

                // Cashed value
                //Long endCodeOffset;


            };
            class CMapFormat6 : public CMapFormat
            {
            public:
                CMapFormat6():pGlyphIdArray(0){format = 6; };
                ~CMapFormat6(){ if(pGlyphIdArray) delete[] pGlyphIdArray;};
                UShort firstCode;
                UShort entryCount;
                UShort *pGlyphIdArray;
            };
            class CMapFormat8 : public CMapFormat
            {
            public:
                class CSequentialMapGroup
                {
                public:
                    ULong startCharCode;
                    ULong endCharCode;
                    ULong startGlyphID;
                };

                CMapFormat8() : numGroups(0), pMapGroup(0) {format = 8; };
                virtual ~CMapFormat8(){ if(pMapGroup) delete[] pMapGroup;};
                BOOL is32Bit(UShort first)
                {
                    return ((is32[first/8] & (1 << (7 - (first % 8)))) != 0? true: false);
                };

            public:
                Byte is32[8192];
                ULong numGroups;
                CSequentialMapGroup *pMapGroup;
            };
            class CMapFormat10 : public CMapFormat
            {
            public:
                CMapFormat10(): pGlyphs(0){format = 10; };
                virtual ~CMapFormat10()
                {
                    if(pGlyphs) delete[]  pGlyphs;
                };

                ULong startCharCode;
                ULong numChars;
                UShort *pGlyphs;
            };
            class CMapFormat12 : public CMapFormat
            {
            public:
                class CSequentialMapGroup
                {
                public:
                    ULong startCharCode;
                    ULong endCharCode;
                    ULong startGlyphID;
                };

                CMapFormat12() : numGroups(0), pGroups(0) {format = 12; };
                virtual ~CMapFormat12(){ if(pGroups) delete[] pGroups;};
            public:
                ULong numGroups;
                CSequentialMapGroup *pGroups;
            };
            /*class CMapFormat14 : public CMapFormat
            {
            public:
                class CVariationSelector
                {
                public:
                    ULong varSelector;
                    Long defaultUVSOffset;
                    Long nonDefaultUVSOffset;
                };

                CMapFormat14() : numVarSelectorRecords(0), pVarSelector(0){};
                ~CMapFormat14()
                {
                    if(pVarSelector) delete[] pVarSelector;
                };

            public:
                ULong numVarSelectorRecords;
                CVariationSelector *pVarSelector;
            };*/

            class CSubTable
            {
            public:
                CSubTable():platformID(0), encodingID(0), offset(0), format(0) {};
                ~CSubTable() { if(format) delete format; };

            public:
                UShort platformID;
                UShort encodingID;
                ULong offset;

                CMapFormat *format;
            };

            UShort version;
            UShort numTables;

            CSubTable *subTables;
        };


    };

};