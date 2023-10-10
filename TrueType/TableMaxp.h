#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableMaxp
        {
        public:
            CTableMaxp(void);
            ~CTableMaxp(void);
            void Load(void* ptr, Long offset);

        public:
            float version;
            UShort numGlyphs;
            UShort maxPoints;
            UShort maxContours;
            UShort maxComponentPoints;
            UShort maxComponentContours;
            UShort maxZones;
            UShort maxTwilightPoints;
            UShort maxStorage;
            UShort maxFunctionDefs;
            UShort maxInstructionDefs;
            UShort maxStackElements;
            UShort maxSizeOfInstructions;
            UShort maxComponentElements;
            UShort maxComponentDepth;
        };

    };

};

