#pragma once

#include "Global.h"
#include <vector>

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableKern
        {
        public:
            class CSubtable
            {
            public:
                virtual ~CSubtable(){};
                UShort GetFormat(){ return (coverage >> 8); };

            public:
                UShort version;
                UShort length;
                UShort coverage;
            };
            class CSubtable0 : public CSubtable
            {
            public:
                class CKernPair
                {
                public:
                    UShort left;
                    UShort right;
                    FWord value;
                };

            public:
                CSubtable0():nPairs(0), pKernPair(0){};
                virtual ~CSubtable0()
                {
                    if(pKernPair) delete[] pKernPair;
                };

            public:
                UShort nPairs;
                UShort searchRange;
                UShort entrySelector;
                UShort rangeShift;
                CKernPair *pKernPair;
            };

        public:
            CTableKern(void);
            ~CTableKern(void);
            void Load(void* ptr, Long offset);

        public:
            UShort version;
            UShort nTables;
            std::vector<CSubtable*> subTables;
        };

    };
};
