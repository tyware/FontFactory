#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableLoca
        {
        public:
            CTableLoca(void);
            ~CTableLoca(void);
            void Load(void* ptr, Long offset);

        public:
            Long numGlyphs;
            BOOL bIsShortFormat;
            ULong *pOffset;
        };

    };

};

