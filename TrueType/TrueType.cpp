// TrueType.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TrueType.h"
#include <vector>
#include "RealPoint.h"
#include "BezierCurve.h"
#include "Instruction.h"
#include "TTFontFace.h"
#include "InstrCmd.h"
#include "Matrix.h"
#include "Raster.h"

extern int g_FE_Platform;


namespace FF_Font_Factory
{
    namespace FF_Font_Factory_TrueType
    {
        CInstrCommand TT_CommandList;

        CTrueTypeFont::CTrueTypeFont()
            :tableEntries(0), pTableHead(0), pTableMaxp(0), pTableName(0), pTableHhea(0), pTablePost(0), pTableGlyf(0), pTableLoca(0), pTableGasp(0), pTableHdmx(0), pTableKern(0),
            pTableVhea(0), pTableVmtx(0), 
            pTableOS2(0), pTableCMap(0), nCvt(0), pCvt(0), pFpgm(0), nFpgm(0), nPrep(0), pPrep(0),
            pStartGState(0), numTables(0)
        {
        }

        CTrueTypeFont::~CTrueTypeFont()
        {
            //if(fp) fclose(fp);
            if(tableEntries) delete[] tableEntries;
            if(pTableOS2) delete pTableOS2;
            if(pTableCMap) delete pTableCMap;
            if(pCvt) delete[] pCvt;
            if(pFpgm) delete[] pFpgm;
            if(pPrep) delete[] pPrep;
            if(pTableHead) delete pTableHead;
            if(pTableMaxp) delete pTableMaxp;
            if(pTableName) delete pTableName;
            if(pTableHhea) delete pTableHhea;
            if(pTableHmtx) delete pTableHmtx;
            if(pTablePost) delete pTablePost;
            if(pTableGlyf) delete pTableGlyf;
            if(pTableLoca) delete pTableLoca;
            if(pTableGasp) delete pTableGasp;
            if(pTableHdmx) delete pTableHdmx;
            if(pTableKern) delete pTableKern;
            if(pTableVhea) delete pTableVhea;
            if(pTableVmtx) delete pTableVmtx;


            if(pStartGState) delete (CGState*)pStartGState;

            std::map<ULong, TT_NestedFunc*>::iterator it;
            for(it = functionDefs.begin(); it != functionDefs.end(); it++)
                delete it->second;
            functionDefs.clear();
        }

        UShort CTrueTypeFont::ToUShort(Byte *buf)
        {
            return buf[1] + (buf[0] << 8);
        }

        Short CTrueTypeFont::ToShort(Byte *buf)
        {
            return buf[1] + (buf[0] << 8);
        }

        ULong CTrueTypeFont::ToULong(Byte *buf)
        {
            return buf[3] + (buf[2] << 8) + (buf[1] << 16) + (buf[0] << 24);
        }

        Long CTrueTypeFont::ToLong(Byte *buf)
        {
            return buf[3] + (buf[2] << 8) + (buf[1] << 16) + (buf[0] << 24);
        }

        float CTrueTypeFont::FixedToFloat(Byte *buf)
        {
            Long v = ToLong(buf);
            return _FixedToFloat(v);
            //Short mantissa = (Short)(v >> 16);
            //float res = (v & 0xFFFF) / 65536.0f;
            //if((mantissa & 0x8000)) res = mantissa - res; // negative
            //else res = mantissa - res; // positive
            //return res;
        }

        Long CTrueTypeFont::FloatToFixed(float f)
        {
            return _FloatToFixed(f);
            //Long v = (Short)f;
            //float fraction = f - (float)v;
            //v = (v << 16) + (Short)(fraction * 65536);
            //return v;
        }

        float CTrueTypeFont::F2DOT14ToFloat(Byte *buf)
        {
            Short v = ToShort(buf);
            return _F2DOT14ToFloat(v);
            //float res = ((v >> 14) & 1) + (v & 0x3FFF) / 16384.0f;
            //if(v >> 15) res = -res;
            //return res;
        }

        Short CTrueTypeFont::FloatToF2DOT14(float f)
        {
            return _FloatToF2DOT14(f);
            //Long v = (Short)f;
            //float fraction = f - (float)v;
            //v = (v << 14) + (Short)(fraction * 16384);
            //return v;
        }

        Byte CTrueTypeFont::ReadByte()
        {
            char buf[1];
            size_t byts = ReadBytes(buf, 1);
            return (Byte)buf[0];
        }
        Char CTrueTypeFont::ReadChar()
        {
            char buf[1];
            size_t byts = ReadBytes(buf, 1);
            return buf[0];
        }
        UShort CTrueTypeFont::ReadUShort()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToUShort((Byte*)buf);
        }
        Short CTrueTypeFont::ReadShort()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToShort((Byte*)buf);
        }
        ULong CTrueTypeFont::Read3Bytes()
        {
            char buf[3];
            size_t byts = ReadBytes(buf, 3);
            return buf[2] + (buf[1] << 8) + (buf[0] << 16);
        }
        ULong CTrueTypeFont::ReadULong()
        {
            char buf[4];
            size_t byts = ReadBytes(buf, 4);
            return ToULong((Byte*)buf);
        }
        Long CTrueTypeFont::ReadLong()
        {
            char buf[4];
            size_t byts = ReadBytes(buf, 4);
            return ToLong((Byte*)buf);
        }
        UShort CTrueTypeFont::ReadFUWord()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToUShort((Byte*)buf);
        }
        Short CTrueTypeFont::ReadFWord()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return ToShort((Byte*)buf);
        }
        float CTrueTypeFont::ReadFixed()
        {
            char buf[4];
            size_t byts = ReadBytes(buf, 4);
            return FixedToFloat((Byte*)buf);
        }
        float CTrueTypeFont::ReadF2Dot14()
        {
            char buf[2];
            size_t byts = ReadBytes(buf, 2);
            return F2DOT14ToFloat((Byte*)buf);
        }

        CTableEntry* CTrueTypeFont::FindTable(TABLE_TAG tag)
        {
            for(UShort i = 0; i< numTables; i++)
            {
                if(tableEntries[i].tag == tag)
                    return &tableEntries[i];
            }
            return NULL;
        }

        BOOL CTrueTypeFont::LoadFontFile(wchar_t *fileName)
        {
            if(!CBaseFont::LoadFontFile(fileName)) return false;

            Seek(0, SEEK_SET);

            // Table Directory //
            sfntVersion = ReadFixed();
            numTables = ReadUShort();
            searchRange = ReadUShort();
            entrySelector = ReadUShort();
            rangeShift = ReadUShort();

            // Read table entries //
            if(tableEntries) delete[] tableEntries;
            tableEntries = NULL;
            if(numTables > 0)
                tableEntries = new CTableEntry[numTables];
            for(UShort i = 0; i< numTables; i++)
            {
                ReadTables(&tableEntries[i]);
            }

            // Read table //
            CTableEntry *pTab = FindTable(TAG_head);
            if(pTab)
            {
                pTableHead = new CTableHead();
                pTableHead->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_maxp);
            if(pTab)
            {
                pTableMaxp = new CTableMaxp();
                pTableMaxp->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_name);
            if(pTab)
            {
                pTableName = new CTableName();
                pTableName->Load(this, pTab->offset);
                m_pFontName = GetFontName();
            }
            pTab = FindTable(TAG_hhea);
            if(pTab)
            {
                pTableHhea = new CTableHhea();
                pTableHhea->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_hmtx);
            if(pTab)
            {
                pTableHmtx = new CTableHmtx();
                if(pTableHhea) pTableHmtx->numMetrix = pTableHhea->numOfLongHorMetrics;
                if(pTableHhea && pTableMaxp) pTableHmtx->numLeftSideBearing = pTableMaxp->numGlyphs - pTableHhea->numOfLongHorMetrics;
                pTableHmtx->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_cmap);
            if(pTab)
            {
                pTableCMap = new CTableCMap();
                pTableCMap->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_loca);
            if(pTab)
            {
                pTableLoca = new CTableLoca();
                if(pTableMaxp) pTableLoca->numGlyphs = pTableMaxp->numGlyphs;
                if(pTableHead) pTableLoca->bIsShortFormat = (pTableHead->indexToLocFormat == 0);
                pTableLoca->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_glyf);
            if(pTab)
            {
                pTableGlyf = new CTableGlyf();
                if(pTableMaxp) pTableGlyf->numGlyphs = pTableMaxp->numGlyphs;
                if(pTableLoca) pTableGlyf->Load(this, pTab->offset, pTableLoca->pOffset);
            }
            pTab = FindTable(TAG_post);
            if(pTab)
            {
                pTablePost = new CTablePost();
                pTablePost->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_gasp);
            if(pTab)
            {
                pTableGasp = new CTableGasp();
                pTableGasp->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_hdmx);
            if(pTab)
            {
                pTableHdmx = new CTableHdmx();
                if(pTableMaxp) pTableHdmx->numGlyphs = pTableMaxp->numGlyphs;
                pTableHdmx->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_kern);
            if(pTab)
            {
                pTableKern = new CTableKern();
                pTableKern->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_vhea);
            if(pTab)
            {
                pTableVhea = new CTableVhea();
                pTableVhea->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_vmtx);
            if(pTab)
            {
                pTableVmtx = new CTableVmtx();
                if(pTableVhea) pTableVmtx->numMetrix = pTableVhea->numOfLongVerMetrics;
                if(pTableVhea && pTableMaxp) pTableVmtx->numTopSideBearing = pTableMaxp->numGlyphs - pTableVhea->numOfLongVerMetrics;
                pTableVmtx->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_os2);
            if(pTab)
            {
                pTableOS2 = new CTableOS2();
                pTableOS2->Load(this, pTab->offset);
            }
            pTab = FindTable(TAG_cvt);
            if(pTab)
            {
                Seek(pTab->offset, SEEK_SET);
                nCvt = pTab->length / sizeof(FWord);
                if(nCvt > 0)
                {
                    pCvt = new Long[nCvt];
                    for(ULong j = 0; j < nCvt; j++)
                        pCvt[j] = ReadFWord();
                }
            }
            pTab = FindTable(TAG_prep);
            if(pTab)
            {
                Seek(pTab->offset, SEEK_SET);
                nPrep = pTab->length / sizeof(Byte);
                if(nPrep > 0)
                {
                    pPrep = new Byte[nPrep];
                    ReadBytes((char*)pPrep, nPrep);
                }
            }
            pTab = FindTable(TAG_fpgm);
            if(pTab)
            {
                Seek(pTab->offset, SEEK_SET);
                nFpgm = pTab->length / sizeof(Byte);
                if(nFpgm > 0)
                {
                    pFpgm = new Byte[nFpgm];
                    ReadBytes((char*)pFpgm, nFpgm);
                }
            }

            // run font program
            if (pTableMaxp)
                pStartGState = RunFontProgram(nFpgm, pFpgm, nCvt, pCvt, pTableMaxp->maxStorage, &functionDefs);

            return true;
        }


        BOOL CTrueTypeFont::ReadTables(CTableEntry *pTable)
        {
            if(pTable == NULL) return false;
            char tag[5];
            tag[0] = ReadByte();
            tag[1] = ReadByte();
            tag[2] = ReadByte();
            tag[3] = ReadByte();
            tag[4] = 0;
            ReadULong(); // bypass checkSum
            pTable->offset = ReadULong();
            pTable->length = ReadULong();

            if(strcmp(tag, "cmap") == 0)
                pTable->tag = TAG_cmap;
            else if(strcmp(tag, "glyf") == 0)
                pTable->tag = TAG_glyf;
            else if(strcmp(tag, "head") == 0)
                pTable->tag = TAG_head;
            else if(strcmp(tag, "hhea") == 0)
                pTable->tag = TAG_hhea;
            else if(strcmp(tag, "hmtx") == 0)
                pTable->tag = TAG_hmtx;
            else if(strcmp(tag, "loca") == 0)
                pTable->tag = TAG_loca;
            else if(strcmp(tag, "maxp") == 0)
                pTable->tag = TAG_maxp;
            else if(strcmp(tag, "name") == 0)
                pTable->tag = TAG_name;
            else if(strcmp(tag, "post") == 0)
                pTable->tag = TAG_post;
            else if(strcmp(tag, "OS/2") == 0)
                pTable->tag = TAG_os2;
            else if(strcmp(tag, "cvt") == 0 || strcmp(tag, "cvt ") == 0)
                pTable->tag = TAG_cvt;
            else if(strcmp(tag, "EBDT") == 0)
                pTable->tag = TAG_ebdt;
            else if(strcmp(tag, "EBLC") == 0)
                pTable->tag = TAG_eblc;
            else if(strcmp(tag, "EBSC") == 0)
                pTable->tag = TAG_ebsc;
            else if(strcmp(tag, "fpgm") == 0)
                pTable->tag = TAG_fpgm;
            else if(strcmp(tag, "gasp") == 0)
                pTable->tag = TAG_gasp;
            else if(strcmp(tag, "hdmx") == 0)
                pTable->tag = TAG_hdmx;
            else if(strcmp(tag, "kern") == 0)
                pTable->tag = TAG_kern;
            else if(strcmp(tag, "LTSH") == 0)
                pTable->tag = TAG_ltsh;
            else if(strcmp(tag, "prep") == 0)
                pTable->tag = TAG_prep;
            else if(strcmp(tag, "PCLT") == 0)
                pTable->tag = TAG_pclt;
            else if(strcmp(tag, "VDMX") == 0)
                pTable->tag = TAG_vdmx;
            else if(strcmp(tag, "vhea") == 0)
                pTable->tag = TAG_vhea;
            else if(strcmp(tag, "vmtx") == 0)
                pTable->tag = TAG_vmtx;

            return true;
        }

        BOOL CTrueTypeFont::IndexedByCid()
        {
            if (pTableCMap == NULL) return FALSE;
            return TRUE;
        }

        FE_CMap CTrueTypeFont::GetCMap(int &platformId, int &encodingId)
        {
            if(pTableCMap == NULL) return NULL;

            FE_CMap pCMap = NULL;

            if(platformId == FE_PLATFORM_ID_Default)
            {
                if(g_FE_Platform == FE_PLATFORM_ID_Unicode)
                {
                    for(int i = 0; i<7; i++)
                    {
                        pCMap = pTableCMap->GetFormat(FE_PLATFORM_ID_Unicode, i);
                        if(pCMap)
                        {
                            platformId = FE_PLATFORM_ID_Unicode;
                            encodingId = i;
                            return pCMap;
                        }
                    }
                    return NULL;
                }
                else if(g_FE_Platform == FE_PLATFORM_ID_Windows)
                {
                    pCMap = pTableCMap->GetFormat(FE_PLATFORM_ID_Windows, FE_WIN_ENCODING_UCS2);
                    if(pCMap)
                    {
                        platformId = FE_PLATFORM_ID_Windows;
                        encodingId = FE_WIN_ENCODING_UCS2;
                        return pCMap;
                    }
                    for(int i = 0; i<6; i++)
                    {
                        pCMap = pTableCMap->GetFormat(FE_PLATFORM_ID_Windows, i);
                        if(pCMap)
                        {
                            platformId = FE_PLATFORM_ID_Windows;
                            encodingId = i;
                            return pCMap;
                        }
                    }
                    pCMap = pTableCMap->GetFormat(FE_PLATFORM_ID_Windows, 10);
                    if(pCMap)
                    {
                        platformId = FE_PLATFORM_ID_Windows;
                        encodingId = 10;
                        return pCMap;
                    }
                    return NULL;
                }
                //(g_FE_Platform == FE_PLATFORM_ID_Mac) : Mac is not supported in TrueType font

                return NULL;
            }
            return pTableCMap->GetFormat(platformId, encodingId);
        }

        int CTrueTypeFont::GetFirstCharacter(FE_CMap pCMap)
        {
            if(pTableCMap == NULL) return -1;
            
            return pTableCMap->GetFirstCharacter(pCMap);
        }

        int CTrueTypeFont::GetNextCharacter(FE_CMap pCMap)
        {
            if(pTableCMap == NULL) return -1;
            
            return pTableCMap->GetNextCharacter(pCMap);
        }
        int CTrueTypeFont::GetFirstGlyph()
        {
            m_nCurGlyphIndex = 0;
            if(pTableGlyf && pTableGlyf->numGlyphs == 0) return -1;
            return 0;
        }
        int CTrueTypeFont::GetNextGlyph()
        {
            m_nCurGlyphIndex++;
            if(pTableGlyf && m_nCurGlyphIndex < pTableGlyf->numGlyphs) return m_nCurGlyphIndex;
            else return -1;
        }

        int CTrueTypeFont::GetGlyphIndex(FE_CMap pCMap, unsigned int cid)
        {
            if(!pCMap || !pTableCMap || !pTableGlyf) return -1;

            return pTableCMap->GetGlyphIndex(pCMap, cid);
        }

        CGlyphOutlines* CTrueTypeFont::GetOutlines(int glyphIndex)
        {
            if(glyphIndex == 0 || glyphIndex == -1) return NULL;

            CGlyphDef *pGlyph = pTableGlyf->GetGlyphObj(glyphIndex);
            if(!pGlyph) return NULL;

            return pGlyph->GetOutline();
        }

        CGlyphOutlines* CTrueTypeFont::GetOutlines(FE_CMap pCMap, unsigned int cid)
        {
            Long glyphIndex = GetGlyphIndex(pCMap, cid);
            return GetOutlines(glyphIndex);
        }


        wchar_t* CTrueTypeFont::GetFontName()
        {
            if(m_pFontName) return m_pFontName;

            if(pTableName)
            {
                for(int i = 0; i<pTableName->count; i++)
                {
                    if(pTableName->pNameRecord[i].nameID == 3)
                    {
                        if(pTableName->pNameRecord[i].platformID == FE_PLATFORM_ID_Windows || pTableName->pNameRecord[i].platformID == FE_PLATFORM_ID_Unicode)
                        {
                            size_t slen = wcslen((wchar_t*)pTableName->pNameRecord[i].pString);
                            wchar_t *pName = new wchar_t[slen + 1];
                            wcscpy_s(pName, slen+1, (wchar_t*)pTableName->pNameRecord[i].pString);
                            return pName;
                        }
                        else if(pTableName->pNameRecord[i].platformID == FE_PLATFORM_ID_Mac)
                        {
                            return Char2Wchar(pTableName->pNameRecord[i].pString, false);
                        }
                    }
                }
            }
            return NULL;
        }


        /*CGlyphOutlines* CTrueTypeFont::BuildGlyphOutline(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef, CMatrix3x3 &mat)
        {
            CGlyphOutlines *pOutlines = BuildGlyphOrigOutline(pFace, glyphIndex, pGlyphDef);

            if (!pOutlines) return nullptr;
            if (pOutlines->numContours == 0)
            {
                delete pOutlines;
                return nullptr;
            }

            int advanceWidth = (int)(GetAdvanceWidth(glyphIndex));// *pFace->m_fScale);
            int lsb = (int)(GetLeftSideBearing(glyphIndex));// *pFace->m_fScale);
            //RunGlyphInstruction_new(pFace, aw, lsb, pGlyphDef, pCompoundOutlines);
            CGlyphOutlines *pOrigOutlines = NULL;
            AppendPhantomPointsAndScale(pOutlines, advanceWidth, lsb, (int)pGlyphDef->xMin, &pOrigOutlines, pFace->m_fScale);

            Byte *pInstr;
            UShort nInstr = pGlyphDef->GetInstructions(0, &pInstr);
            if (nInstr > 0 && pInstr)
            {
                CGState *pGState = (CGState*)pFace->m_pStartGState;
                if (pGState->instrControl & TTIC_IGNORE_CVT_CHANGES)
                    pGState->Reset();

                if ((pGState->instrControl & TTIC_NOT_RUN_GLYPH_INSTRUCTS) == 0)
                    RunInstrs(pGState, true, nInstr, pInstr, nCvt, pFace->m_pCvt, pTableMaxp->maxTwilightPoints, pOrigOutlines, pOutlines);
            }

            pOutlines->numPoints -= 4;

            if (pOrigOutlines)
                delete pOrigOutlines;

            return pOutlines;

        }*/
        CGlyphOutlines* CTrueTypeFont::BuildGlyphOutline(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef, CMatrix3x3 &mat)//, bool bNoInstrScale)
        {
            if (pGlyphDef == nullptr) return nullptr;

            CGlyphOutlines *pOrigOutlines = nullptr;
            CGlyphOutlines *pOutlines = nullptr;

            if (pGlyphDef->numberOfContours != -1) // simple glyph
            {
                pOutlines = PrepareSimpleGlyphOutlines(pFace, glyphIndex, pGlyphDef, &pOrigOutlines);
            }
            else // compound glyph
            {
                CCompoundGlyph *pCompoundGlyph = (CCompoundGlyph*)pGlyphDef;
                int size = (int)pCompoundGlyph->components.size();

                CGlyphOutlines *pCompoundOutlines = new CGlyphOutlines();
                pCompoundOutlines->numContours = 0;
                pCompoundOutlines->numPoints = 0;
                // sum up points of all sub-glyphs
                for (int i = 0; i < size; i++)
                {
                    CGlyphDef *pComponentGlyph = pTableGlyf->GetGlyphObj(pCompoundGlyph->components[i]->glyphid);
                    pOutlines = pComponentGlyph->GetOutline();
                    if (pOutlines)
                    {
                        pCompoundOutlines->numContours += pOutlines->numContours;
                        pCompoundOutlines->numPoints += pOutlines->numPoints;
                    }
                }

                pCompoundOutlines->pEndPtsOfContours = new unsigned int[pCompoundOutlines->numContours];
                pCompoundOutlines->pCoordinates = new CGlyphCoordinate[pCompoundOutlines->numPoints + 4]; // add 4 Phantom points

                // process sub-glyph one by one
                int curContour = 0, curPoint = 0;
                for (int i = 0; i < size; i++)
                {
                    CCompoundGlyph::CComponent *pComponent = pCompoundGlyph->components[i];
                    CGlyphDef *pSubGlyphDef = pTableGlyf->GetGlyphObj(pComponent->glyphid);

                    // translate component glyph from offsets or anchor points to compound glyph coordinate system
                    CMatrix3x3 sub_mat;
                    GetSubglyphMatrix(pComponent, pCompoundOutlines, pOutlines, sub_mat);

                    //pOutlines = BuildGlyphOrigOutline(pFace, pComponent->glyphid, pSubGlyphDef);
                    pOutlines = BuildGlyphOutline(pFace, pComponent->glyphid, pSubGlyphDef, sub_mat);

                    if (pOutlines == nullptr)
                    {
                        delete pCompoundOutlines;
                        return nullptr;
                    }
                    
                    // copy over to compound glyph
                    for (int j = 0; j < pOutlines->numContours; j++, curContour++)
                    {
                        pCompoundOutlines->pEndPtsOfContours[curContour] = curPoint + pOutlines->pEndPtsOfContours[j];
                    }
                    for (int j = 0; j< pOutlines->numPoints; j++, curPoint++)
                    {
                        pCompoundOutlines->pCoordinates[curPoint].x = pOutlines->pCoordinates[j].x;
                        pCompoundOutlines->pCoordinates[curPoint].y = pOutlines->pCoordinates[j].y;
                        pCompoundOutlines->pCoordinates[curPoint].bOnCurve = pOutlines->pCoordinates[j].bOnCurve;
                    }

                    delete pOutlines;
                }

                pOutlines = pCompoundOutlines;
            }

            if (pOutlines == nullptr) return nullptr;
            if (pOutlines->numContours == 0)
            {
                delete pOutlines;
                return nullptr;
            }

            Byte *pInstr;
            //UShort nInstr = pGlyphDef->GetInstructions(0, &pInstr);
            UShort nInstr = pGlyphDef->GetInstructions(&pInstr);
            if (nInstr > 0 && pInstr)
            {
                CGState *pGState = (CGState*)pFace->m_pStartGState;
                if (pGState->instrControl & TTIC_IGNORE_CVT_CHANGES)
                    pGState->Reset();

                if ((pGState->instrControl & TTIC_NOT_RUN_GLYPH_INSTRUCTS) == 0)
                    RunInstrs(pGState, true, nInstr, pInstr, nCvt, pFace->m_pCvt, pTableMaxp->maxTwilightPoints, pOrigOutlines, pOutlines);
            }

            pOutlines->numPoints -= 4;

            if (pOrigOutlines)
                delete pOrigOutlines;

            if (!mat.IsIdentity())
            {
                mat.mat[2][0] *= pFace->m_fScale;
                mat.mat[2][1] *= pFace->m_fScale;
                CRealPoint pt;
                for (int i = 0; i < pOutlines->numPoints; i++)
                {
                    pt.x = pOutlines->pCoordinates[i].x;
                    pt.y = pOutlines->pCoordinates[i].y;
                    pt = pt * mat;
                    pOutlines->pCoordinates[i].x = pt.x;
                    pOutlines->pCoordinates[i].y = pt.y;
                }
            }

            // convert curve to points
            //ConvertCurveToPoints(pOutlines);

            return pOutlines;

        }
        
        void CTrueTypeFont::GetSubglyphMatrix(CCompoundGlyph::CComponent *pComponent, CGlyphOutlines *pCompoundOutlines, CGlyphOutlines *pOutlines, CMatrix3x3 &mat)
        {
            if (pComponent->isPoints)
            {
                pComponent->e = pCompoundOutlines->pCoordinates[pComponent->arg1].x - pOutlines->pCoordinates[pComponent->arg2].x;
                pComponent->f = pCompoundOutlines->pCoordinates[pComponent->arg1].y - pOutlines->pCoordinates[pComponent->arg2].y;
            }
            else
            {
                float m = max(abs(pComponent->a), abs(pComponent->b));
                if (abs(abs(pComponent->a) - abs(pComponent->c)) <= (33.0f / 65536))
                    m *= 2;

                float n = max(abs(pComponent->c), abs(pComponent->d));
                if (abs(abs(pComponent->b) - abs(pComponent->d)) <= (33.0f / 65536))
                    n *= 2;

                pComponent->e = m * pComponent->arg1;
                pComponent->f = n * pComponent->arg2;
            }

            mat.Set(pComponent->a, pComponent->b,
                pComponent->c, pComponent->d,
                pComponent->e, pComponent->f);
        }

        CGlyphOutlines* CTrueTypeFont::PrepareSimpleGlyphOutlines(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef, CGlyphOutlines **pOrigOutlines)
        {
            CGlyphOutlines *pRawOutlines = pGlyphDef->GetOutline();
            if (pRawOutlines == nullptr || pRawOutlines->numContours == 0) return nullptr;

            CGlyphOutlines * pOutlines = pRawOutlines->Copy(4); // add 4 Phantom points

            if (pOutlines == nullptr) return nullptr;
            if (pOutlines->numContours == 0)
            {
                delete pOutlines;
                return nullptr;
            }

            int advanceWidth = (int)(GetAdvanceWidth(glyphIndex));
            int lsb = (int)(GetLeftSideBearing(glyphIndex));
            PrepareOutlinesForInstrs(pOutlines, advanceWidth, lsb, (int)pGlyphDef->xMin, pOrigOutlines, pFace->m_fScale);

            return pOutlines;
        }

        void CTrueTypeFont::PrepareOutlinesForInstrs(CGlyphOutlines *pOutlines, int advanceWidth, int lsb, int xMin, CGlyphOutlines **pOrigOutlines, float scale)
        {
            if (pOutlines)
            {
                // add 4 Phantom points
                pOutlines->numPoints += 4;

                pOutlines->pCoordinates[pOutlines->numPoints - 4].x = (float)(int)((xMin - lsb));
                pOutlines->pCoordinates[pOutlines->numPoints - 4].y = pOutlines->pCoordinates[pOutlines->numPoints - 3].y = 0;
                pOutlines->pCoordinates[pOutlines->numPoints - 3].x = (float)(int)((xMin - lsb + advanceWidth));

                pOutlines->pCoordinates[pOutlines->numPoints - 1].x = pOutlines->pCoordinates[pOutlines->numPoints - 2].x = .0f;
                if (pTableHhea)
                {
                    pOutlines->pCoordinates[pOutlines->numPoints - 2].y = (float)((int)(pTableHhea->ascender));
                    pOutlines->pCoordinates[pOutlines->numPoints - 1].y = (float)((int)(pTableHhea->descender));
                }
                else if (pTableVhea)
                {
                    pOutlines->pCoordinates[pOutlines->numPoints - 2].y = (float)((int)(pTableVhea->vertTypoAscender));
                    pOutlines->pCoordinates[pOutlines->numPoints - 1].y = (float)((int)(pTableVhea->vertTypoDescender));
                }
                else
                {
                    pOutlines->pCoordinates[pOutlines->numPoints - 2].y = 0;
                    pOutlines->pCoordinates[pOutlines->numPoints - 1].y = 0;
                }
                pOutlines->pCoordinates[pOutlines->numPoints - 4].bOnCurve = PF_HELP;
                pOutlines->pCoordinates[pOutlines->numPoints - 3].bOnCurve = PF_HELP;
                pOutlines->pCoordinates[pOutlines->numPoints - 2].bOnCurve = PF_HELP;
                pOutlines->pCoordinates[pOutlines->numPoints - 1].bOnCurve = PF_HELP;

                // scale the points
                for (int i = 0; i< pOutlines->numPoints; i++)
                {
                    pOutlines->pCoordinates[i].x *= scale;
                    pOutlines->pCoordinates[i].y *= scale;
                }

                if (pOrigOutlines)
                    *pOrigOutlines = pOutlines->Copy();

                // round Phantom points in current outlines to int
                pOutlines->pCoordinates[pOutlines->numPoints - 4].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 4].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 4].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 4].y);
                pOutlines->pCoordinates[pOutlines->numPoints - 3].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 3].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 3].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 3].y);
                pOutlines->pCoordinates[pOutlines->numPoints - 2].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 2].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 2].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 2].y);
                pOutlines->pCoordinates[pOutlines->numPoints - 1].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 1].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 1].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 1].y);
            }
        }

        CGlyphOutlines* CTrueTypeFont::BuildGlyphOrigOutline(CTTFontFace *pFace, int glyphIndex, CGlyphDef *pGlyphDef)//, bool bNoInstrScale)
        {
            CGlyphOutlines *pCompoundOutlines = nullptr;

            if (pGlyphDef->numberOfContours == -1) // compound glyph
            {
                CGlyphOutlines *pOutlines = NULL;
                CCompoundGlyph *pCompoundGlyph = (CCompoundGlyph*)pGlyphDef;
                int size = (int)pCompoundGlyph->components.size();

                pCompoundOutlines = new CGlyphOutlines();
                pCompoundOutlines->numContours = 0;
                pCompoundOutlines->numPoints = 0;
                for (int i = 0; i < size; i++)
                {
                    CGlyphDef *pComponentGlyph = pTableGlyf->GetGlyphObj(pCompoundGlyph->components[i]->glyphid);
                    pOutlines = pComponentGlyph->GetOutline();
                    if (pOutlines)
                    {
                        pCompoundOutlines->numContours += pOutlines->numContours;
                        pCompoundOutlines->numPoints += pOutlines->numPoints;
                    }
                }

                pCompoundOutlines->pEndPtsOfContours = new unsigned int[pCompoundOutlines->numContours];
                pCompoundOutlines->pCoordinates = new CGlyphCoordinate[pCompoundOutlines->numPoints + 4]; // add 4 Phantom points

                int curContour = 0, curPoint = 0;
                for (int i = 0; i < size; i++)
                {
                    CCompoundGlyph::CComponent *pComponent = pCompoundGlyph->components[i];
                    CGlyphDef *pSubGlyphDef = pTableGlyf->GetGlyphObj(pComponent->glyphid);
                    pOutlines = BuildGlyphOrigOutline(pFace, pComponent->glyphid, pSubGlyphDef);

                    if (pOutlines == NULL) continue;

                    // translate component glyph from offsets or anchor points to compound glyph coordinate system
                    if (pComponent->isPoints)
                    {
                        pComponent->e = pCompoundOutlines->pCoordinates[pComponent->arg1].x - pOutlines->pCoordinates[pComponent->arg2].x;
                        pComponent->f = pCompoundOutlines->pCoordinates[pComponent->arg1].y - pOutlines->pCoordinates[pComponent->arg2].y;
                    }
                    else
                    {
                        float m = max(abs(pComponent->a), abs(pComponent->b));
                        if (abs(abs(pComponent->a) - abs(pComponent->c)) <= (33.0f / 65536))
                            m *= 2;

                        float n = max(abs(pComponent->c), abs(pComponent->d));
                        if (abs(abs(pComponent->b) - abs(pComponent->d)) <= (33.0f / 65536))
                            n *= 2;

                        pComponent->e = m * pComponent->arg1;// *pFace->m_fScale;
                        pComponent->f = n * pComponent->arg2;// *pFace->m_fScale;
                    }

                    CMatrix3x3 mat;
                    mat.Set(pComponent->a, pComponent->b,
                        pComponent->c, pComponent->d,
                        pComponent->e, pComponent->f);

                    // copy over to compound glyph
                    for (int j = 0; j < pOutlines->numContours; j++, curContour++)
                    {
                        pCompoundOutlines->pEndPtsOfContours[curContour] = curPoint + pOutlines->pEndPtsOfContours[j];
                    }

                    for (int j = 0; j< pOutlines->numPoints; j++, curPoint++)
                    {
                        CRealPoint pt(pOutlines->pCoordinates[j].x, pOutlines->pCoordinates[j].y);
                        pt = pt * mat;
                        pCompoundOutlines->pCoordinates[curPoint].x = pt.x;
                        pCompoundOutlines->pCoordinates[curPoint].y = pt.y;
                        pCompoundOutlines->pCoordinates[curPoint].bOnCurve = pOutlines->pCoordinates[j].bOnCurve;
                    }

                    delete pOutlines;
                }
            }
            else // simple glyph
            {
                CGlyphOutlines *pRawOutlines = pGlyphDef->GetOutline();
                if (pRawOutlines == NULL) return NULL;

                pCompoundOutlines = pRawOutlines->Copy(4); // add 4 Phantom points
            }

            return pCompoundOutlines;
        }


        void CTrueTypeFont::AppendPhantomPointsAndScale(CGlyphOutlines *pOutlines, int advanceWidth, int lsb, int xMin, CGlyphOutlines **pOrigOutlines, float scale)
        {
            if (pOutlines)
            {
                // add 4 Phantom points
                pOutlines->numPoints += 4;

                pOutlines->pCoordinates[pOutlines->numPoints - 4].x = (float)(int)((xMin - lsb));
                pOutlines->pCoordinates[pOutlines->numPoints - 4].y = pOutlines->pCoordinates[pOutlines->numPoints - 3].y = 0;
                pOutlines->pCoordinates[pOutlines->numPoints - 3].x = (float)(int)((xMin - lsb + advanceWidth));

                pOutlines->pCoordinates[pOutlines->numPoints - 1].x = pOutlines->pCoordinates[pOutlines->numPoints - 2].x = .0f;
                if (pTableHhea)
                {
                    pOutlines->pCoordinates[pOutlines->numPoints - 2].y = (float)((int)(pTableHhea->ascender));
                    pOutlines->pCoordinates[pOutlines->numPoints - 1].y = (float)((int)(pTableHhea->descender));
                }
                else if (pTableVhea)
                {
                    pOutlines->pCoordinates[pOutlines->numPoints - 2].y = (float)((int)(pTableVhea->vertTypoAscender));
                    pOutlines->pCoordinates[pOutlines->numPoints - 1].y = (float)((int)(pTableVhea->vertTypoDescender));
                }
                else
                {
                    pOutlines->pCoordinates[pOutlines->numPoints - 2].y = 0;
                    pOutlines->pCoordinates[pOutlines->numPoints - 1].y = 0;
                }
                pOutlines->pCoordinates[pOutlines->numPoints - 4].bOnCurve = PF_HELP;
                pOutlines->pCoordinates[pOutlines->numPoints - 3].bOnCurve = PF_HELP;
                pOutlines->pCoordinates[pOutlines->numPoints - 2].bOnCurve = PF_HELP;
                pOutlines->pCoordinates[pOutlines->numPoints - 1].bOnCurve = PF_HELP;

                // scale the points
                for (int i = 0; i< pOutlines->numPoints; i++)
                {
                    pOutlines->pCoordinates[i].x *= scale;
                    pOutlines->pCoordinates[i].y *= scale;
                }

                if (pOrigOutlines)
                    *pOrigOutlines = pOutlines->Copy();

                // round Phantom points in current outlines to int
                pOutlines->pCoordinates[pOutlines->numPoints - 4].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 4].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 4].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 4].y);
                pOutlines->pCoordinates[pOutlines->numPoints - 3].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 3].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 3].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 3].y);
                pOutlines->pCoordinates[pOutlines->numPoints - 2].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 2].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 2].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 2].y);
                pOutlines->pCoordinates[pOutlines->numPoints - 1].x = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 1].x);
                pOutlines->pCoordinates[pOutlines->numPoints - 1].y = RoundToInt(pOutlines->pCoordinates[pOutlines->numPoints - 1].y);
            }
        }

        CGlyphOutlines* CTrueTypeFont::Scaler(FE_FontFace hFace, int glyphIndex )
        {
            return nullptr;
        }

        unsigned short CTrueTypeFont::GetUpem()
        {
            if(pTableHead == NULL) return 0;
            return pTableHead->unitsPerEm;
        }

        int CTrueTypeFont::GetNumGlyphs(FE_CMap pCMap)
        {
            if(pTableMaxp) return pTableMaxp->numGlyphs;
            return 0;
        }

        int CTrueTypeFont::GetMaxAdvanceWidth() 
        {
            if(pTableHhea) return pTableHhea->advanceWidthMax;
            else if(pTableVhea) return pTableVhea->advanceHeightMax;
            else return 0;
        }

        int CTrueTypeFont::GetNumCharacters(FE_CMap pCMap)
        {
            if(pTableCMap && pCMap) return pTableCMap->GetNumCharacters(pCMap);
            else if(pTableGlyf) return pTableGlyf->numGlyphs;
            return 0;
        }

        int CTrueTypeFont::GetAdvanceWidth(int glyphIndex)
        {
            if(pTableHmtx)
            {
                if(glyphIndex < pTableHmtx->numMetrix)
                    return pTableHmtx->pMetrix[glyphIndex].advanceWidth;
                else if(pTableHmtx->numMetrix > 0)
                    return pTableHmtx->pMetrix[pTableHmtx->numMetrix - 1].advanceWidth;
            }
            else if(pTableVhea)
            {
                return pTableVhea->vertTypoAscender - pTableVhea->vertTypoDescender;
            }

            CGlyphDef *pGlyph = pTableGlyf->GetGlyphObj(glyphIndex);
            return pGlyph->xMax;

            //if(pTableHhea == NULL || pTableHmtx == NULL || glyphIndex < 0 || glyphIndex >= pTableHmtx->numMetrix) return GetMaxAdvanceWidth();
            //return pTableHmtx->pMetrix[glyphIndex].advanceWidth;
        }

        int CTrueTypeFont::GetAdvanceWidth(FE_CMap pCMap, unsigned int cid)
        {
            if(!pCMap || !pTableCMap || !pTableGlyf) return GetMaxAdvanceWidth();

            Long glyphIndex = pTableCMap->GetGlyphIndex(pCMap, cid);

            return GetAdvanceWidth(glyphIndex);
        }

        int CTrueTypeFont::GetAdvanceHeight(int glyphIndex)
        {
            if(pTableVmtx)
            {
                if(glyphIndex < pTableVmtx->numMetrix)
                    return pTableVmtx->pMetrix[glyphIndex].advanceHeight;
                else if(pTableVmtx->numMetrix > 0)
                    return pTableVmtx->pMetrix[pTableVmtx->numMetrix - 1].advanceHeight;
            }
            else if(pTableHhea)
            {
                return pTableHhea->ascender - pTableHhea->descender;
            }

            CGlyphDef *pGlyph = pTableGlyf->GetGlyphObj(glyphIndex);
            return pGlyph->yMax;
        }
        int CTrueTypeFont::GetAdvanceHeight(FE_CMap pCMap, unsigned int cid)
        {
            if(!pCMap || !pTableCMap || !pTableGlyf) return GetMaxAdvanceWidth();

            Long glyphIndex = pTableCMap->GetGlyphIndex(pCMap, cid);

            return GetAdvanceHeight(glyphIndex);
        }

        int CTrueTypeFont::GetLineGap(int glyphIndex)
        {
            if(pTableHhea)
            {
                return pTableHhea->lineGap;
            }
            else if(pTableVhea)
            {
                return pTableVhea->vertTypoLineGap;
            }

            return 0;
        }
        int CTrueTypeFont::GetLineGap(FE_CMap pCMap, unsigned int cid)
        {
            if(!pCMap || !pTableCMap || !pTableGlyf) return 0;

            Long glyphIndex = pTableCMap->GetGlyphIndex(pCMap, cid);

            return GetLineGap(glyphIndex);
        }

        int CTrueTypeFont::GetLeftSideBearing(int glyphIndex)
        {
            if(pTableGlyf)
            {
                CGlyphDef *pGlyph = pTableGlyf->GetGlyphObj(glyphIndex);
                if(pGlyph)
                {
                    if(pGlyph->numberOfContours == -1) // compound glyph
                    {
                        CCompoundGlyph *pCompoundGlyph = (CCompoundGlyph*)pGlyph;
                        for(size_t i = 0; i < pCompoundGlyph->components.size(); i++)
                        {
                            if(pCompoundGlyph->components[i]->UseMyMetrics())
                                return GetLeftSideBearing(pCompoundGlyph->components[i]->glyphid);
                        }
                    }
                }
            }

            if(pTableHhea)
            {
                if(pTableHmtx)
                {
                    if(glyphIndex < pTableHmtx->numMetrix)
                        return pTableHmtx->pMetrix[glyphIndex].leftSideBearing;
                    else if(glyphIndex - pTableHmtx->numMetrix < pTableHmtx->numLeftSideBearing)
                        return pTableHmtx->pLeftSideBearing[glyphIndex - pTableHmtx->numMetrix];
                }
                return pTableHhea->minLeftSideBearing;
            }

            return 0;
        }

        int CTrueTypeFont::GetLeftSideBearing(FE_CMap pCMap, unsigned int cid)
        {
            if(!pCMap || !pTableCMap || !pTableGlyf) return 0;

            Long glyphIndex = pTableCMap->GetGlyphIndex(pCMap, cid);

            return GetLeftSideBearing(glyphIndex);
        }

        int CTrueTypeFont::GetRightSideBearing(int glyphIndex)
        {
            CGlyphDef *pGlyph = pTableGlyf->GetGlyphObj(glyphIndex);
            return GetAdvanceWidth(glyphIndex) - GetLeftSideBearing(glyphIndex) - (pGlyph->xMax - pGlyph->xMin);
        }

        int CTrueTypeFont::GetRightSideBearing(FE_CMap pCMap, unsigned int cid)
        {
            if(!pCMap || !pTableCMap || !pTableGlyf) return 0;

            Long glyphIndex = pTableCMap->GetGlyphIndex(pCMap, cid);

            return GetRightSideBearing(glyphIndex);

            if(pTableHhea)
                return pTableHhea->minRightSideBearing;
            return 0;
        }

        BOOL CTrueTypeFont::GetFontBBox(CFeRect &rect)
        {
            if(!pTableHead) return false;

            rect.Set(pTableHead->xMin, pTableHead->yMin);
            rect.Add(pTableHead->xMax, pTableHead->yMax);
            return true;
        }

        BOOL CTrueTypeFont::GetGlyphBBox(FE_CMap pCMap, unsigned int cid, CFeRect &rect)
        {
            Long glyphIndex = GetGlyphIndex(pCMap, cid);

            return GetGlyphBBox(glyphIndex, rect);
        }

        BOOL CTrueTypeFont::GetGlyphBBox(int glyphIndex, CFeRect &rect)
        {
            if(glyphIndex == -1) return false;

            CGlyphDef *pGlyph = pTableGlyf->GetGlyphObj(glyphIndex);
            if(!pGlyph) return false;

            rect.Set(pGlyph->xMin, pGlyph->yMin);
            rect.Add(pGlyph->xMax, pGlyph->yMax);

            return true;
        }

        CFontFace* CTrueTypeFont::FindFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
        {
            for(size_t i = 0; i<m_arrayFaces.size(); i++)
            {
                if(m_arrayFaces[i]->m_DPI == dpi 
                    && m_arrayFaces[i]->m_fontSize == fontSize 
                    && m_arrayFaces[i]->m_flags == flags 
                    && m_arrayFaces[i]->m_nPlatformId == platformId 
                    && m_arrayFaces[i]->m_nEncodingId == encodingId
                    )
                    return m_arrayFaces[i];
            }

            return NULL;
        }

        CFeGlyph* CTrueTypeFont::GetGlyphByIndex(FE_FontFace face, int glyphIndex)
        {
            if(glyphIndex == -1) return NULL;

            CTTFontFace *pFace = (CTTFontFace*)face;

            CFeGlyph *pGlyph = new CFeGlyph();
            GetGlyphBBox(glyphIndex, pGlyph->m_bbox);
            pGlyph->m_bbox.SetLeft(pGlyph->m_bbox.GetLeft() * pFace->m_fScale);
            pGlyph->m_bbox.SetRight(pGlyph->m_bbox.GetRight() * pFace->m_fScale);
            pGlyph->m_bbox.SetTop(pGlyph->m_bbox.GetTop() * pFace->m_fScale);
            pGlyph->m_bbox.SetBottom(pGlyph->m_bbox.GetBottom() * pFace->m_fScale);
            if(pFace->m_pCMap)
            {
                pGlyph->m_lsb = (GetLeftSideBearing(glyphIndex) * pFace->m_fScale);
                pGlyph->m_advanceWidth = (GetAdvanceWidth(glyphIndex) * pFace->m_fScale);
            }
            else
            {
                pGlyph->m_lsb = 0;
                pGlyph->m_advanceWidth = 0;
            }
            pGlyph->m_rsb = (pGlyph->m_advanceWidth - pGlyph->m_lsb - pGlyph->m_bbox.GetWidth());

            Byte *pIntr = NULL;
            //CGlyphOutlines *pOutlines = Scaler(pFace, glyphIndex);
            CGlyphDef *pGlyphDef = pTableGlyf->GetGlyphObj(glyphIndex);
            CGlyphOutlines *pOutlines = BuildGlyphOutline(pFace, glyphIndex, pGlyphDef, CMatrix3x3());

            pGlyph->bmap = NULL;//(FE_Bitmap*)m_pFont->GenerateGlyphBmap(0, pGlyph->m_bbox, pOutlines);
            pGlyph->outlines = (FE_Outlines*)pOutlines;

            pFace->m_arrayGid2Glyphs.insert(std::pair<int, CFeGlyph*>(glyphIndex, pGlyph));

            return pGlyph;
        }

        CFeGlyph* CTrueTypeFont::GetGlyph(FE_FontFace face, unsigned int cid)
        {
            CTTFontFace *pFace = (CTTFontFace*)face;
            std::map<int, CFeGlyph*>::iterator it = pFace->m_arrayGlyphs.find(cid);
            if(it != pFace->m_arrayGlyphs.end())
                return it->second;

            int glyphIndex = pFace->GetGlyphIndex(cid);
            if(glyphIndex == -1) return NULL;

            CFeGlyph *pGlyph = GetGlyphByIndex(face, glyphIndex);
            if (pGlyph) pFace->m_arrayGlyphs.insert(std::pair<int, CFeGlyph*>(cid, pGlyph));
            return pGlyph;
        }

        CFontFace* CTrueTypeFont::CreateFontFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
        {
            if (!pTableHead) return nullptr;

            CFontFace *pFontFace = FindFace(fontSize, dpi, flags, platformId, encodingId);
            if(pFontFace)
            {
                pFontFace->m_nRefCount++;
                return pFontFace;
            }

            CTTFontFace *pFace = new CTTFontFace(this, fontSize, dpi, flags, platformId, encodingId);
            //pFace->m_nPlatformId = (FE_PLATFORM_ID)platformId;
            //pFace->m_nEncodingId = encodingId;
            pFace->m_nRefCount++;
            AppendFace(pFace);

            // run CVT program
            Short ppem = Calculate_ppem(fontSize, dpi);
            double unitsPerPixel = (double)pTableHead->unitsPerEm / ppem;
            if(pCvt)
            {
                pFace->m_pCvt = new Long[nCvt];
                //memcpy(pFace->m_pCvt, pCvt, nCvt * sizeof(Long));
                for(ULong i = 0; i < nCvt; i++)
                    pFace->m_pCvt[i] = (Long)(pCvt[i] / unitsPerPixel * 64);
            }
            //{
            //    FILE *fp = fopen("G:\\Test\\tmp1\\cvt_1.txt", "w");
            //    for(int i = 0; i<nCvt; i++)
            //    {
            //        char buf[64];
            //        sprintf_s(buf, 64, "%d\n", pFace->m_pCvt[i]);
            //        fwrite(buf, 1, strlen(buf), fp);
            //    }
            //    fclose(fp);
            //}
            pFace->m_pStartGState = RunCvtProgram(pStartGState, ppem, fontSize, (float)unitsPerPixel, nPrep, pPrep, nCvt, pFace->m_pCvt);

            //{
            //    FILE *fp = fopen("G:\\Test\\tmp1\\cvt_2.txt", "w");
            //    for(int i = 0; i<nCvt; i++)
            //    {
            //        char buf[64];
            //        sprintf_s(buf, 64, "%d\n", pFace->m_pCvt[i]);
            //        fwrite(buf, 1, strlen(buf), fp);
            //    }
            //    fclose(fp);
            //}

            return pFace;
        }

        ///////////////////////////////////////////////////////////
        // debug functions
        ///////////////////////////////////////////////////////////
        float CTrueTypeFont::RoundToInt(float v)
        {
            if (v > 0) v += 0.5f;
            else if (v < 0) v -= 0.5f;
            return (float)(int)v;
        }
    }

};
