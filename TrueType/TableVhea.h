#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableVhea
        {
        public:
            CTableVhea(void);
            ~CTableVhea(void);
            void Load(void* ptr, Long offset);

        public:
            float version; // Fixed32
            Short vertTypoAscender;
            Short vertTypoDescender;
            Short vertTypoLineGap;
            Short advanceHeightMax;
            Short minTopSideBearing;
            Short minBottomSideBearing;
            Short yMaxExtent;
            Short caretSlopeRise;
            Short caretSlopeRun;
            Short caretOffset;
            Short reserved1;
            Short reserved2;
            Short reserved3;
            Short reserved4;
            Short metricDataFormat;
            UShort numOfLongVerMetrics;
        };

    };

};


