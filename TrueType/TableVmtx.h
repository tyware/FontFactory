#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableVmtx
        {
        public:
            class longHorMetrix
            {
            public:
                UShort advanceHeight;
                Short topSideBearing;
            };

        public:
            CTableVmtx(void);
            ~CTableVmtx(void);
            void Load(void* ptr, Long offset);

            Long numMetrix;
            longHorMetrix *pMetrix;
            Long numTopSideBearing;
            Short *pTopSideBearing;
        };

    };

};

