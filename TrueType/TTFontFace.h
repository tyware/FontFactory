#pragma once
#include "Global.h"
#include "BaseFont.h"
#include "FontFace.h"
#include "TableGlyf.h"

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        class CTTFontFace : public CFontFace
        {
        public:
            CTTFontFace(void);
            CTTFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags);
            CTTFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId);
            virtual ~CTTFontFace(void);

        public:
            void* m_pStartGState;
            int *m_pCvt;
        };

    };

};


