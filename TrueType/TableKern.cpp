#include "stdafx.h"
#include "TableKern.h"
#include "TrueType.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {



        CTableKern::CTableKern(void)
            :nTables(0)
        {
        }


        CTableKern::~CTableKern(void)
        {
            for(size_t i = 0; i<subTables.size(); i++)
            {
                delete subTables[i];
            }
            subTables.clear();
        }

        void CTableKern::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadUShort();
            nTables = pFont->ReadUShort();
            if(nTables > 0)
            {
                for(UShort i = 0; i< nTables; i++)
                {
                    UShort version = pFont->ReadUShort();
                    UShort length = pFont->ReadUShort();
                    UShort coverage = pFont->ReadUShort();
                    switch(coverage>>8) // format
                    {
                    case 0:
                        {
                            CSubtable0 *pSub = new CSubtable0();
                            pSub->version = version;
                            pSub->length = length;
                            pSub->coverage = coverage;
                            pSub->nPairs = pFont->ReadUShort();
                            pSub->searchRange = pFont->ReadUShort();
                            pSub->entrySelector = pFont->ReadUShort();
                            pSub->rangeShift = pFont->ReadUShort();
                            if(pSub->nPairs > 0)
                            {
                                pSub->pKernPair = new CSubtable0::CKernPair[pSub->nPairs];
                                for(UShort j = 0; j<pSub->nPairs; j++)
                                {
                                    pSub->pKernPair[j].left = pFont->ReadUShort();
                                    pSub->pKernPair[j].right = pFont->ReadUShort();
                                    pSub->pKernPair[j].value = pFont->ReadFWord();
                                }
                            }
                            subTables.push_back(pSub);
                        }
                        break;
                    case 2:
                        pFont->Seek(length - 6, SEEK_CUR);
                        break;
                    case 1: // not support
                    case 3:
                    default:
                        pFont->Seek(length - 6, SEEK_CUR);
                        break;
                    }
                }
            }
        }

    };

};
