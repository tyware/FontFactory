#include "stdafx.h"
#include "TableMaxp.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableMaxp::CTableMaxp(void)
        {
        }


        CTableMaxp::~CTableMaxp(void)
        {
        }

        void CTableMaxp::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadFixed();
            numGlyphs = pFont->ReadUShort();
            maxPoints = pFont->ReadUShort();
            maxContours = pFont->ReadUShort();
            maxComponentPoints = pFont->ReadUShort();
            maxComponentContours = pFont->ReadUShort();
            maxZones = pFont->ReadUShort();
            maxTwilightPoints = pFont->ReadUShort();
            maxStorage = pFont->ReadUShort();
            maxFunctionDefs = pFont->ReadUShort();
            maxInstructionDefs = pFont->ReadUShort();
            maxStackElements = pFont->ReadUShort();
            maxSizeOfInstructions = pFont->ReadUShort();
            maxComponentElements = pFont->ReadUShort();
            maxComponentDepth = pFont->ReadUShort();
        }
    };

};
