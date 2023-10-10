#pragma once
#include "Global.h"
#include <vector>
//#include <stack>
#include <map>
#include "FeStack.h"
#include "BaseFont.h"
#include <string>

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        typedef enum
        {
            TTDS_GREY = 0,
            TTDS_BLACK = 1,
            TTDS_WHITE = 2,
            TTDS_NONE = 3
        }TT_DISTANCE_TYPE;

        typedef enum
        {
            TTZP_TWILIGHT,
            TTZP_NORMAL
        }TT_ZONE_POINTER;
        typedef enum
        {
            TTRS_OFF,
            TTRS_TO_GRID,
            TTRS_TO_HALF_GRID, // only to half grid, not including whole grid, e.g. 0.5, 1.5, not 1, 2
            TTRS_TO_DOUBLE_GRID, // 0.5, 1, 1.5, 2 ...
            TTRS_DOWN_GRID,
            TTRS_UP_GRID,
            TTRS_SUPER_45,
            TTRS_SUPER
        }TT_ROUND_STATE;
        typedef enum
        {
            TTIC_NOT_RUN_GLYPH_INSTRUCTS = 1,
            TTIC_IGNORE_CVT_CHANGES = 2
        }TT_INSTRUCT_CONTROL;

        class TT_Vector
        {
        public:
            TT_Vector():x(1), y(0){};
            float x, y;
        };
        class TT_NestedFunc
        {
        public:
            TT_NestedFunc(ULong _offset, Byte *_pInstr, ULong _nInstr, ULong loopOffset, Long executeTimes, bool bOpCode)
                : offset(_offset), nInstr(_nInstr), pInstr(_pInstr),
                loopCallOffset(loopOffset), loopCallTimes(executeTimes), bIsLoopCall(executeTimes > 1),  bIsOpCode(bOpCode)
            {};

        public:
            ULong offset;
            ULong nInstr;
            Byte *pInstr;

            ULong loopCallOffset;
            Long loopCallTimes;
            bool bIsLoopCall;
            bool bIsOpCode;
        };

        class CDebugBreakPoint
        {
        public:
            CDebugBreakPoint() :pos(0), pInstr(0){};
            CDebugBreakPoint(int _pos, Byte *_pInstr) :pos(_pos), pInstr(_pInstr){};

            int pos;
            Byte *pInstr;
        };

        class CGState
        {
        public:
            CGState();
            ~CGState();

            void Reset();
            void ResetGState();
            void Copy(CGState *pGS);

        public:
            int rp0, rp1, rp2;
            int loop;
            TT_ZONE_POINTER zp0, zp1, zp2; // 0 - Z0: twilight zone, 1 - Z1: normal glyph zone (outlines)
            float minDist;
            float cvtCutin;
            float singWidCut;
            float singWidVal;
            TT_Vector freeVec, projVec, dualVec;
            bool autoFlip;
            int deltaBase;
            int deltaShift;
            TT_ROUND_STATE rndState;
            float sRndPeriod;
            float sRndPhase;
            float sRndThreshold;
            int instrControl;
            bool scanControl;
            int scanType;
            int ppem;
            int fontSize;
            float unitsPerPixel;

            // other dynamic parameters
            CFeStack<ULong> m_stack;
            std::map<ULong, TT_NestedFunc*> *m_pFuncDefs;
            ULong m_nStorage;
            ULong *m_pStorage;
            ULong m_nTwilightPoints;
            CGlyphCoordinate *m_pTwilightZone;

            // run-time data
            ULong nInstr;
            Byte *pInstr;
            Long nOffset;
            ULong nCvt;
            Long *pCvt;
            CGlyphOutlines *pOrigOutlines;
            CGlyphOutlines *pOutlines;
            std::vector<TT_NestedFunc*> nestedFuncs;
            //std::vector<Byte> nestedIfElse;

            Byte curOpCode;

            std::vector<CDebugBreakPoint*> breakpoints;
        };

        float _FixedToFloat(Long v);
        Long _FloatToFixed(float v);
        float _F2DOT14ToFloat(Short v);
        Short _FloatToF2DOT14(float v);

        //class CInstruction
        //{
        //public:
        //    CInstruction(void);
        //    ~CInstruction(void);

            void* RunFontProgram(ULong nFpgm, Byte *pFpgm, ULong nCvt, Long *pCvt, ULong nStorage, std::map<ULong, TT_NestedFunc*> *funcDefs); // Executed after font is loaded. Return a pointer of CGState
            void* RunCvtProgram(void* gState, Short ppem, Short fontSize, float unitspp, ULong nPrep, Byte *pPrep, ULong nCvt, Long *pCvt); // Executed after font face is created. Return a pointer of CGState
            void RunInstrs(void* gInitialState, bool bCreateGS, ULong nInstr, Byte *pInstr, ULong nCvt, Long *pCvt, ULong maxTwilightPoints, CGlyphOutlines *pOrigOutlines, CGlyphOutlines *pOutlines);

            void CreateInstrCmdList(ULong nInstr, Byte *pInstr, std::vector<std::wstring> &res, std::map<int,int> &instrMap);

        //    //void CreateStartGState(); // create m_startGState for every glyph
        //    //void ResetGState();       // reset m_gState to m_startGState for every glyph

        //public:
        //    //// glyph instruction
        //    //Byte *m_iStream;
        //    //Long m_nStreamLength;
        //    // glyph graphics state
        //    //CGState m_gState;

        //    //// other dynamic parameters
        //    //std::vector<ULong> m_storage;
        //    //std::stack<ULong> m_statck;
        //    //std::vector<TT_Vector> m_twlightZone;
        //    //std::vector<TT_Vector> m_glyphZone;

        //};

    };
};
