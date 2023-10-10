#pragma once
#include "BaseFont.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        typedef unsigned char Byte;
        typedef char Char;
        typedef unsigned short UShort;
        typedef short Short;
        typedef UINT32 ULong;
        typedef INT32 Long;
        typedef INT32 Fixed;
        typedef INT16 FWord;
        typedef INT32 FUInt;
        typedef UINT32 UFUInt;
        typedef INT16 F2Dot14;

        typedef enum
        {
            TAG_none,
            TAG_cmap,
            TAG_glyf,
            TAG_head,
            TAG_hhea,
            TAG_hmtx,
            TAG_loca,
            TAG_maxp,
            TAG_name,
            TAG_post,
            TAG_os2,
            TAG_cvt,
            TAG_ebdt,
            TAG_eblc,
            TAG_ebsc,
            TAG_fpgm,
            TAG_gasp,
            TAG_hdmx,
            TAG_kern,
            TAG_ltsh,
            TAG_prep,
            TAG_pclt,
            TAG_vdmx,
            TAG_vhea,
            TAG_vmtx
        }TABLE_TAG;

        class CTableOS2
        {
        public:
            CTableOS2()
                :version(0), xAvgCharWidth(0), usWeightClass(0), usWidthClass(0), fsType(0), ySubscriptXSize(0), ySubscriptYSize(0), ySubscriptXOffset(0), ySubscriptYOffset(0), ySuperscriptXSize(0)
                , ySuperscriptYSize(0), ySuperscriptXOffset(0), ySuperscriptYOffset(0), yStrikeoutSize(0), yStrikeoutPosition(0), sFamilyClass(0)
                , bFamilyType(0), bSerifStyle(0), bWeight(0), bProportion(0), bContrast(0), bStrokeVariation(0), bArmStyle(0), bLetterForm(0), bMidline(0), bXHeight(0)
                , ulUnicodeRange1(0), ulUnicodeRange2(0), ulUnicodeRange3(0), ulUnicodeRange4(0), fsSelection(0), usFirstCharIndex(0), usLastCharIndex(0), sTypoAscender(0), sTypoDescender(0)
                , sTypoLineGap(0), usWinAscent(0), usWinDescent(0), ulCodePageRange1(0), ulCodePageRange2(0)
            {
                memset(achVendID, 0, 4);
            };

        public:
            UShort version;
            Short xAvgCharWidth;
            UShort usWeightClass;
            UShort usWidthClass;
            Short fsType;
            Short ySubscriptXSize;
            Short ySubscriptYSize;
            Short ySubscriptXOffset;
            Short ySubscriptYOffset;
            Short ySuperscriptXSize;
            Short ySuperscriptYSize;
            Short ySuperscriptXOffset;
            Short ySuperscriptYOffset;
            Short yStrikeoutSize;
            Short yStrikeoutPosition;
            Short sFamilyClass;
            // Panose
            Byte bFamilyType;
            Byte bSerifStyle;
            Byte bWeight;
            Byte bProportion;
            Byte bContrast;
            Byte bStrokeVariation;
            Byte bArmStyle;
            Byte bLetterForm;
            Byte bMidline;
            Byte bXHeight;

            ULong ulUnicodeRange1;
            ULong ulUnicodeRange2;
            ULong ulUnicodeRange3;
            ULong ulUnicodeRange4;
            Char achVendID[4];
            UShort fsSelection;
            UShort usFirstCharIndex;
            UShort usLastCharIndex;
            UShort sTypoAscender;
            UShort sTypoDescender;
            UShort sTypoLineGap;
            UShort usWinAscent;
            UShort usWinDescent;
            ULong ulCodePageRange1;
            ULong ulCodePageRange2;
        };

        class CTableCMap
        {
        public:
            CTableCMap()
                :version(0), numTables(0), subTables(0)
            {};
            ~CTableCMap()
            {
                if(subTables) delete[] subTables;
            }

        public:
            class CSubTable
            {
            public:
                UShort platformID;
                UShort encodingID;
                ULong offset;
            };

            UShort version;
            UShort numTables;

            CSubTable *subTables;
        };

        class CTableEntry
        {
        public:
            CTableEntry():offset(0), length(0), tag(TAG_none) { }

            TABLE_TAG tag;
            ULong offset;
            ULong length;
        };

        class CTrueTypeFont : public CBaseFont
        {
        public:
            CTrueTypeFont();
            virtual ~CTrueTypeFont();

            virtual BOOL LoadFontFile(wchar_t *fileName);

            static UShort ToUShort(Byte *buf);
            static Short ToShort(Byte *buf);
            static ULong ToULong(Byte *buf);
            static Long ToLong(Byte *buf);
            static float FixedToFloat(Byte *buf);
            static float F2DOT14ToFloat(Byte *buf);

            Byte ReadByte();
            Char ReadChar();
            UShort ReadUShort();
            Short ReadShort();
            ULong ReadULong();
            Long ReadLong();
            UShort ReadFUWord();
            Short ReadFWord();
            float ReadFixed();
            float ReadF2Dot14();

            BOOL ReadTables(CTableEntry *pTable);
            BOOL ReadOS2(Long offset);
            BOOL ReadCMap(Long offset, Long length);

        public:
            float sfntVersion;
            UShort numTables;
            UShort searchRange;
            UShort entrySelector;
            UShort rangeShift;

            CTableEntry *tableEntries;

            CTableOS2 tableOS2;
            CTableCMap tableCMap;

        private:
            


        };

    };

};

