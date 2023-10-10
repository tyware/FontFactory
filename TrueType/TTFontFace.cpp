#include "stdafx.h"
#include "TTFontFace.h"
#include "Instruction.h"


namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {

        CTTFontFace::CTTFontFace(void)
            :m_pStartGState(0), m_pCvt(0)
        {
        }
        CTTFontFace::CTTFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags)
            :CFontFace(hFont, fontSize, dpi, flags), m_pStartGState(0), m_pCvt(0)
        {
        }
        CTTFontFace::CTTFontFace(FE_Font hFont, int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
            :CFontFace(hFont, fontSize, dpi, flags, platformId, encodingId), m_pStartGState(0), m_pCvt(0)
        {
        }


        CTTFontFace::~CTTFontFace(void)
        {
            if(m_pStartGState) delete (CGState*)m_pStartGState;
            if(m_pCvt) delete[] m_pCvt;
        }

    };

};
