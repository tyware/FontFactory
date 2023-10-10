#pragma once
//#include "stdio.h"
#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableOS2
        {
        public:
            CTableOS2();

            void Load(void* ptr, Long offset);

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

    };

};