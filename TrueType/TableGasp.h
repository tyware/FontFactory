#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        enum _GASP_FLAGS
        {
            kGASPGridFit = 1,
            kGASPDoGray  = 2
        };

        class CTableGasp
        {
        public:
            class CGaspRange
            {
            public:
                UShort rangeMaxPPEM;
                UShort rangeGaspBehavior;
            };

        public:
            CTableGasp(void);
            ~CTableGasp(void);
            void Load(void* ptr, Long offset);

        public:
            UShort version;
            UShort numRanges;
            CGaspRange *pGaspRange;
        };

    };

};

