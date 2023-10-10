#include "stdafx.h"
#include "InstrCmd.h"
#include "Instruction.h"
#include <map>
#include "TrueType.h"
#include "math.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        extern CInstrCommand TT_CommandList;

        wchar_t tt_gCmdName[256][16] =
        {
            L"SVTCA0[y]",
            L"SVTCA1[x]",
            L"SPVTCA0",
            L"SPVTCA1",
            L"SFVTCA0",
            L"SFVTCA1",
            L"SPVTL0",
            L"SPVTL1",
            L"SFVTL0",
            L"SFVTL1",
            L"SPVFS",
            L"SFVFS",
            L"GPV",
            L"GFV",
            L"SFVTPV",
            L"ISECT",
            L"SRP0",
            L"SRP1",
            L"SRP2",
            L"SZP0",
            L"SZP1",
            L"SZP2",
            L"SZPS",
            L"SLOOP",
            L"RTG",
            L"RTHG",
            L"SMD",
            L"ELSE",
            L"JMPR",
            L"SCVTCI",
            L"SSWCI",
            L"SSW",
            L"DUP",
            L"POP",
            L"CLEAR",
            L"SWAP",
            L"DEPTH",
            L"CINDEX",
            L"MINDEX",
            L"ALIGNPTS",
            L"NA",
            L"UTP",
            L"LOOPCALL",
            L"CALL",
            L"FDEF",
            L"ENDF",
            L"MDAP0[nr]",
            L"MDAP1[rnd]",
            L"IUP0[y]",
            L"IUP1[x]",
            L"SHP0",
            L"SHP1",
            L"SHC0",
            L"SHC1",
            L"SHZ0",
            L"SHZ1",
            L"SHPIX",
            L"IP",
            L"MSIRP0",
            L"MSIRP1",
            L"ALIGNRP",
            L"RTDG",
            L"MIAP0",
            L"MIAP1",
            L"NPUSHB",
            L"NPUSHW",
            L"WS",
            L"RS",
            L"WCVTP",
            L"RCVT",
            L"GC0",
            L"GC1",
            L"SCFS",
            L"MD0",
            L"MD1",
            L"MPPEM",
            L"MPS",
            L"FLIPON",
            L"FLIPOFF",
            L"DEBUG",
            L"LT",
            L"LTEQ",
            L"GT",
            L"GTEQ",
            L"EQ",
            L"NEQ",
            L"ODD",
            L"EVEN",
            L"IF",
            L"EIF",
            L"AND",
            L"OR",
            L"NOT",
            L"DELTAP1",
            L"SDB",
            L"SDS",
            L"ADD",
            L"SUB",
            L"DIV",
            L"MUL",
            L"ABS",
            L"NEG",
            L"FLOOR",
            L"CEILING",
            L"ROUND0",
            L"ROUND1",
            L"ROUND2",
            L"ROUND3",
            L"NROUND0",
            L"NROUND1",
            L"NROUND2",
            L"NROUND3",
            L"WCVTF",
            L"DELTAP2",
            L"DELTAP3",
            L"DELTAC1",
            L"DELTAC2",
            L"DELTAC3",
            L"SROUND",
            L"S45ROUND",
            L"JROT",
            L"JROF",
            L"ROFF",
            L"NA",
            L"RUTG",
            L"RDTG",
            L"SANGW",
            L"AA",
            L"FLIPPT",
            L"FLIPRGON",
            L"NA",
            L"NA",
            L"FLIPRGOFF",
            L"SCANCTRL",
            L"SDPVTL0",
            L"SDPVTL1",
            L"GETINFO",
            L"IDEF",
            L"ROLL",
            L"MAX",
            L"MIN",
            L"SCANTYPE",
            L"INSTCTRL",        //0x8E
            L"NA", //OpProcs[0x8F] = tt_Wrong;
            L"NA", //OpProcs[0x90] = tt_Wrong;
            L"NA", //OpProcs[0x91] = tt_Wrong;
            L"NA", //OpProcs[0x92] = tt_Wrong;
            L"NA", //OpProcs[0x93] = tt_Wrong;
            L"NA", //OpProcs[0x94] = tt_Wrong;
            L"NA", //OpProcs[0x95] = tt_Wrong;
            L"NA", //OpProcs[0x96] = tt_Wrong;
            L"NA", //OpProcs[0x97] = tt_Wrong;
            L"NA", //OpProcs[0x98] = tt_Wrong;
            L"NA", //OpProcs[0x99] = tt_Wrong;
            L"NA", //OpProcs[0x9A] = tt_Wrong;
            L"NA", //OpProcs[0x9B] = tt_Wrong;
            L"NA", //OpProcs[0x9C] = tt_Wrong;
            L"NA", //OpProcs[0x9D] = tt_Wrong;
            L"NA", //OpProcs[0x9E] = tt_Wrong;
            L"NA", //OpProcs[0x9F] = tt_Wrong;
            L"NA", //OpProcs[0xA0] = tt_Wrong;
            L"NA", //OpProcs[0xA1] = tt_Wrong;
            L"NA", //OpProcs[0xA2] = tt_Wrong;
            L"NA", //OpProcs[0xA3] = tt_Wrong;
            L"NA", //OpProcs[0xA4] = tt_Wrong;
            L"NA", //OpProcs[0xA5] = tt_Wrong;
            L"NA", //OpProcs[0xA6] = tt_Wrong;
            L"NA", //OpProcs[0xA7] = tt_Wrong;
            L"NA", //OpProcs[0xA8] = tt_Wrong;
            L"NA", //OpProcs[0xA9] = tt_Wrong;
            L"NA", //OpProcs[0xAA] = tt_Wrong;
            L"NA", //OpProcs[0xAB] = tt_Wrong;
            L"NA", //OpProcs[0xAC] = tt_Wrong;
            L"NA", //OpProcs[0xAD] = tt_Wrong;
            L"NA", //OpProcs[0xAE] = tt_Wrong;
            L"NA", //OpProcs[0xAF] = tt_Wrong;
            L"PUSHB1", //0xB0
            L"PUSHB2",
            L"PUSHB3",
            L"PUSHB4",
            L"PUSHB5",
            L"PUSHB6",
            L"PUSHB7",
            L"PUSHB8",
            L"PUSHW1",
            L"PUSHW2",
            L"PUSHW3",
            L"PUSHW4",
            L"PUSHW5",
            L"PUSHW6",
            L"PUSHW7",
            L"PUSHW8",
            L"MDRP00",
            L"MDRP01",
            L"MDRP02",
            L"MDRP03",
            L"MDRP04",
            L"MDRP05",
            L"MDRP06",
            L"MDRP07",
            L"MDRP08",
            L"MDRP09",
            L"MDRP0A",
            L"MDRP0B",
            L"MDRP0C",
            L"MDRP0D",
            L"MDRP0E",
            L"MDRP0F",
            L"MDRP10",
            L"MDRP11",
            L"MDRP12",
            L"MDRP13",
            L"MDRP14",
            L"MDRP15",
            L"MDRP16",
            L"MDRP17",
            L"MDRP18",
            L"MDRP19",
            L"MDRP1A",
            L"MDRP1B",
            L"MDRP1C",
            L"MDRP1D",
            L"MDRP1E",
            L"MDRP1F",
            L"MIRP0[gry]",
            L"MIRP1[blk]",
            L"MIRP2[wht]",
            L"MIRP3[none]",
            L"MIRP4",
            L"MIRP5",
            L"MIRP6",
            L"MIRP7",
            L"MIRP8",
            L"MIRP9",
            L"MIRPA",
            L"MIRPB",
            L"MIRPC",
            L"MIRPD",
            L"MIRPE",
            L"MIRPF",
            L"MIRP10",
            L"MIRP11",
            L"MIRP12",
            L"MIRP13",
            L"MIRP14",
            L"MIRP15",
            L"MIRP16",
            L"MIRP17",
            L"MIRP18",
            L"MIRP19",
            L"MIRP1A",
            L"MIRP1B",
            L"MIRP1C",
            L"MIRP1D",
            L"MIRP1E",
            L"MIRP1F",
        };
        void CreateInstrCmdList(ULong nInstr, Byte *pInstr, std::vector<std::wstring> &res, std::map<int, int> &instrMap)
        {
            res.clear();
            instrMap.clear();
            for (ULong pos = 0; pos < nInstr; pos++)
            {
                res.push_back(tt_gCmdName[pInstr[pos]]);
                instrMap.insert(std::pair<int, int>(pos, (int)res.size()-1));
                Byte n = 0;
                switch (pInstr[pos])
                {
                case TT_NPUSHB:
                    n = pInstr[pos + 1] + 1;
                    res.push_back(L"  " + std::to_wstring(n - 1));
                    for (Byte i = 0; i < n; i++)
                    {
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 2 + i]));
                    }
                    break;
                case TT_NPUSHW:
                    n = pInstr[pos + 1] * 2 + 1;
                    res.push_back(L"  " + std::to_wstring((n - 1) / 2));
                    for (Byte i = 0; i < n/2; i++)
                    {
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 2 + i * 2] << 8) + pInstr[pos + 3 + i * 2])));
                    }
                    break;
                case TT_PUSHB1:
                    res.push_back(L"  " + std::to_wstring(pInstr[pos + 1]));
                    n++;
                    break;
                case TT_PUSHB2:
                    for (int i = 0; i < 2; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 2;
                    break;
                case TT_PUSHB3:
                    for (int i = 0; i < 3; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 3;
                    break;
                case TT_PUSHB4:
                    for (int i = 0; i < 4; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 4;
                    break;
                case TT_PUSHB5:
                    for (int i = 0; i < 5; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 5;
                    break;
                case TT_PUSHB6:
                    for (int i = 0; i < 6; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 6;
                    break;
                case TT_PUSHB7:
                    for (int i = 0; i < 7; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 7;
                    break;
                case TT_PUSHB8:
                    for (int i = 0; i < 8; i++)
                        res.push_back(L"  " + std::to_wstring(pInstr[pos + 1 + i]));
                    n += 8;
                    break;
                case TT_PUSHW1:
                    res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1] << 8) + pInstr[pos + 2])));
                    n += 2;
                    break;
                case TT_PUSHW2:
                    for (int i = 0; i < 2; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 4;
                    break;
                case TT_PUSHW3:
                    for (int i = 0; i < 3; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 6;
                    break;
                case TT_PUSHW4:
                    for (int i = 0; i < 4; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 8;
                    break;
                case TT_PUSHW5:
                    for (int i = 0; i < 5; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 10;
                    break;
                case TT_PUSHW6:
                    for (int i = 0; i < 6; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 12;
                    break;
                case TT_PUSHW7:
                    for (int i = 0; i < 7; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 14;
                    break;
                case TT_PUSHW8:
                    for (int i = 0; i < 8; i++)
                        res.push_back(L"  " + std::to_wstring((Short)((pInstr[pos + 1 + i * 2] << 8) + pInstr[pos + 2 + i * 2])));
                    n += 16;
                    break;
                }
                pos += n;
            }
        }

        FE_DebugInfo CTrueTypeFont::GlyphDebugStart(FE_FontFace face, int cid, int set)
        {
            if (set == 0)
            {
                Byte *pInstr = nullptr;
                UShort nInstr = 0;

                int glyphIndex = cid;
                CTTFontFace *pFace = (CTTFontFace*)face;
                if (pFace->m_pCMap)
                    glyphIndex = GetGlyphIndex(pFace->m_pCMap, cid);
                CFeGlyph* pGlyph = GetGlyph(face, cid);
                if (pGlyph == nullptr) pGlyph = GetGlyphByIndex(face, glyphIndex);
                if (pGlyph)
                {
                    CGlyphDef *pGlyphDef = pTableGlyf->GetGlyphObj(glyphIndex);

                    //CGlyphOutlines *pOutlines = BuildGlyphOrigOutline(pFace, cid, pGlyphDef);

                    CGlyphOutlines *pCompoundOutlines = nullptr;
                    CGlyphOutlines *pOutlines = nullptr;
                    CGlyphOutlines *pOrigOutlines = nullptr;

                    if (pGlyphDef->numberOfContours != -1) // simple glyph
                    {
                        pOutlines = PrepareSimpleGlyphOutlines(pFace, glyphIndex, pGlyphDef, &pOrigOutlines);
                        nInstr = pGlyphDef->GetInstructions(&pInstr);
                    }
                    else
                    {
                        CCompoundGlyph *pCompoundGlyph = (CCompoundGlyph*)pGlyphDef;
                        int size = (int)pCompoundGlyph->components.size();

                        if (size > 0)
                        {
                            pCompoundOutlines = new CGlyphOutlines();
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

                            // The first component
                            CCompoundGlyph::CComponent *pComponent = pCompoundGlyph->components[0];
                            CGlyphDef *pSubGlyphDef = pTableGlyf->GetGlyphObj(pComponent->glyphid);

                            // translate component glyph from offsets or anchor points to compound glyph coordinate system
                            CMatrix3x3 mat;
                            GetSubglyphMatrix(pComponent, pCompoundOutlines, pOutlines, mat);

                            // prepare the outlines of first sub-glyph
                            pOutlines = PrepareSimpleGlyphOutlines(pFace, pComponent->glyphid, pSubGlyphDef, &pOrigOutlines);

                            // load instructions
                            nInstr = pSubGlyphDef->GetInstructions(&pInstr);
                        }
                    }

                    if (pOutlines == nullptr)
                    {
                        if (pCompoundOutlines) delete pCompoundOutlines;
                        return nullptr;
                    }

                    ////////////////////////////////////////////////////////////////////////
                    // prepare instructions
                    CGState *pGState = (CGState*)pFace->m_pStartGState;
                    if (pGState->instrControl & TTIC_IGNORE_CVT_CHANGES)
                        pGState->Reset();

                    if ((pGState->instrControl & TTIC_NOT_RUN_GLYPH_INSTRUCTS) == 0)
                    {
                        CGState *m_pGState = new CGState();
                        m_pGState->Copy((CGState*)pGState);
                        m_pGState->m_nTwilightPoints = pTableMaxp->maxTwilightPoints + 4;
                        if (m_pGState->m_nTwilightPoints > 0)
                            m_pGState->m_pTwilightZone = new CGlyphCoordinate[m_pGState->m_nTwilightPoints];

                        m_pGState->nInstr = nInstr;
                        m_pGState->pInstr = pInstr;
                        m_pGState->nOffset = 0;
                        m_pGState->nCvt = nCvt;
                        m_pGState->pCvt = pFace->m_pCvt;
                        m_pGState->pOrigOutlines = pOrigOutlines;
                        m_pGState->pOutlines = pOutlines;

                        CGlyphDebugInfo *pDebugInfo = new CGlyphDebugInfo();
                        pDebugInfo->cid = cid;
                        pDebugInfo->pData = m_pGState;
                        pDebugInfo->pOutlines = pOutlines;
                        pDebugInfo->pOrigOutlines = pOrigOutlines;
                        pDebugInfo->pTwilights = m_pGState->m_pTwilightZone;
                        pDebugInfo->nTwilights = m_pGState->m_nTwilightPoints;
                        pDebugInfo->currentPos = 0;
                        pDebugInfo->intructionSet = set;
                        pDebugInfo->pData1 = pCompoundOutlines;
                        if (pCompoundOutlines)
                        {
                            CCompoundGlyph *pCompoundGlyph = (CCompoundGlyph*)pGlyphDef;
                            for (size_t i = 0; i < pCompoundGlyph->components.size(); i++)
                            {
                                pDebugInfo->subglyphs.push_back(pCompoundGlyph->components[i]->glyphid);
                                pDebugInfo->curSubGlyph = 0;
                            }
                        }
                        SetDebugState(m_pGState, pDebugInfo);
                        SetDebugStack(m_pGState, pDebugInfo);
                        SetDebugStorage(m_pGState, pDebugInfo);
                        SetDebugCvt(m_pGState, pDebugInfo);
                        CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                        return pDebugInfo;
                    }
                }

                return nullptr;
            }
            else if (set == 1) // CVT Program
            {
                CTTFontFace *pFace = (CTTFontFace*)face;

                if (pFace->m_pStartGState) delete ((CGState*)pFace->m_pStartGState);
                pFace->m_pStartGState = nullptr;

                // run CVT program
                Short ppem = Calculate_ppem(pFace->m_fontSize, pFace->m_DPI);
                double unitsPerPixel = (double)pTableHead->unitsPerEm / ppem;
                if (pCvt)
                {
                    if (pFace->m_pCvt == nullptr)
                        pFace->m_pCvt = new Long[nCvt];
                    for (ULong i = 0; i < nCvt; i++)
                        pFace->m_pCvt[i] = (Long)(pCvt[i] / unitsPerPixel * 64);
                }

                CGState *m_pGState = new CGState();
                m_pGState->Copy((CGState*)pStartGState);
                m_pGState->ppem = ppem;
                m_pGState->fontSize = pFace->m_fontSize;
                m_pGState->unitsPerPixel = (float)unitsPerPixel;

                pFace->m_pStartGState = m_pGState;

                m_pGState->nInstr = nPrep;
                m_pGState->pInstr = pPrep;
                m_pGState->nOffset = 0;
                m_pGState->nCvt = nCvt;
                m_pGState->pCvt = pFace->m_pCvt;
                m_pGState->pOrigOutlines = nullptr;
                m_pGState->pOutlines = nullptr;

                CGlyphDebugInfo *pDebugInfo = new CGlyphDebugInfo();
                pDebugInfo->pData = m_pGState;
                pDebugInfo->pOutlines = nullptr;
                pDebugInfo->pOrigOutlines = nullptr;
                pDebugInfo->currentPos = 0;
                pDebugInfo->intructionSet = set;
                SetDebugState(m_pGState, pDebugInfo);
                SetDebugStack(m_pGState, pDebugInfo);
                SetDebugStorage(m_pGState, pDebugInfo);
                SetDebugCvt(m_pGState, pDebugInfo);
                CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                return pDebugInfo;
            }
            else // Font program
            {
                if (pStartGState) delete ((CGState*)pStartGState);

                std::map<ULong, TT_NestedFunc*>::iterator it;
                for (it = functionDefs.begin(); it != functionDefs.end(); it++)
                    delete it->second;
                functionDefs.clear();

                CGState *m_pGState = new CGState();
                m_pGState->m_nStorage = pTableMaxp->maxStorage;
                if (m_pGState->m_nStorage > 0)
                {
                    m_pGState->m_pStorage = new ULong[m_pGState->m_nStorage];
                    for (ULong i = 0; i < m_pGState->m_nStorage; i++)
                        m_pGState->m_pStorage[i] = 87654321;
                }
                m_pGState->m_pFuncDefs = &functionDefs;

                pStartGState = m_pGState;

                m_pGState->nInstr = nFpgm;
                m_pGState->pInstr = pFpgm;
                m_pGState->nOffset = 0;
                m_pGState->nCvt = nCvt;
                m_pGState->pCvt = pCvt;
                m_pGState->pOrigOutlines = nullptr;
                m_pGState->pOutlines = nullptr;

                CGlyphDebugInfo *pDebugInfo = new CGlyphDebugInfo();
                pDebugInfo->pData = m_pGState;
                pDebugInfo->pOutlines = nullptr;
                pDebugInfo->pOrigOutlines = nullptr;
                pDebugInfo->currentPos = 0;
                pDebugInfo->intructionSet = set;
                SetDebugState(m_pGState, pDebugInfo);
                SetDebugStack(m_pGState, pDebugInfo);
                SetDebugStorage(m_pGState, pDebugInfo);
                SetDebugCvt(m_pGState, pDebugInfo);
                CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                return pDebugInfo;
            }

            return nullptr;
        }
        void CTrueTypeFont::SetDebugState(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo)
        {
            if (m_pGState && pDebugInfo)
            {
                if (pDebugInfo->stateVariables.size() == 0)
                {
                    for (int i = 0; i < 25; i++)
                        pDebugInfo->stateVariables.push_back(new CGlyphDebugInfo::CNameValue());
                }

                pDebugInfo->stateVariables[0]->name = L"rp0";
                pDebugInfo->stateVariables[0]->value = std::to_wstring(m_pGState->rp0).c_str();
                pDebugInfo->stateVariables[1]->name = L"rp1";
                pDebugInfo->stateVariables[1]->value = std::to_wstring(m_pGState->rp1).c_str();
                pDebugInfo->stateVariables[2]->name = L"rp2";
                pDebugInfo->stateVariables[2]->value = std::to_wstring(m_pGState->rp2).c_str();
                pDebugInfo->stateVariables[3]->name = L"loop";
                pDebugInfo->stateVariables[3]->value = std::to_wstring(m_pGState->loop).c_str();
                pDebugInfo->stateVariables[4]->name = L"zp0";
                pDebugInfo->stateVariables[4]->value = (m_pGState->zp0 == TTZP_TWILIGHT) ? L"twilight" : L"Normal";
                pDebugInfo->stateVariables[5]->name = L"zp1";
                pDebugInfo->stateVariables[5]->value = (m_pGState->zp1 == TTZP_TWILIGHT) ? L"twilight" : L"Normal";
                pDebugInfo->stateVariables[6]->name = L"zp2";
                pDebugInfo->stateVariables[6]->value = (m_pGState->zp2 == TTZP_TWILIGHT) ? L"twilight" : L"Normal";

                wchar_t buf[256];
                swprintf_s(buf, 256, L"%.2f", m_pGState->minDist);
                pDebugInfo->stateVariables[7]->name = L"minDist";
                pDebugInfo->stateVariables[7]->value = buf;
                swprintf_s(buf, 256, L"%.2f", m_pGState->cvtCutin);
                pDebugInfo->stateVariables[8]->name = L"cvtCutin";
                pDebugInfo->stateVariables[8]->value = buf;
                swprintf_s(buf, 256, L"%.2f", m_pGState->singWidCut);
                pDebugInfo->stateVariables[9]->name = L"singWidCut";
                pDebugInfo->stateVariables[9]->value = buf;
                swprintf_s(buf, 256, L"%.2f", m_pGState->singWidVal);
                pDebugInfo->stateVariables[10]->name = L"singWidVal";
                pDebugInfo->stateVariables[10]->value = buf;
                swprintf_s(buf, 256, L"%.2f, %.2f", m_pGState->freeVec.x, m_pGState->freeVec.y);
                pDebugInfo->stateVariables[11]->name = L"freeVec";
                pDebugInfo->stateVariables[11]->value = buf;
                swprintf_s(buf, 256, L"%.2f, %.2f", m_pGState->projVec.x, m_pGState->projVec.y);
                pDebugInfo->stateVariables[12]->name = L"projVec";
                pDebugInfo->stateVariables[12]->value = buf;
                swprintf_s(buf, 256, L"%.2f, %.2f", m_pGState->dualVec.x, m_pGState->dualVec.y);
                pDebugInfo->stateVariables[13]->name = L"dualVec";
                pDebugInfo->stateVariables[13]->value = buf;
                pDebugInfo->stateVariables[14]->name = L"autoFlip";
                pDebugInfo->stateVariables[14]->value = (m_pGState->autoFlip) ? L"True" : L"False";
                swprintf_s(buf, 256, L"%d", m_pGState->deltaBase);
                pDebugInfo->stateVariables[15]->name = L"deltaBase";
                pDebugInfo->stateVariables[15]->value = buf;
                swprintf_s(buf, 256, L"%d", m_pGState->deltaShift);
                pDebugInfo->stateVariables[16]->name = L"deltaShift";
                pDebugInfo->stateVariables[16]->value = buf;
                pDebugInfo->stateVariables[17]->name = L"rndState";
                switch (m_pGState->rndState)
                {
                case TTRS_OFF:
                    pDebugInfo->stateVariables[17]->value = L"Off";
                    break;
                case TTRS_TO_GRID:
                    pDebugInfo->stateVariables[17]->value = L"To Grid";
                    break;
                case TTRS_TO_HALF_GRID:
                    pDebugInfo->stateVariables[17]->value = L"To Halfgrid";
                    break;
                case TTRS_TO_DOUBLE_GRID:
                    pDebugInfo->stateVariables[17]->value = L"To DblGrid";
                    break;
                case TTRS_DOWN_GRID:
                    pDebugInfo->stateVariables[17]->value = L"DownGrid";
                    break;
                case TTRS_UP_GRID:
                    pDebugInfo->stateVariables[17]->value = L"UpGrid";
                    break;
                case TTRS_SUPER_45:
                    pDebugInfo->stateVariables[17]->value = L"Sup45";
                    break;
                case TTRS_SUPER:
                    pDebugInfo->stateVariables[17]->value = L"Supper";
                    break;
                }
                swprintf_s(buf, 256, L"%.2f", m_pGState->sRndPeriod);
                pDebugInfo->stateVariables[18]->name = L"sRndPeriod";
                pDebugInfo->stateVariables[18]->value = buf;
                swprintf_s(buf, 256, L"%.2f", m_pGState->sRndPhase);
                pDebugInfo->stateVariables[19]->name = L"sRndPhase";
                pDebugInfo->stateVariables[19]->value = buf;
                swprintf_s(buf, 256, L"%.2f", m_pGState->sRndThreshold);
                pDebugInfo->stateVariables[20]->name = L"sRndThreshold";
                pDebugInfo->stateVariables[20]->value = buf;
                swprintf_s(buf, 256, L"%d", m_pGState->instrControl);
                pDebugInfo->stateVariables[21]->name = L"instrControl";
                pDebugInfo->stateVariables[21]->value = buf;
                pDebugInfo->stateVariables[22]->name = L"scanControl";
                pDebugInfo->stateVariables[22]->value = (m_pGState->scanControl) ? L"True" : L"False";
                swprintf_s(buf, 256, L"%d", m_pGState->scanType);
                pDebugInfo->stateVariables[23]->name = L"scanType";
                pDebugInfo->stateVariables[23]->value = buf;
                swprintf_s(buf, 256, L"%d", m_pGState->ppem);
                pDebugInfo->stateVariables[24]->name = L"ppem";
                pDebugInfo->stateVariables[24]->value = buf;
            }

        }
        void CTrueTypeFont::SetDebugStack(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo)
        {
            if (m_pGState && pDebugInfo)
            {
                pDebugInfo->stack.clear();
                for (size_t i = 0; i < m_pGState->m_stack.size(); i++)
                {
                    pDebugInfo->stack.push_back(m_pGState->m_stack.get((int)i));
                }
            }
        }
        void CTrueTypeFont::SetDebugStorage(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo)
        {
            if (m_pGState && pDebugInfo && m_pGState->m_pStorage)
            {
                pDebugInfo->storage.clear();
                for (ULong i = 0; i < m_pGState->m_nStorage; i++)
                {
                    pDebugInfo->storage.push_back(m_pGState->m_pStorage[i]);
                }
            }
        }
        void CTrueTypeFont::SetDebugCvt(CGState *m_pGState, CGlyphDebugInfo *pDebugInfo)
        {
            if (m_pGState && pDebugInfo)
            {
                pDebugInfo->nCvt = m_pGState->nCvt;
                pDebugInfo->pCvt = m_pGState->pCvt;
            }
        }

        BOOL CTrueTypeFont::GlyphDebugContinue(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            if (pDebugInfo->bEnd)
            {
                if (pDebugInfo->pData1 == nullptr) return true;

                int glyphIndex = pDebugInfo->cid;
                CTTFontFace *pFace = (CTTFontFace*)face;
                if (pFace->m_pCMap)
                    glyphIndex = GetGlyphIndex(pFace->m_pCMap, pDebugInfo->cid);
                CFeGlyph* pGlyph = GetGlyph(face, pDebugInfo->cid);
                if (pGlyph == nullptr) pGlyph = GetGlyphByIndex(face, glyphIndex);
                if (pGlyph)
                {
                    CCompoundGlyph *pCompoundGlyph = (CCompoundGlyph*)pTableGlyf->GetGlyphObj(glyphIndex);

                    CGlyphOutlines *pCompoundOutlines = (CGlyphOutlines*)pDebugInfo->pData1;
                    CGlyphOutlines *pOutlines = nullptr;
                    CGlyphOutlines *pOrigOutlines = nullptr;

                    Byte *pInstr = nullptr;
                    UShort nInstr = 0;

                    pDebugInfo->curSubGlyph++;
                    if (pDebugInfo->curSubGlyph > 0 && pDebugInfo->curSubGlyph < (int)pCompoundGlyph->components.size())
                    {
                        // The first component
                        CCompoundGlyph::CComponent *pComponent = pCompoundGlyph->components[pDebugInfo->curSubGlyph];
                        CGlyphDef *pSubGlyphDef = pTableGlyf->GetGlyphObj(pComponent->glyphid);

                        // translate component glyph from offsets or anchor points to compound glyph coordinate system
                        CMatrix3x3 mat;
                        GetSubglyphMatrix(pComponent, pCompoundOutlines, pOutlines, mat);

                        // TODO: it is possible the sub-glyph is another compound glyph
                        // prepare the outlines of first sub-glyph
                        pOutlines = PrepareSimpleGlyphOutlines(pFace, pComponent->glyphid, pSubGlyphDef, &pOrigOutlines);

                        // load instructions
                        nInstr = pSubGlyphDef->GetInstructions(&pInstr);
                    }

                    if (pOutlines == nullptr)
                        return false;

                    ////////////////////////////////////////////////////////////////////////
                    // prepare instructions
                    CGState *pGState = (CGState*)pFace->m_pStartGState;

                    if ((pGState->instrControl & TTIC_NOT_RUN_GLYPH_INSTRUCTS) == 0)
                    {
                        if (m_pGState->pOrigOutlines) delete m_pGState->pOrigOutlines;
                        if (m_pGState->pOutlines) delete m_pGState->pOutlines;

                        m_pGState->ResetGState();
                        m_pGState->Copy((CGState*)pGState);
                        m_pGState->m_nTwilightPoints = pTableMaxp->maxTwilightPoints + 4;
                        if (m_pGState->m_nTwilightPoints > 0)
                            m_pGState->m_pTwilightZone = new CGlyphCoordinate[m_pGState->m_nTwilightPoints];

                        m_pGState->nInstr = nInstr;
                        m_pGState->pInstr = pInstr;
                        m_pGState->nOffset = 0;
                        m_pGState->nCvt = nCvt;
                        m_pGState->pCvt = pFace->m_pCvt;
                        m_pGState->pOrigOutlines = pOrigOutlines;
                        m_pGState->pOutlines = pOutlines;

                        pDebugInfo->pData = m_pGState;
                        pDebugInfo->pOutlines = pOutlines;
                        pDebugInfo->pOrigOutlines = pOrigOutlines;
                        pDebugInfo->pTwilights = m_pGState->m_pTwilightZone;
                        pDebugInfo->nTwilights = m_pGState->m_nTwilightPoints;
                        pDebugInfo->currentPos = 0;
                        pDebugInfo->bEnd = false;
                        SetDebugState(m_pGState, pDebugInfo);
                        SetDebugStack(m_pGState, pDebugInfo);
                        SetDebugStorage(m_pGState, pDebugInfo);
                        SetDebugCvt(m_pGState, pDebugInfo);
                        CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                        return true;
                    }
                }
                return true;
            }

            while (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                    break;

                // check if runing to one breakpoint
                for (size_t i = 0; i < m_pGState->breakpoints.size(); i++)
                {
                    if (m_pGState->pInstr == m_pGState->breakpoints[i]->pInstr && m_pGState->nOffset == m_pGState->breakpoints[i]->pos)
                    {
                        SetDebugState(m_pGState, pDebugInfo);
                        SetDebugStack(m_pGState, pDebugInfo);
                        SetDebugStorage(m_pGState, pDebugInfo);
                        SetDebugCvt(m_pGState, pDebugInfo);

                        if (m_pGState->curOpCode == TT_INSTR_CMD::TT_CALL || m_pGState->curOpCode == TT_INSTR_CMD::TT_ENDF)
                        {
                            CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                            pDebugInfo->currentPos = 0;
                            pDebugInfo->bRefillInstructions = true;
                        }
                        else
                            pDebugInfo->bRefillInstructions = false;

                        std::map<int, int>::iterator it = pDebugInfo->instrMap.find(m_pGState->nOffset);
                        if (it != pDebugInfo->instrMap.end())
                            pDebugInfo->currentPos = it->second;

                        if (m_pGState->nestedFuncs.size() == 0 && m_pGState->nInstr == m_pGState->nOffset)
                            pDebugInfo->bEnd = true;
                        else
                            pDebugInfo->bEnd = false;

                        return TRUE;
                    }
                }
            }

            // run to end
            SetDebugState(m_pGState, pDebugInfo);
            SetDebugStack(m_pGState, pDebugInfo);
            SetDebugStorage(m_pGState, pDebugInfo);
            SetDebugCvt(m_pGState, pDebugInfo);

            pDebugInfo->currentPos = m_pGState->nInstr;
            if (pDebugInfo->instrMap.size() > 0)
            {
                std::map<int, int>::iterator it = pDebugInfo->instrMap.end();
                it--;
                pDebugInfo->currentPos = it->second;
            }

            pDebugInfo->bRefillInstructions = false;
            pDebugInfo->bEnd = true;

            return TRUE;
        }
        BOOL CTrueTypeFont::GlyphDebugStepOver(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            if (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                    return FALSE;

                if ( m_pGState->curOpCode == TT_INSTR_CMD::TT_ENDF)
                {
                    CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                    pDebugInfo->currentPos = 0;
                    pDebugInfo->bRefillInstructions = true;
                }
                else if (m_pGState->curOpCode == TT_INSTR_CMD::TT_CALL)
                {
                    int oldPos = (int)m_pGState->nestedFuncs.size() - 1;
                    while (m_pGState->nOffset < (Long)m_pGState->nInstr)
                    {
                        m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                        if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                            return FALSE;
                        if (m_pGState->curOpCode == TT_INSTR_CMD::TT_ENDF && oldPos == (int)m_pGState->nestedFuncs.size())
                        {
                            CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                            pDebugInfo->currentPos = 0;
                            pDebugInfo->bRefillInstructions = true;
                            break;
                        }
                    }
                }
                else
                    pDebugInfo->bRefillInstructions = false;

                SetDebugState(m_pGState, pDebugInfo);
                SetDebugStack(m_pGState, pDebugInfo);
                SetDebugStorage(m_pGState, pDebugInfo);
                SetDebugCvt(m_pGState, pDebugInfo);

                std::map<int, int>::iterator it = pDebugInfo->instrMap.find(m_pGState->nOffset);
                if (it != pDebugInfo->instrMap.end())
                    pDebugInfo->currentPos = it->second;

                pDebugInfo->bRefillInstructions = false;
                if (m_pGState->nestedFuncs.size() == 0 && m_pGState->nInstr == m_pGState->nOffset)
                    pDebugInfo->bEnd = true;
                else
                    pDebugInfo->bEnd = false;
            }

            return TRUE;
        }
        BOOL CTrueTypeFont::GlyphDebugStepIn(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            if (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                    return FALSE;
 
                if (m_pGState->curOpCode == TT_INSTR_CMD::TT_CALL || m_pGState->curOpCode == TT_INSTR_CMD::TT_ENDF)
                {
                    CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                    pDebugInfo->currentPos = 0;
                    pDebugInfo->bRefillInstructions = true;
                }
                else
                    pDebugInfo->bRefillInstructions = false;

                SetDebugState(m_pGState, pDebugInfo);
                SetDebugStack(m_pGState, pDebugInfo);
                SetDebugStorage(m_pGState, pDebugInfo);
                SetDebugCvt(m_pGState, pDebugInfo);

                std::map<int, int>::iterator it = pDebugInfo->instrMap.find(m_pGState->nOffset);
                if (it != pDebugInfo->instrMap.end())
                    pDebugInfo->currentPos = it->second;

                if (m_pGState->nestedFuncs.size() == 0 && m_pGState->nInstr == m_pGState->nOffset)
                    pDebugInfo->bEnd = true;
                else
                    pDebugInfo->bEnd = false;
            }

            return TRUE;
        }
        BOOL CTrueTypeFont::GlyphDebugStepOut(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            int oldPos = (int)m_pGState->nestedFuncs.size() - 1;
            while (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                    return FALSE;

                if (m_pGState->curOpCode == TT_INSTR_CMD::TT_ENDF && oldPos == (int)m_pGState->nestedFuncs.size())
                {
                    CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                    pDebugInfo->currentPos = m_pGState->nOffset;
                    pDebugInfo->bRefillInstructions = true;
                    break;
                }
            }

            SetDebugState(m_pGState, pDebugInfo);
            SetDebugStack(m_pGState, pDebugInfo);
            SetDebugStorage(m_pGState, pDebugInfo);
            SetDebugCvt(m_pGState, pDebugInfo);

            std::map<int, int>::iterator it = pDebugInfo->instrMap.find(m_pGState->nOffset);
            if (it != pDebugInfo->instrMap.end())
                pDebugInfo->currentPos = it->second;

            if (m_pGState->nestedFuncs.size() == 0 && m_pGState->nInstr == m_pGState->nOffset)
                pDebugInfo->bEnd = true;
            else
                pDebugInfo->bEnd = false;

            return TRUE;
        }
        BOOL CTrueTypeFont::GlyphDebugEnd(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            if (debugInfo)
            {
                CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
                if (pDebugInfo->intructionSet == 0 && pDebugInfo->pData)
                {
                    CGState *pGState = (CGState*)pDebugInfo->pData;
                    if (pGState->pOrigOutlines) delete pGState->pOrigOutlines;
                    delete pGState;
                }
                if (pDebugInfo->pData1)
                    delete (CGlyphOutlines*)pDebugInfo->pData1;

                delete pDebugInfo;
            }
            return TRUE;
        }
        
        void* CTrueTypeFont::GlyphDebugCopyBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            if (debugInfo == 0) return FALSE;
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;
            
            std::vector<CDebugBreakPoint*> *pBreakpoints = new std::vector<CDebugBreakPoint*>;
            for (size_t i = 0; i < m_pGState->breakpoints.size(); i++)
            {
                CDebugBreakPoint *pBp = new CDebugBreakPoint(m_pGState->breakpoints[i]->pos, m_pGState->breakpoints[i]->pInstr);
                pBreakpoints->push_back(pBp);
            }
            return pBreakpoints;
        }
        BOOL CTrueTypeFont::GlyphDebugSetBreakPoints(FE_FontFace face, FE_DebugInfo debugInfo, void* breakpoints)
        {
            if (debugInfo == 0 || breakpoints == 0) return FALSE;
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            std::vector<CDebugBreakPoint*> *pBreakpoints = (std::vector<CDebugBreakPoint*>*)breakpoints;
            for (size_t i = 0; i < pBreakpoints->size(); i++)
            {
                m_pGState->breakpoints.push_back((*pBreakpoints)[i]);
            }
            pBreakpoints->clear();
            delete pBreakpoints;

            return TRUE;
        }
        BOOL CTrueTypeFont::GlyphDebugIsBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
        {
            if (debugInfo == 0) return FALSE;

            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            for (std::map<int, int>::iterator it = pDebugInfo->instrMap.begin(); it != pDebugInfo->instrMap.end(); it++)
            {
                if (it->second == pos)
                {
                    for (size_t i = 0; i < m_pGState->breakpoints.size(); i++)
                    {
                        if (m_pGState->breakpoints[i]->pos == it->first && m_pGState->breakpoints[i]->pInstr == m_pGState->pInstr)
                        {
                            return TRUE;
                        }
                    }
                    break;
                }
            }

            return FALSE;
        }
        BOOL CTrueTypeFont::GlyphDebugSetBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
        {
            if (debugInfo == 0) return FALSE;

            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            std::map<int, int>::iterator it;
            for (it = pDebugInfo->instrMap.begin(); it != pDebugInfo->instrMap.end(); it++)
            {
                if (it->second == pos)
                {
                    CDebugBreakPoint *pBkpt = new CDebugBreakPoint();
                    pBkpt->pInstr = m_pGState->pInstr;
                    pBkpt->pos = it->first;
                    m_pGState->breakpoints.push_back(pBkpt);
                    return TRUE;
                }
            }

            return FALSE;
        }
        BOOL CTrueTypeFont::GlyphDebugDelBreakPoint(FE_FontFace face, FE_DebugInfo debugInfo, int pos)
        {
            if (debugInfo == 0) return FALSE;

            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            for (std::map<int, int>::iterator it = pDebugInfo->instrMap.begin(); it != pDebugInfo->instrMap.end(); it++)
            {
                if (it->second == pos)
                {
                    for (size_t i = 0; i < m_pGState->breakpoints.size(); i++)
                    {
                        if (m_pGState->breakpoints[i]->pos == it->first && m_pGState->breakpoints[i]->pInstr == m_pGState->pInstr)
                        {
                            m_pGState->breakpoints.erase(m_pGState->breakpoints.begin() + i);
                            return TRUE;
                        }
                    }
                    break;
                }
            }

            return FALSE;
        }

    }
};
