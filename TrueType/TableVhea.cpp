#include "stdafx.h"
#include "TableVhea.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableVhea::CTableVhea(void)
        {
        }


        CTableVhea::~CTableVhea(void)
        {
        }

        void CTableVhea::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadFixed();
            Short vertTypoAscender = pFont->ReadShort();
            Short vertTypoDescender = pFont->ReadShort();
            Short vertTypoLineGap = pFont->ReadShort();
            Short advanceHeightMax = pFont->ReadShort();
            Short minTopSideBearing = pFont->ReadShort();
            Short minBottomSideBearing = pFont->ReadShort();
            Short yMaxExtent = pFont->ReadShort();
            Short caretSlopeRise = pFont->ReadShort();
            Short caretSlopeRun = pFont->ReadShort();
            Short caretOffset = pFont->ReadShort();
            Short reserved1 = pFont->ReadShort();
            Short reserved2 = pFont->ReadShort();
            Short reserved3 = pFont->ReadShort();
            Short reserved4 = pFont->ReadShort();
            Short metricDataFormat = pFont->ReadShort();
            UShort numOfLongVerMetrics = pFont->ReadUShort();
        }

    };

};

