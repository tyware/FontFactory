#pragma once
#include "BaseFont.h"
#include "RealPoint.h"


namespace FF_Font_Factory
{
    class __declspec(dllexport) CNonZeroWinding
    {
    public:
        CNonZeroWinding(void);
        ~CNonZeroWinding(void);
        static bool IsInside(const CRealPoint &point, const CGlyphOutlines *pOutlines);
        static int IsLeftOfLine(const CRealPoint &p0, const CRealPoint &p1, const CRealPoint &point);

    };

};

