#include "stdafx.h"
#include "TableHhea.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableHhea::CTableHhea(void)
        {
        }


        CTableHhea::~CTableHhea(void)
        {
        }

        void CTableHhea::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadFixed();
            ascender = pFont->ReadFWord();
            descender = pFont->ReadFWord();
            lineGap = pFont->ReadFWord();
            advanceWidthMax = pFont->ReadFWord();
            minLeftSideBearing = pFont->ReadFWord();
            minRightSideBearing = pFont->ReadFWord();
            xMaxExtent = pFont->ReadFWord();
            caretSlopeRise = pFont->ReadShort();
            caretSlopeRun = pFont->ReadShort();
            caretOffset = pFont->ReadShort();
            reserved1 = pFont->ReadShort();
            reserved2 = pFont->ReadShort();
            reserved3 = pFont->ReadShort();
            reserved4 = pFont->ReadShort();
            metricDataFormat = pFont->ReadShort();
            numOfLongHorMetrics = pFont->ReadUShort();

        }

    };

};
