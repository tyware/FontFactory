#include "stdafx.h"
#include "TableGasp.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {


        CTableGasp::CTableGasp(void)
            :numRanges(0), pGaspRange(0)
        {
        }


        CTableGasp::~CTableGasp(void)
        {
            if(pGaspRange) delete[] pGaspRange;
        }

        void CTableGasp::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadUShort();
            numRanges = pFont->ReadUShort();
            if(numRanges > 0)
            {
                pGaspRange = new CGaspRange[numRanges];
                for(UShort i = 0; i< numRanges; i++)
                {
                    pGaspRange[i].rangeMaxPPEM = pFont->ReadUShort();
                    pGaspRange[i].rangeGaspBehavior = pFont->ReadUShort();
                }
            }
        }


    };

};

