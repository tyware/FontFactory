#include "stdafx.h"
#include "TableVmtx.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {


        CTableVmtx::CTableVmtx(void)
            :numMetrix(0), pMetrix(0), pTopSideBearing(0), numTopSideBearing(0)
        {
        }


        CTableVmtx::~CTableVmtx(void)
        {
            if(pMetrix) delete[] pMetrix;
            if(pTopSideBearing) delete[] pTopSideBearing;
        }

        void CTableVmtx::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            if(numMetrix)
                pMetrix = new longHorMetrix[numMetrix];
            for(Long i = 0; i<numMetrix; i++)
            {
                pMetrix[i].advanceHeight = pFont->ReadUShort();
                pMetrix[i].topSideBearing = pFont->ReadShort();
            }
            if(numTopSideBearing)
                pTopSideBearing = new Short[numTopSideBearing];
            for(Long i = 0; i<numTopSideBearing; i++)
                pTopSideBearing[i] = pFont->ReadShort();

        }
    

    };

};
