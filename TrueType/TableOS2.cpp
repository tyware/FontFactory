#include "stdafx.h"
#include "TableOS2.h"
#include "Global.h"
#include "TrueType.h"


namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
            CTableOS2::CTableOS2()
                :version(0), xAvgCharWidth(0), usWeightClass(0), usWidthClass(0), fsType(0), ySubscriptXSize(0), ySubscriptYSize(0), ySubscriptXOffset(0), ySubscriptYOffset(0), ySuperscriptXSize(0)
                , ySuperscriptYSize(0), ySuperscriptXOffset(0), ySuperscriptYOffset(0), yStrikeoutSize(0), yStrikeoutPosition(0), sFamilyClass(0)
                , bFamilyType(0), bSerifStyle(0), bWeight(0), bProportion(0), bContrast(0), bStrokeVariation(0), bArmStyle(0), bLetterForm(0), bMidline(0), bXHeight(0)
                , ulUnicodeRange1(0), ulUnicodeRange2(0), ulUnicodeRange3(0), ulUnicodeRange4(0), fsSelection(0), usFirstCharIndex(0), usLastCharIndex(0), sTypoAscender(0), sTypoDescender(0)
                , sTypoLineGap(0), usWinAscent(0), usWinDescent(0), ulCodePageRange1(0), ulCodePageRange2(0)
            {
                memset(achVendID, 0, 4);
            };

            void CTableOS2::Load(void* ptr, Long offset)
            {
                CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

                pFont->Seek(offset, SEEK_SET);

                version = pFont->ReadUShort();
                xAvgCharWidth = pFont->ReadShort();
                usWeightClass = pFont->ReadUShort();
                usWidthClass = pFont->ReadUShort();
                fsType = pFont->ReadShort();
                ySubscriptXSize = pFont->ReadShort();
                ySubscriptYSize = pFont->ReadShort();
                ySubscriptXOffset = pFont->ReadShort();
                ySubscriptYOffset = pFont->ReadShort();
                ySuperscriptXSize = pFont->ReadShort();
                ySuperscriptYSize = pFont->ReadShort();
                ySuperscriptXOffset = pFont->ReadShort();
                ySuperscriptYOffset = pFont->ReadShort();
                yStrikeoutSize = pFont->ReadShort();
                yStrikeoutPosition = pFont->ReadShort();
                sFamilyClass = pFont->ReadShort();

                bFamilyType = pFont->ReadByte();
                bSerifStyle = pFont->ReadByte();
                bWeight = pFont->ReadByte();
                bProportion = pFont->ReadByte();
                bContrast = pFont->ReadByte();
                bStrokeVariation = pFont->ReadByte();
                bArmStyle = pFont->ReadByte();
                bLetterForm = pFont->ReadByte();
                bMidline = pFont->ReadByte();
                bXHeight = pFont->ReadByte();

                ulUnicodeRange1 = pFont->ReadULong();
                ulUnicodeRange2 = pFont->ReadULong();
                ulUnicodeRange3 = pFont->ReadULong();
                ulUnicodeRange4 = pFont->ReadULong();

                pFont->ReadBytes(achVendID, 4);

                fsSelection = pFont->ReadUShort();
                usFirstCharIndex = pFont->ReadUShort();
                usLastCharIndex = pFont->ReadUShort();
                sTypoAscender = pFont->ReadUShort();
                sTypoDescender = pFont->ReadUShort();
                sTypoLineGap = pFont->ReadUShort();
                usWinAscent = pFont->ReadUShort();
                usWinDescent = pFont->ReadUShort();
                ulCodePageRange1 = pFont->ReadUShort();
                ulCodePageRange2 = pFont->ReadUShort();

            }

    };

};

