#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableHead
        {
        public:
            CTableHead(void);
            ~CTableHead(void);
            void Load(void* ptr, Long offset);

        public:
            float version;
            float fontRevision;
            ULong checkSumAdjustment;
            ULong magicNumber;
            UShort flags;
            UShort unitsPerEm;
            Char created[8]; // datatime
            Char modified[8]; // datatime
            FWord xMin;
            FWord yMin;
            FWord xMax;
            FWord yMax;
            UShort macStyle;
            UShort lowestRecPPEM;
            Short fontDirectionHint;
            Short indexToLocFormat;
            Short glyphDataFormat;

            BOOL bBaseLineAtY0;
            BOOL bLsbAtX0;
            BOOL bInstrOnPointSize;
            BOOL bIntegerScaling;
            BOOL bVertLayout;
        };

    };

};

