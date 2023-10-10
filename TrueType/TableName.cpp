#include "stdafx.h"
#include "TableName.h"
#include "TrueType.h"


namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTableName::CTableName(void)
            :pNameRecord(0)//, pNameString(0)
        {
        }


        CTableName::~CTableName(void)
        {
            if(pNameRecord) delete[] pNameRecord;
            //if(pNameString) delete[] pNameString;
        }

        void CTableName::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            format = pFont->ReadUShort();
            count = pFont->ReadUShort();
            stringOffset = pFont->ReadUShort();
            if(count > 0)
                pNameRecord = new CNameRecord[count];

            for(UShort i = 0; i< count; i++)
            {
                pNameRecord[i].platformID = pFont->ReadUShort();
                pNameRecord[i].platformSpecificID = pFont->ReadUShort();
                pNameRecord[i].languageID = pFont->ReadUShort();
                pNameRecord[i].nameID = pFont->ReadUShort();
                pNameRecord[i].length = pFont->ReadUShort();
                pNameRecord[i].offset = pFont->ReadUShort();
            }
            // load strings: file pointer is at the storage beginning.
            Long curOffset = pFont->Tell();
            for(UShort i = 0; i< count; i++)
            {
                if(pNameRecord[i].length > 0)
                {
                    pFont->Seek(curOffset + pNameRecord[i].offset, SEEK_SET);
                    pNameRecord[i].pString = new char[pNameRecord[i].length+ 4];
                    pFont->ReadBytes(pNameRecord[i].pString, pNameRecord[i].length  + 1);
                    pNameRecord[i].pString[pNameRecord[i].length] = 0;
                    pNameRecord[i].pString[pNameRecord[i].length + 1] = 0;
                    if(pNameRecord[i].platformID == FE_PLATFORM_ID_Windows) // Unicode_16BE
                    {
                        for(int j = 0; j<pNameRecord[i].length; j++)
                        {
                            char ch =  pNameRecord[i].pString[j];
                            pNameRecord[i].pString[j] = pNameRecord[i].pString[j + 1];
                            pNameRecord[i].pString[j + 1] = ch;
                        }
                    }
                }
            }
        }

    };

};
