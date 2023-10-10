#pragma once

#include "Global.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTableHdmx
        {
        public:
            class CHdmxRecord
            {
            public:
                CHdmxRecord() : pWidths(0){};
                ~CHdmxRecord()
                {
                    if(pWidths) delete[] pWidths;
                };

            public:
                Byte pixelSize;
                Byte maxWidth;
                Byte *pWidths;
            };

        public:
            CTableHdmx(void);
            ~CTableHdmx(void);
            void Load(void* ptr, Long offset);

        public:
            Short version;
            Short numRecords;
            Long sizeOfRecord;
            CHdmxRecord *pRecords;

            ///////
            Long numGlyphs;
        };

    };
};
