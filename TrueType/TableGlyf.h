#pragma once

#include "Global.h"
#include <vector>
#include <map>
#include "BaseFont.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        enum SimpleGlyphFlags
        {
            ON_CURVE_POINT                       = 0x01,
            X_SHORT_VECTOR                       = 0x02,
            Y_SHORT_VECTOR                       = 0x04,
            REPEAT_FLAG                          = 0x08,
            X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,
            Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,
        };

        enum CompoundGlyphFlags
        {
            ARG_1_AND_2_ARE_WORDS     = 0x0001,
            ARGS_ARE_XY_VALUES        = 0x0002,
            ROUND_XY_TO_GRID          = 0x0004,
            WE_HAVE_A_SCALE           = 0x0008,
            NON_OVERLAPPING           = 0x0010,
            MORE_COMPONENTS           = 0x0020,
            WE_HAVE_AN_X_AND_Y_SCALE  = 0x0040,
            WE_HAVE_A_TWO_BY_TWO      = 0x0080,
            WE_HAVE_INSTRUCTIONS      = 0x0100,
            USE_MY_METRICS            = 0x0200,
            OVERLAP_COMPOUND          = 0x0400,
            SCALED_COMPONENT_OFFSET   = 0x0800,
            UNSCALED_COMPONENT_OFFSET = 0x1000
        };

        /*class CGlyphInstrs
        {
        public:
            CGlyphInstrs();
            CGlyphInstrs(UShort nInstrs);
            ~CGlyphInstrs();

        public:
            int m_nContourStart;
            int m_nPointStart, m_nPointEnd;
            UShort nInstructions;
            Byte *pInstructions;
        };*/

        class CGlyphDef
        {
        public:
            CGlyphDef();
            virtual ~CGlyphDef();
            CGlyphOutlines* GetOutline();
            UShort GetInstructions(Byte **pInstr);
            //UShort GetInstructions(int index, Byte **pInstr);

            Short numberOfContours;
            FWord xMin;
            FWord yMin;
            FWord xMax;
            FWord yMax;

            UShort nInstructions;
            Byte *pInstructions;
            //std::vector<CGlyphInstrs*> instructions;

            CGlyphDef *pRef;
        };

        class CSimpleGlyph : public CGlyphDef
        {
        public:
            CSimpleGlyph();
            virtual ~CSimpleGlyph();

            CGlyphOutlines  outlines;
            //UShort *pEndPtsOfContours;
            //Long numPoints;
            //CGlyphCoordinate *pCoordinates; // number of coordinates is the last value of pEndPtsOfContours + 1
        };

        class CCompoundGlyph : public CGlyphDef
        {
        public:
            class CComponent
            {
            public:
                UShort flags;
                UShort glyphid;
                Short arg1;
                Short arg2;
                bool isPoints;

                float a, b, c, d, e, f; //F2Dot14

                bool UseMyMetrics()
                {
                    if((flags & USE_MY_METRICS) != 0) return true;
                    return false;
                }
            };

        public:
            CCompoundGlyph();
            virtual ~CCompoundGlyph();

            std::vector<CComponent*> components;
            //UShort nInstructions;
            //Byte *pInstructions;
        };

        class CTableGlyf
        {
        public:
            CTableGlyf(void);
            ~CTableGlyf(void);
            void Load(void* ptr, Long offset, ULong *offsetArray);
            CGlyphDef* GetGlyphObj(Long index);

        public:

            Long numGlyphs;
            std::vector<CGlyphDef*> glyphArray; // all unique glyph objects.
            std::map<Long, CGlyphDef*> glyphMap; // all glyphs ordered by glyph index, can map to same glyph
        };

    };

};


