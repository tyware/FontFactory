#include "stdafx.h"
#include "TableLoca.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableLoca::CTableLoca(void)
            :numGlyphs(0), bIsShortFormat(false), pOffset(0)
        {
        }


        CTableLoca::~CTableLoca(void)
        {
            if(pOffset) delete[] pOffset;
        }

        void CTableLoca::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            if(numGlyphs > 0)
            {
                pOffset = new ULong[numGlyphs + 1];
                if(bIsShortFormat)
                {
                    for(Long i = 0; i <= numGlyphs; i++)
                        pOffset[i] = 2 * pFont->ReadUShort();
                }
                else
                {
                    for(Long i = 0; i <= numGlyphs; i++)
                        pOffset[i] = pFont->ReadULong();
                }
            }
        }

    };

};
