#include "stdafx.h"
#include "NonZeroWinding.h"

namespace FF_Font_Factory
{
    /*
    
    Quadratic Bézier curves:
    B(t) = (1 -t)* (1-t) * P0 + 2 * ( 1 - t) * t *P1 + t*t *P2, 0<=t<=1, t can be set to 0.5, P0 is start point, P1 is control point, P2 is end point
       set t = 0.5, then
       B(0.5) = 0.25 * P0 + 0.5 * P1 + 0.25 * P2
    
    Cubic Bézier curves:
    B(t) = (1-t) * (1-t) * (1-t) * P0 + 3 * (1-t) * (1-t) * t * P1 + 3 * (1-t) * t * t * P2 + t * t * t * P3, 0 <= t <= 1, P0 is start point, P1 and P2 is control point, P3 is end point
       set t = 0.5, then
       B(0.5) = 0.125 * P0 + 0.375 * P1 + 0.375 * P2 + 0.125 * P3
    
    */

    CNonZeroWinding::CNonZeroWinding(void)
    {
    }


    CNonZeroWinding::~CNonZeroWinding(void)
    {
    }

    bool CNonZeroWinding::IsInside(const CRealPoint &point, const CGlyphOutlines *pOutlines)
    {
        // The winding number counter.
        int winding_number = 0;

        int count = 0;
        for(int i = 0; i< pOutlines->numContours; i++)
        {
            for(unsigned int j = count; pOutlines->pEndPtsOfContours[i]!= 0xFFFF && j <= pOutlines->pEndPtsOfContours[i]; j++)
            {
                CRealPoint point1((float)pOutlines->pCoordinates[j].x, (float)pOutlines->pCoordinates[j].y);
                CRealPoint point2;

                if(j == pOutlines->pEndPtsOfContours[i])
                    point2 = CRealPoint((float)pOutlines->pCoordinates[count].x, (float)pOutlines->pCoordinates[count].y);
                else
                    point2 = CRealPoint((float)pOutlines->pCoordinates[j + 1].x, (float)pOutlines->pCoordinates[j + 1].y);

                if (point1.y <= point.y)                                    // start y <= point.y
                {
                    if (point2.y > point.y)                                 // An upward crossing
                    {
                        if (IsLeftOfLine(point1, point2, point) > 0)        // Point left of edge
                            ++winding_number;                               // Have a valid up intersect
                    }
                }
                else
                {
                    // start y > point.y (no test needed)
                    if (point2.y <= point.y)                                // A downward crossing
                    {
                        if (IsLeftOfLine(point1, point2, point) < 0)        // Point right of edge
                            --winding_number;                               // Have a valid down intersect
                    }
                }
            }
            count = pOutlines->pEndPtsOfContours[i] + 1;
        }

        return (winding_number != 0);
    }

    int CNonZeroWinding::IsLeftOfLine(const CRealPoint &p0, const CRealPoint &p1, const CRealPoint &point)
    {
        return (int)((p1.x - p0.x) * (point.y - p0.y) - (point.x - p0.x) * (p1.y - p0.y));
    }
};

