#include "stdafx.h"
#include "TableHead.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableHead::CTableHead(void)
        {
        }


        CTableHead::~CTableHead(void)
        {
        }

        void CTableHead::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadFixed();
            fontRevision = pFont->ReadFixed();
            checkSumAdjustment = pFont->ReadULong();
            magicNumber = pFont->ReadULong();
            flags = pFont->ReadUShort();
            unitsPerEm = pFont->ReadUShort();
            pFont->ReadBytes(created, 8);
            pFont->ReadBytes(modified, 8);
            xMin = pFont->ReadFWord();
            yMin = pFont->ReadFWord();
            xMax = pFont->ReadFWord();
            yMax = pFont->ReadFWord();
            macStyle = pFont->ReadUShort();
            lowestRecPPEM = pFont->ReadUShort();
            fontDirectionHint = pFont->ReadShort();
            indexToLocFormat = pFont->ReadShort();
            glyphDataFormat = pFont->ReadShort();

            bBaseLineAtY0 = ((flags & 0x1) > 0);
            bLsbAtX0 = ((flags & 0x2) > 0);
            bInstrOnPointSize = ((flags & 0x4) > 0);
            bIntegerScaling = ((flags & 0x8) > 0);
            bVertLayout = ((flags & 0x20) > 0);

        }
    };

};
