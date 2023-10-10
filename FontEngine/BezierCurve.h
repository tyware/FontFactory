#pragma once
#include "BaseFont.h"
#include "RealPoint.h"

namespace FF_Font_Factory
{
    __declspec(dllexport) void BezierCurve(CGlyphCoordinate *coords, int numPoints, CGlyphCoordinate *extra, CRealPoint **resPoints, int &numResPoints);
    __declspec(dllexport) CGlyphOutlines* _ConvertOutlinesToPoints(FF_Font_Factory::CGlyphOutlines *pOutlines, float scale = 1.0f);

};


