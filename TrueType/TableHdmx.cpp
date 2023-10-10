#include "stdafx.h"
#include "TableHdmx.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableHdmx::CTableHdmx(void)
            :numGlyphs(0), pRecords(0)
        {
        }


        CTableHdmx::~CTableHdmx(void)
        {
            if(pRecords) delete[] pRecords;
        }

        void CTableHdmx::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadShort();
            numRecords = pFont->ReadShort();
            sizeOfRecord = pFont->ReadLong();
            if(numRecords > 0)
            {
                pRecords = new CHdmxRecord[numRecords];
                for(UShort i = 0; i< numRecords; i++)
                {
                    pRecords[i].pixelSize = pFont->ReadByte();
                    pRecords[i].maxWidth = pFont->ReadByte();
                    if(numGlyphs > 0)
                    {
                        pRecords[i].pWidths = new Byte[numGlyphs];
                        pFont->ReadBytes((char*)pRecords[i].pWidths, numGlyphs);
                    }
                }
            }
        }
    };

};

