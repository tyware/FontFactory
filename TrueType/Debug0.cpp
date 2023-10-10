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

        extern wchar_t tt_gCmdName[256][16];

        FE_DebugInfo CTrueTypeFont::GlyphDebugStart0(FE_FontFace face, int cid, int set)
        {
            if (set == 0)
            {
                int glyphIndex = cid;
                CTTFontFace *pFace = (CTTFontFace*)face;
                if (pFace->m_pCMap)
                    glyphIndex = GetGlyphIndex(pFace->m_pCMap, cid);
                CFeGlyph* pGlyph = GetGlyph(face, cid);
                if (pGlyph == nullptr) GetGlyphByIndex(face, cid);
                if (pGlyph)
                {
                    CGlyphDef *pGlyphDef = pTableGlyf->GetGlyphObj(glyphIndex);
                    CGlyphOutlines *pOutlines = BuildGlyphOrigOutline(pFace, cid, pGlyphDef);

                    // run instructions
                    int advanceWidth = (int)(pGlyph->m_advanceWidth / pFace->m_fScale);
                    int lsb = (int)(pGlyph->m_lsb / pFace->m_fScale);

                    ////////////////////////////////////////////////////////////////////////
                    // run instruction
                    Byte *pInstr;
                    //UShort nInstr = pGlyphDef->GetInstructions(0, &pInstr);
                    UShort nInstr = pGlyphDef->GetInstructions(&pInstr);
                    CGlyphOutlines *pOrigOutlines = NULL;

                    AppendPhantomPointsAndScale(pOutlines, advanceWidth, lsb, (int)pGlyphDef->xMin, &pOrigOutlines, pFace->m_fScale);

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
                        pDebugInfo->pData = m_pGState;
                        pDebugInfo->pOutlines = pOutlines;
                        pDebugInfo->pOrigOutlines = pOrigOutlines;
                        pDebugInfo->pTwilights = m_pGState->m_pTwilightZone;
                        pDebugInfo->nTwilights = m_pGState->m_nTwilightPoints;
                        pDebugInfo->currentPos = 0;
                        pDebugInfo->intructionSet = set;
                        SetDebugState(m_pGState, pDebugInfo);
                        SetDebugStack(m_pGState, pDebugInfo);
                        SetDebugStorage(m_pGState, pDebugInfo);
                        SetDebugCvt(m_pGState, pDebugInfo);
                        CreateInstrCmdList(m_pGState->nInstr, m_pGState->pInstr, pDebugInfo->instructions, pDebugInfo->instrMap);
                        return pDebugInfo;
                    }
                }
                else return nullptr;
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
        BOOL CTrueTypeFont::GlyphDebugContinue0(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;
            while (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                {
                    int tt = 0; tt++;
                    break;
                }

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
        BOOL CTrueTypeFont::GlyphDebugStepOver0(FE_FontFace face, FE_DebugInfo debugInfo)
        {
            CGlyphDebugInfo *pDebugInfo = (CGlyphDebugInfo*)debugInfo;
            CGState *m_pGState = (CGState*)pDebugInfo->pData;

            if (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                    return FALSE;

                if (m_pGState->curOpCode == TT_INSTR_CMD::TT_ENDF)
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
        BOOL CTrueTypeFont::GlyphDebugStepIn0(FE_FontFace face, FE_DebugInfo debugInfo)
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
        BOOL CTrueTypeFont::GlyphDebugStepOut0(FE_FontFace face, FE_DebugInfo debugInfo)
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
        BOOL CTrueTypeFont::GlyphDebugEnd0(FE_FontFace face, FE_DebugInfo debugInfo)
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
                delete pDebugInfo;
            }
            return TRUE;
        }
   };
};
