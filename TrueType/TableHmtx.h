#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableHmtx
        {
        public:
            class longHorMetrix
            {
            public:
                UShort advanceWidth;
                Short leftSideBearing;
            };

        public:
            CTableHmtx(void);
            ~CTableHmtx(void);
            void Load(void* ptr, Long offset);

            Long numMetrix;
            longHorMetrix *pMetrix;
            Long numLeftSideBearing;
            FWord *pLeftSideBearing;
        };

    };

};


