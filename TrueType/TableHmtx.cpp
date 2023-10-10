#include "stdafx.h"
#include "TableHmtx.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        CTableHmtx::CTableHmtx(void)
            :numMetrix(0), pMetrix(0), pLeftSideBearing(0), numLeftSideBearing(0)
        {
        }


        CTableHmtx::~CTableHmtx(void)
        {
            if(pMetrix) delete[] pMetrix;
            if(pLeftSideBearing) delete[] pLeftSideBearing;
        }


        void CTableHmtx::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            if(numMetrix)
                pMetrix = new longHorMetrix[numMetrix];
            for(Long i = 0; i<numMetrix; i++)
            {
                pMetrix[i].advanceWidth = pFont->ReadUShort();
                pMetrix[i].leftSideBearing = pFont->ReadShort();
            }
            if(numLeftSideBearing)
                pLeftSideBearing = new FWord[numLeftSideBearing];
            for(Long i = 0; i<numLeftSideBearing; i++)
                pLeftSideBearing[i] = pFont->ReadFWord();

        }
    
    };

};


