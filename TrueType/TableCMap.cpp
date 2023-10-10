#include "stdafx.h"
#include "TableCMap.h"
#include "TrueType.h"
#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        CTableCMap::CTableCMap()
            :version(0), numTables(0), subTables(0)
        {}

        CTableCMap::~CTableCMap()
        {
            if(subTables) delete[] subTables;
        }

        void CTableCMap::Load(void* ptr, Long offset)
        {
            CTrueTypeFont *pFont = (CTrueTypeFont*) ptr;

            pFont->Seek(offset, SEEK_SET);

            version = pFont->ReadUShort();
            numTables = pFont->ReadUShort();

            if(numTables > 0) subTables = new CTableCMap::CSubTable[numTables];

            // read tables 
            for(UShort i = 0; i< numTables; i++)
            {
                subTables[i].platformID = pFont->ReadUShort();
                subTables[i].encodingID = pFont->ReadUShort();
                subTables[i].offset = pFont->ReadULong();
            }

            // read table entries
            for(UShort i = 0; i< numTables; i++)
            {
                pFont->Seek(offset + subTables[i].offset, SEEK_SET);
                UShort f = pFont->ReadUShort();
                switch(f)
                {
                case 2:
                    {
                        CMapFormat2 *pFormat = new CMapFormat2();
                        subTables[i].format = pFormat;
                        pFormat->length = pFont->ReadUShort();
                        pFormat->language = pFont->ReadUShort();
                        pFormat->maxNumHeaders= 0;
                        for(int j =0; j<256; j++)
                        {
                            pFormat->subHeaderKeys[i] = pFont->ReadUShort();
                            UShort headerIndex = pFormat->subHeaderKeys[i] / 8;
                            if(headerIndex > pFormat->maxNumHeaders) pFormat->maxNumHeaders = headerIndex;
                        }
                        if(pFormat->maxNumHeaders > 0)
                            pFormat->pSubHeader = new CMapFormat2::CFormat2SubHeader[pFormat->maxNumHeaders];

                        Long offs = pFont->Tell();
                        for(int j = 0; j<pFormat->maxNumHeaders; j++)
                        {
                            pFormat->pSubHeader[j].firstCode = pFont->ReadUShort();
                            pFormat->pSubHeader[j].entryCount = pFont->ReadUShort();
                            pFormat->pSubHeader[j].idDelta = pFont->ReadShort();
                            offs += 6;
                            pFormat->pSubHeader[j].idRangeOffset = pFont->ReadUShort();
                            pFormat->pSubHeader[j].idRangeOffset += offs;
                            offs += 2;
                        }

                        pFormat->startOfGlyphs = pFont->Tell();
                        pFormat->numGlyphs = (pFormat->startOfGlyphs - (offset + subTables[i].offset)) / sizeof(UShort);
                        pFormat->pGlyphIdArray = new UShort[pFormat->numGlyphs];
                        for(int j = 0; j < pFormat->numGlyphs; j++)
                            pFormat->pGlyphIdArray[j] = pFont->ReadUShort();
                    }
                    break;
                case 4:
                    {
                        CMapFormat4 *pFormat = new CMapFormat4();
                        subTables[i].format = pFormat;
                        pFormat->length = pFont->ReadUShort();
                        pFormat->language = pFont->ReadUShort();
                        pFormat->segCountX2 = pFont->ReadUShort();
                        pFormat->segCount = pFormat->segCountX2 / 2;
                        pFormat->searchRange = pFont->ReadUShort();
                        pFormat->entrySelector = pFont->ReadUShort();
                        pFormat->rangeShift = pFont->ReadUShort();

#ifdef DEBUG_CMAP
                        FILE *fp;
                        char buf[256];
                        fopen_s(&fp, "G:\\Test\\tmp1\\CMap.txt", "w");
                        sprintf_s(buf, "segCount: %d\nsearchRange: %d\nentrySelector: %d\nrangeShift: %d\n\n", pFormat->segCount, pFormat->searchRange, pFormat->entrySelector, pFormat->rangeShift);
                        fwrite(buf, 1, strlen(buf), fp);
#endif

                        if(pFormat->segCount > 0)
                        {
                            pFormat->pEndCount = new UShort[pFormat->segCount];

#ifdef DEBUG_CMAP
                            sprintf_s(buf, 256, "EndCount: \n");
                            fwrite(buf, 1, strlen(buf), fp);
#endif
                            for(int j = 0; j < pFormat->segCount; j++)
                            {
                                pFormat->pEndCount[j] = pFont->ReadUShort();
#ifdef DEBUG_CMAP
                                sprintf_s(buf, 256, "%d  ", pFormat->pEndCount[j]);
                                fwrite(buf, 1, strlen(buf), fp);
#endif
                            }

                            pFormat->reservedPad = pFont->ReadUShort();

#ifdef DEBUG_CMAP
                            sprintf_s(buf, 256, "\n\nStartCount: \n");
                            fwrite(buf, 1, strlen(buf), fp);
#endif
                            pFormat->pStartCount = new UShort[pFormat->segCount];
                            for(int j = 0; j < pFormat->segCount; j++)
                            {
                                pFormat->pStartCount[j] = pFont->ReadUShort();
#ifdef DEBUG_CMAP
                                sprintf_s(buf, 256, "%d  ", pFormat->pStartCount[j]);
                                fwrite(buf, 1, strlen(buf), fp);
#endif
                            }

                            //int total = 0;
                            //for(int j = 0; j < pFormat->segCount && pFormat->pStartCount[j] != 0xFFFF; j++)
                            //    total += (pFormat->pEndCount[j] - pFormat->pStartCount[j] + 1);

#ifdef DEBUG_CMAP
                            sprintf_s(buf, 256, "\n\nIdDelta: \n");
                            fwrite(buf, 1, strlen(buf), fp);
#endif
                            pFormat->pIdDelta = new Short[pFormat->segCount];
                            for(int j = 0; j < pFormat->segCount; j++)
                            {
                                pFormat->pIdDelta[j] = pFont->ReadShort();
#ifdef DEBUG_CMAP
                                sprintf_s(buf, 256, "%d  ", pFormat->pIdDelta[j]);
                                fwrite(buf, 1, strlen(buf), fp);
#endif
                            }

#ifdef DEBUG_CMAP
                            sprintf_s(buf, 256, "\n\nIdRangeOffset: \n");
                            fwrite(buf, 1, strlen(buf), fp);
#endif
                            pFormat->pIdRangeOffset = new UShort[pFormat->segCount];
                            pFormat->pSegGlyphOffset = new UShort[pFormat->segCount];
                            for(int j = 0; j < pFormat->segCount; j++)
                            {
                                pFormat->pIdRangeOffset[j] = pFont->ReadUShort();
                                if(pFormat->pIdRangeOffset[j] == 0)
                                    pFormat->pSegGlyphOffset[j] = 0;
                                else
                                    pFormat->pSegGlyphOffset[j] = pFormat->pIdRangeOffset[j]/2 - (pFormat->segCount - j);
#ifdef DEBUG_CMAP
                                sprintf_s(buf, 256, "%d  ", pFormat->pIdRangeOffset[j]);
                                fwrite(buf, 1, strlen(buf), fp);
#endif
                            }

                            Long curOffset = pFont->Tell();
                            pFormat->numRangeGlyphs = (pFormat->length - (curOffset - (offset + subTables[i].offset))) / sizeof(UShort);
                            pFormat->pGlyphIdArray = new UShort[pFormat->numRangeGlyphs];
#ifdef DEBUG_CMAP
                            sprintf_s(buf, 256, "\n\nNumGlyphs: %d\n", pFormat->numRangeGlyphs);
                            fwrite(buf, 1, strlen(buf), fp);
                            sprintf_s(buf, 256, "Glyphs:\n");
                            fwrite(buf, 1, strlen(buf), fp);
#endif
                            for(int j = 0; j < pFormat->numRangeGlyphs; j++)
                            {
                                pFormat->pGlyphIdArray[j] = pFont->ReadUShort();
#ifdef DEBUG_CMAP
                                sprintf_s(buf, 256, "%d(%04X)  ", pFormat->pGlyphIdArray[j], pFormat->pGlyphIdArray[j]);
                                fwrite(buf, 1, strlen(buf), fp);
#endif
                            }
#ifdef DEBUG_CMAP
                            fwrite("\n", 1, 1, fp);
#endif
                        }
#ifdef DEBUG_CMAP
                        fclose(fp);
#endif
                    }
                    break;
                case 6:
                    {
                        CMapFormat6 *pFormat = new CMapFormat6();
                        subTables[i].format = pFormat;
                        pFormat->length = pFont->ReadUShort();
                        pFormat->language = pFont->ReadUShort();
                        pFormat->firstCode = pFont->ReadUShort();
                        pFormat->entryCount = pFont->ReadUShort();
                        pFormat->pGlyphIdArray = new UShort[pFormat->entryCount];
                        for(int j = 0; j < pFormat->entryCount; j++)
                            pFormat->pGlyphIdArray[j] = pFont->ReadUShort();
                    }
                    break;
                case 8:
                    {
                        CMapFormat8 *pFormat = new CMapFormat8();
                        subTables[i].format = pFormat;
                        pFont->ReadUShort(); // reserved
                        pFormat->length = pFont->ReadULong();
                        pFormat->language = pFont->ReadULong();
                        pFont->ReadBytes((char*)pFormat->is32, 8192);
                        pFormat->numGroups = pFont->ReadULong();
                        if(pFormat->numGroups > 0)
                            pFormat->pMapGroup = new CMapFormat8::CSequentialMapGroup[pFormat->numGroups];
                        for(ULong j = 0; j<pFormat->numGroups; j++)
                        {
                            pFormat->pMapGroup[j].startCharCode = pFont->ReadULong();
                            pFormat->pMapGroup[j].endCharCode = pFont->ReadULong();
                            pFormat->pMapGroup[j].startGlyphID = pFont->ReadULong();
                        }
                    }
                    break;
                case 10:
                    {
                        CMapFormat10 *pFormat = new CMapFormat10();
                        subTables[i].format = pFormat;
                        pFont->ReadUShort(); // reserved
                        pFormat->length = pFont->ReadULong();
                        pFormat->language = pFont->ReadULong();
                        pFormat->startCharCode = pFont->ReadULong();
                        pFormat->numChars = pFont->ReadULong();
                        if(pFormat->numChars > 0)
                            pFormat->pGlyphs = new UShort[pFormat->numChars];
                        for(ULong j = 0; j<pFormat->numChars; j++)
                            pFormat->pGlyphs[j] = pFont->ReadULong();
                    }
                    break;
                case 12:
                case 13:
                    {
                        CMapFormat12 *pFormat = new CMapFormat12();
                        subTables[i].format = pFormat;
                        pFont->ReadUShort(); // reserved
                        pFormat->length = pFont->ReadULong();
                        pFormat->language = pFont->ReadULong();
                        pFormat->numGroups = pFont->ReadULong();
                        if(pFormat->numGroups > 0)
                            pFormat->pGroups = new CMapFormat12::CSequentialMapGroup[pFormat->numGroups];
                        for(ULong j = 0; j<pFormat->numGroups; j++)
                        {
                            pFormat->pGroups[j].startCharCode = pFont->ReadULong();
                            pFormat->pGroups[j].endCharCode = pFont->ReadULong();
                            pFormat->pGroups[j].startGlyphID = pFont->ReadULong();
                        }
                    }
                    break;
                //case 14:
                //    {
                //        CMapFormat14 *pFormat = new CMapFormat14();
                //        subTables[i].format = pFormat;
                //        pFormat->length = pFont->ReadULong();
                //        pFormat->numVarSelectorRecords = pFont->ReadULong();
                //        if(pFormat->numVarSelectorRecords > 0)
                //            pFormat->pVarSelector = new CMapFormat14::CVariationSelector[pFormat->numVarSelectorRecords];
                //        for(ULong j = 0; j<pFormat->numVarSelectorRecords; j++)
                //        {
                //            pFormat->pVarSelector[j].varSelector = pFont->Read3Bytes();
                //            pFormat->pVarSelector[j].defaultUVSOffset = pFont->ReadLong();
                //            pFormat->pVarSelector[j].nonDefaultUVSOffset = pFont->ReadLong();
                //        }
                //    }
                //    break;
                case 0:
                default:
                    subTables[i].format = new CMapFormat0();
                    subTables[i].format->length = pFont->ReadUShort();
                    subTables[i].format->language = pFont->ReadUShort();
                    pFont->ReadBytes((char*)((CMapFormat0*)subTables[i].format)->glyphIdArray, 256);
                    //{
                    //    FILE *fp1;
                    //    fopen_s(&fp1, "G:\\Applications\\Private\\FontEditor\\f0.txt", "w");
                    //    char buf[32];
                    //    for(int j=0; j<256; j++)
                    //    {
                    //        sprintf_s(buf, 32, "%02X ", j);
                    //        fwrite(buf, 1, strlen(buf), fp1);
                    //    }
                    //    fwrite("\n", 1, 1, fp1);
                    //    for(int j=0; j<256; j++)
                    //    {
                    //        sprintf_s(buf, 32, "%02X ", ((CMapFormat0*)subTables[i].format)->glyphIdArray[j]);
                    //        fwrite(buf, 1, strlen(buf), fp1);
                    //    }
                    //    fwrite("\n\n", 1, 1, fp1);
                    //    fclose(fp1);
                    //}
                    break;
                }

                subTables[i].format->format = f;
            }
        }

        Long CTableCMap::GetGlyphIndex(void* pFormat, ULong chLongCode)
        {
            UShort chCode = (UShort)chLongCode;
            
           //for(int i = 0; i<numTables; i++)
            //{
            //    if(subTables[i].platformID == platformId && subTables[i].encodingID == encodingId)
            //    {
                    CMapFormat* pFmt = (CMapFormat*)pFormat;
                    switch(pFmt->format)
                    //switch(subTables[i].format->format)
                    {
                    case 0:
                        if(chCode > 255) return 0; // missing glyph
                        return ((CMapFormat0*)pFmt)->glyphIdArray[chCode];
                    case 2:
                        {

                            CMapFormat2 *fmt = (CMapFormat2*)pFmt;
                            Byte byte1 = (Byte)(chCode >> 8);
                            Byte byte2 = (Byte)(chCode & 0xFF);
                            UShort nGlyph = 0;
                            UShort subHeaderIndex = fmt->subHeaderKeys[byte1] / 8;
                            CMapFormat2::CFormat2SubHeader *pSubHeader = &fmt->pSubHeader[subHeaderIndex];
                            if(subHeaderIndex == 0) // one-byte char
                            {
                                if(byte1 >= pSubHeader->firstCode && byte1 < pSubHeader->firstCode + pSubHeader->entryCount)
                                {
                                    return fmt->pGlyphIdArray[pSubHeader->idRangeOffset - fmt->startOfGlyphs + byte1 - pSubHeader->firstCode];
                                }
                                else return 0;
                            }
                            else // two-byte char
                            {
                                if(byte2 >= pSubHeader->firstCode && byte2 < pSubHeader->firstCode + pSubHeader->entryCount)
                                {
                                    return fmt->pGlyphIdArray[pSubHeader->idRangeOffset - fmt->startOfGlyphs + byte2 - pSubHeader->firstCode];
                                }
                                else return 0;
                            }
                        }
                        break;
                    case 4:
                        {
                            CMapFormat4 *fmt = (CMapFormat4*)pFmt;
                            int segCount = 0;
                            for(; segCount<fmt->segCount; segCount++)
                            {
                                if(fmt->pEndCount[segCount] == 0xFFFF) return 0;
                                if(fmt->pEndCount[segCount] >= chCode && fmt->pStartCount[segCount] <= chCode)
                                    break;
                            }

                            if(fmt->pIdRangeOffset[segCount] == 0)
                                return chCode + fmt->pIdDelta[segCount];
                            else
                            {
                                Long index = fmt->pSegGlyphOffset[segCount] + chCode - fmt->pStartCount[segCount];
                                if(index >= fmt->numRangeGlyphs) return 0;
                                return fmt->pGlyphIdArray[index];
                            }
                        }
                    case 6:
                        {
                            CMapFormat6 *fmt = (CMapFormat6*)pFmt;
                            if(chCode >= fmt->firstCode && chCode < fmt->firstCode + fmt->entryCount)
                                return fmt->pGlyphIdArray[chCode - fmt->firstCode];
                            else return 0;
                        }
                    case 8:
                        {
                            CMapFormat8 *fmt = (CMapFormat8*)pFmt;
                            for(ULong j = 0; j<fmt->numGroups; j++)
                            {
                                if(chLongCode >= fmt->pMapGroup[j].startCharCode && chLongCode <= fmt->pMapGroup[j].endCharCode)
                                    return fmt->pMapGroup[j].startGlyphID + chLongCode - fmt->pMapGroup[j].startCharCode;
                            }
                            return 0;
                        }
                    case 10:
                        {
                            CMapFormat10 *fmt = (CMapFormat10*)pFmt;
                            if(chLongCode >= fmt->startCharCode && chLongCode < fmt->startCharCode + fmt->numChars)
                                return fmt->pGlyphs[chLongCode - fmt->startCharCode];
                            else return 0;
                        }
                    case 12:
                        {
                            CMapFormat12 *fmt = (CMapFormat12*)pFmt;
                            for(ULong j = 0; j<fmt->numGroups; j++)
                            {
                                if(chLongCode >= fmt->pGroups[j].startCharCode && chLongCode <= fmt->pGroups[j].endCharCode)
                                    return fmt->pGroups[j].startGlyphID + chLongCode - fmt->pGroups[j].startCharCode;
                            }
                            return 0;
                        }
                    case 13:
                        {
                            CMapFormat12 *fmt = (CMapFormat12*)pFmt;
                            for(ULong j = 0; j<fmt->numGroups; j++)
                            {
                                if(chLongCode >= fmt->pGroups[j].startCharCode && chLongCode <= fmt->pGroups[j].endCharCode)
                                    return fmt->pGroups[j].startGlyphID;
                            }
                            return 0;
                        }
                    }
            //    }
            //}
            return 0; // missing glyph
        }

        void* CTableCMap::GetFormat(int platformId, int encodingId)
        {
            for(int i = 0; i<numTables; i++)
            {
                if(subTables[i].platformID == platformId && subTables[i].encodingID == encodingId)
                    return subTables[i].format;
            }
            return NULL;
        }

        Byte CTableCMap::BytesInChar(void* pFormat, Char *s)
        {
            //for(int i = 0; i<numTables; i++)
            //{
            //    if(subTables[i].platformID == platformId && subTables[i].encodingID == encodingId)
            //    {
                    CMapFormat* pFmt = (CMapFormat*)pFormat;
                    switch(pFmt->format)
                    {
                    case 0:
                        return 1;
                    case 2:
                        if(((CMapFormat2*)pFmt)->subHeaderKeys[s[0]] / 8 > 0) return 2;
                        else return 1;
                    case 4:
                    case 6:
                        return 2;
                    case 8:
                        if(((CMapFormat8*)pFmt)->is32Bit(*(UShort*)s)) return 4;
                        else return 2;
                    case 10:
                    case 12:
                    case 13:
                        return 4;
                    }
            //    }
            //}
            return 1;
        }

        int CTableCMap::GetFirstCharacter(void *pCMap)
        {
            CMapFormat* pFmt = (CMapFormat*)pCMap;
            switch(pFmt->format)
            {
            case 0:
                pFmt->rtOff = 0;
                return ((CMapFormat0*)pFmt)->glyphIdArray[pFmt->rtOff];
            case 2:
                if(((CMapFormat2*)pFmt)->pSubHeader == NULL) return -1;
                pFmt->rtOff = 0;
                pFmt->rtOff1 = 0;
                return ((CMapFormat2*)pFmt)->pSubHeader[pFmt->rtOff1].firstCode + pFmt->rtOff;
            case 4:
                if(((CMapFormat4*)pFmt)->pStartCount == NULL) return -1;
                pFmt->rtOff = 0;
                pFmt->rtOff1 = 0;
                return ((CMapFormat4*)pFmt)->pStartCount[pFmt->rtOff1] + pFmt->rtOff;
            case 6:
                pFmt->rtOff = 0;
                return ((CMapFormat6*)pFmt)->firstCode + pFmt->rtOff;
            case 8:
                if(((CMapFormat8*)pFmt)->pMapGroup == NULL) return -1;
                pFmt->rtOff = 0;
                pFmt->rtOff1 = 0;
                return ((CMapFormat8*)pFmt)->pMapGroup[pFmt->rtOff1].startCharCode + pFmt->rtOff;
            case 10:
                pFmt->rtOff = 0;
                return ((CMapFormat10*)pFmt)->startCharCode + pFmt->rtOff;
            case 12:
            case 13:
                if(((CMapFormat12*)pFmt)->pGroups == NULL) return -1;
                pFmt->rtOff = 0;
                pFmt->rtOff1 = 0;
                return ((CMapFormat12*)pFmt)->pGroups[pFmt->rtOff1].startCharCode + pFmt->rtOff;
            }
            return -1;
        }

        int CTableCMap::GetNextCharacter(void *pCMap)
        {
            CMapFormat* pFmt = (CMapFormat*)pCMap;
            switch(pFmt->format)
            {
            case 0:
                pFmt->rtOff++;
                if(pFmt->rtOff >= 256) return -1;
                return ((CMapFormat0*)pFmt)->glyphIdArray[pFmt->rtOff];
            case 2:
                {
                    CMapFormat2 *pFormat = (CMapFormat2*)pFmt;
                    if(pFormat->pSubHeader == NULL || pFmt->rtOff1 >= pFormat->maxNumHeaders) return -1;
                    pFmt->rtOff++;
                    if(pFmt->rtOff >= pFormat->pSubHeader[pFmt->rtOff1].entryCount)
                    {
                        pFmt->rtOff1++;
                        pFmt->rtOff = 0;
                        if(pFmt->rtOff1 >= pFormat->maxNumHeaders) return -1;
                    }
                    return pFormat->pSubHeader[pFmt->rtOff1].firstCode + pFmt->rtOff;
                }
            case 4:
                {
                    CMapFormat4 *pFormat = (CMapFormat4*)pFmt;
                    if(pFormat->pStartCount == NULL || pFmt->rtOff1 >= pFormat->segCount) return -1;
                    pFmt->rtOff++;
                    if(pFormat->pStartCount[pFmt->rtOff1] + pFmt->rtOff > pFormat->pEndCount[pFmt->rtOff1])
                    {
                        pFmt->rtOff1++;
                        pFmt->rtOff = 0;
                        if(pFmt->rtOff1 >= pFormat->segCount) return -1;
                    }
                    if(pFormat->pEndCount[pFmt->rtOff1] == 0xFFFF)
                        return -1;
                    return pFormat->pStartCount[pFmt->rtOff1] + pFmt->rtOff;
                }
            case 6:
                {
                    CMapFormat6 *pFormat = (CMapFormat6*)pFmt;
                    pFmt->rtOff++;
                    if(pFmt->rtOff >= pFormat->entryCount) return -1;
                    return pFormat->firstCode + pFmt->rtOff;
                }
            case 8:
                {
                    CMapFormat8 *pFormat = (CMapFormat8*)pFmt;
                    if(pFormat->pMapGroup == NULL || pFmt->rtOff1 >= (int)pFormat->numGroups) return -1;
                    pFmt->rtOff++;
                    if(pFormat->pMapGroup[pFmt->rtOff1].startCharCode + pFmt->rtOff > (int)pFormat->pMapGroup[pFmt->rtOff1].endCharCode)
                    {
                        pFmt->rtOff1++;
                        if(pFmt->rtOff1 >= (int)pFormat->numGroups) return -1;
                        pFmt->rtOff = 0;
                    }
                    return pFormat->pMapGroup[pFmt->rtOff1].startCharCode + pFmt->rtOff;
                }
            case 10:
                pFmt->rtOff++;
                if(pFmt->rtOff >= (int)((CMapFormat10*)pFmt)->numChars) return -1;
                return ((CMapFormat10*)pFmt)->startCharCode + pFmt->rtOff;
            case 12:
            case 13:
                {
                    CMapFormat12 *pFormat = (CMapFormat12*)pFmt;
                    if(pFormat->pGroups == NULL || pFmt->rtOff1 >= (int)pFormat->numGroups) return -1;
                    pFmt->rtOff++;
                    if(pFormat->pGroups[pFmt->rtOff1].startCharCode + pFmt->rtOff > (int)pFormat->pGroups[pFmt->rtOff1].endCharCode)
                    {
                        pFmt->rtOff1++;
                        if(pFmt->rtOff1 >= (int)pFormat->numGroups) return -1;
                        pFmt->rtOff = 0;
                    }
                    return pFormat->pGroups[pFmt->rtOff1].startCharCode + pFmt->rtOff;
                }
            }
            return -1;
        }

        int CTableCMap::GetNumCharacters(void *pCMap)
        {
            int count = 0;
            int res = GetFirstCharacter(pCMap);
            while(res != -1)
            {
                count++;
                res = GetNextCharacter(pCMap);
            }
            return count;
        }

    };

};