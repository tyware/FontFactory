#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        class CTableHhea
        {
        public:
            CTableHhea(void);
            ~CTableHhea(void);
            void Load(void* ptr, Long offset);

        public:
            float version;
            FWord ascender;
            FWord descender;
            FWord lineGap;
            UFWord advanceWidthMax;
            FWord minLeftSideBearing;
            FWord minRightSideBearing;
            FWord xMaxExtent;
            Short caretSlopeRise;
            Short caretSlopeRun;
            Short caretOffset;
            Short reserved1;
            Short reserved2;
            Short reserved3;
            Short reserved4;
            Short metricDataFormat;
            UShort numOfLongHorMetrics;
        };

    };

};
