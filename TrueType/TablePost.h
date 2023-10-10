#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTablePost
        {
        public:
            CTablePost(void);
            ~CTablePost(void);
            void Load(void* ptr, Long offset);

        public:
            float format;
            float italicAngle;
            FWord underlinePosition;
            FWord underlineThickness;
            ULong isFixedPitch;
            ULong minMemType42;
            ULong maxMemType42;
            ULong minMemType1;
            ULong maxMemType1;

            // format 1, 2, 2.5, 3, and 4 are not supported yet
        };

    };

};


