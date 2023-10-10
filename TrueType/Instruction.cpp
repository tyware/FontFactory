#include "stdafx.h"
#include "Instruction.h"
#include "InstrCmd.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        extern  CInstrCommand TT_CommandList;

        CGState::CGState()
        {
            m_pStorage = NULL;
            m_pTwilightZone = NULL;
            ResetGState();
            m_pFuncDefs = NULL;
        }

        CGState::~CGState()
        {
            ResetGState();
        }

        void CGState::Reset()
        {
            rp0 = 0; rp1 = 0;  rp2 = 0; 
            zp0 = zp1 = zp2 = TTZP_NORMAL; 
            loop = 1; minDist = 1.0f; cvtCutin = 1.0625f; singWidCut = 0.0f; singWidVal = 0.0f;
            autoFlip = true; deltaBase = 9; deltaShift = 3; rndState = TTRS_TO_GRID; sRndPeriod = 1.0f; sRndPhase = 0.0f; sRndThreshold = 0.0f;
            instrControl = 0; scanControl = false; scanType = 0; 
            
            fontSize = 12; ppem = 12; unitsPerPixel = 100;
        }
        void CGState::ResetGState()
        {
            Reset();

            m_pFuncDefs = NULL;

            m_nStorage = 0;

            pOrigOutlines = pOutlines = NULL;

            if( m_pTwilightZone)
                delete[] m_pTwilightZone;
            m_nTwilightPoints = 0;
            m_pTwilightZone = NULL;

            for(size_t i = 0; i<nestedFuncs.size(); i++)
                delete nestedFuncs[i];
            nestedFuncs.clear();
            //nestedIfElse.clear();

            if(m_pStorage)
                delete[] m_pStorage;
            m_pStorage = NULL;

            m_stack.reset();

            for (size_t i = 0; i<breakpoints.size(); i++)
                delete breakpoints[i];
            breakpoints.clear();
        }
        void CGState::Copy(CGState *pGS)
        {
            rp0 = pGS->rp0;
            rp1 = pGS->rp1;
            rp2 = pGS->rp2;
            zp0 = pGS->zp0;
            zp1 = pGS->zp1;
            zp2 = pGS->zp2;
            loop = pGS->loop;
            minDist = pGS->minDist;
            cvtCutin = pGS->cvtCutin;
            singWidCut = pGS->singWidCut;
            singWidVal = pGS->singWidVal;
            autoFlip = pGS->autoFlip;
            deltaBase = pGS->deltaBase;
            deltaShift = pGS->deltaShift;
            rndState = pGS->rndState;
            sRndPeriod = pGS->sRndPeriod;
            sRndPhase = pGS->sRndPhase;
            sRndThreshold = pGS->sRndThreshold;
            instrControl = pGS->instrControl;
            scanControl = pGS->scanControl;
            scanType = pGS->scanType;
            ppem = pGS->ppem;
            fontSize = pGS->fontSize;
            unitsPerPixel = pGS->unitsPerPixel;

            m_nStorage = pGS->m_nStorage;
            if(m_nStorage > 0)
            {
                m_pStorage = new ULong[m_nStorage];
                memcpy(m_pStorage, pGS->m_pStorage, sizeof(ULong) * m_nStorage);
            }
            //m_stack = pGS->m_stack;
            m_nTwilightPoints = pGS->m_nTwilightPoints;
            if(m_nTwilightPoints > 0 && pGS->m_pTwilightZone)
            {
                m_pTwilightZone = new CGlyphCoordinate[m_nTwilightPoints];
                memcpy(m_pTwilightZone, pGS->m_pTwilightZone, m_nTwilightPoints * sizeof(CGlyphCoordinate));
            }
            m_pFuncDefs = pGS->m_pFuncDefs;

            for(size_t i = 0; i<nestedFuncs.size(); i++)
            {
                TT_NestedFunc *pF = new TT_NestedFunc(pGS->nestedFuncs[i]->offset, pGS->nestedFuncs[i]->pInstr, pGS->nestedFuncs[i]->nInstr,
                    pGS->nestedFuncs[i]->loopCallOffset, pGS->nestedFuncs[i]->loopCallTimes, pGS->nestedFuncs[i]->bIsOpCode);
                nestedFuncs.push_back(pF);
            }

            for (size_t i = 0; i < breakpoints.size(); i++)
            {
                CDebugBreakPoint *bp = new CDebugBreakPoint(pGS->breakpoints[i]->pos, pGS->breakpoints[i]->pInstr);
                breakpoints.push_back(bp);
            }

            //nestedIfElse = pGS->nestedIfElse;
        };


        //CInstruction::CInstruction(void)
        //{
        //}


        //CInstruction::~CInstruction(void)
        //{
        //}

        void* RunFontProgram(ULong nFpgm, Byte *pFpgm, ULong nCvt, Long *pCvt, ULong nStorage, std::map<ULong, TT_NestedFunc*> *funcDefs)
        {
            CGState *pGState = new CGState();
            pGState->m_nStorage = nStorage;
            if(pGState->m_nStorage > 0)
            {
                pGState->m_pStorage = new ULong[pGState->m_nStorage];
                for (ULong i = 0; i < pGState->m_nStorage; i++)
                    pGState->m_pStorage[i] = 87654321;
            }
            pGState->m_pFuncDefs = funcDefs;

            RunInstrs(pGState, false, nFpgm, pFpgm, nCvt, pCvt, 0, NULL, NULL);

            return pGState;
        }

        void* RunCvtProgram(void* gState, Short ppem, Short fontSize, float unitspp, ULong nPrep, Byte *pPrep, ULong nCvt, Long *pCvt)
        {
            CGState *pGState = new CGState();
            if(gState)
                pGState->Copy((CGState*)gState);

            pGState->ppem = ppem;
            pGState->fontSize = fontSize;
            pGState->unitsPerPixel = unitspp;

            RunInstrs(pGState, false, nPrep, pPrep, nCvt, pCvt, 0, NULL, NULL);

            return pGState;
        }

        void RunInstrs(void* gInitialState, bool bCreateGS, ULong nInstr, Byte *pInstr, ULong nCvt, Long *pCvt, ULong maxTwilightPoints, CGlyphOutlines *pOrigOutlines, CGlyphOutlines *pOutlines)
        {
            if(pInstr == NULL || nInstr == 0) return;

            CGState *m_pGState = (CGState*)gInitialState;

            if(bCreateGS)
            {
                m_pGState = new CGState();
                if(gInitialState)
                    m_pGState->Copy((CGState*)gInitialState);

                m_pGState->m_nTwilightPoints = maxTwilightPoints;
                if(maxTwilightPoints > 0)
                    m_pGState->m_pTwilightZone = new CGlyphCoordinate[maxTwilightPoints];
            }

            m_pGState->nInstr = nInstr;
            m_pGState->pInstr = pInstr;
            m_pGState->nOffset = 0;
            m_pGState->nCvt = nCvt;
            m_pGState->pCvt = pCvt;
            m_pGState->pOrigOutlines = pOrigOutlines;
            m_pGState->pOutlines = pOutlines;

            while (m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                m_pGState->curOpCode = (TT_INSTR_CMD)m_pGState->pInstr[m_pGState->nOffset++];
                if ((*TT_CommandList.OpProcs[m_pGState->curOpCode])(m_pGState)) // the instruction fails
                {
                    int tt = 0; tt++;
                    break;
                }
            }

            if(bCreateGS)
                delete m_pGState;
        }

        float _FixedToFloat(Long v)
        {
            return v / 64.0f;
            //Short mantissa = (Short)(v >> 16);
            //float res = (v & 0xFFFF) / 65536.0f;
            //if((mantissa & 0x8000)) res = mantissa - res; // negative
            //else res = mantissa - res; // positive
            //return res;
        }
        Long _FloatToFixed(float v)
        {
            return (Long) (v * 64);
        }
        float _F2DOT14ToFloat(Short v)
        {
            return v / 16384.0f;
            //float res = ((v >> 14) & 1) + (v & 0x3FFF) / 16384.0f;
            //if(v >> 15) res = -res;
            //return res;
        }
        Short _FloatToF2DOT14(float v)
        {
            return (Short)(v * 16384);
        }

    };

};


