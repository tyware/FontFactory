#include "stdafx.h"
#include "BezierCurve.h"
#include <vector>

namespace FF_Font_Factory
{
    double FactorialLookup[33] = {
            1.0, 1.0, 2.0, 6.0,
            24.0, 120.0,720.0,5040.0,
            40320.0, 362880.0, 3628800.0, 39916800.0,
            479001600.0, 6227020800.0, 87178291200.0, 1307674368000.0,
            20922789888000.0, 355687428096000.0, 6402373705728000.0, 121645100408832000.0,
            2432902008176640000.0, 51090942171709440000.0, 1124000727777607680000.0, 25852016738884976640000.0,
            620448401733239439360000.0, 15511210043330985984000000.0, 403291461126605635584000000.0, 10888869450418352160768000000.0,
            304888344611713860501504000000.0, 8841761993739701954543616000000.0, 265252859812191058636308480000000.0, 8222838654177922817725562880000000.0,
            263130836933693530167218012160000000.0,
    };

    double factorial(int n)
    {
        return FactorialLookup[n]; /* returns the value n! as a SUMORealing point number */
    }

    double Ni(int n, int i)
    {
        double ni;
        double a1 = factorial(n);
        double a2 = factorial(i);
        double a3 = factorial(n - i);
        ni =  a1/ (a2 * a3);
        return ni;
    }

    // Calculate Bernstein basis
    double Bernstein(int n, int i, double t)
    {
        double basis;
        double ti; /* t^i */
        double tni; /* (1 - t)^i */

        /* Prevent problems with pow */

        if (t == 0.0 && i == 0) 
            ti = 1.0; 
        else 
            ti = pow(t, i);

        if (n == i && t == 1.0) 
            tni = 1.0; 
        else 
            tni = pow((1 - t), (n - i));

        //Bernstein basis
        basis = Ni(n, i) * ti * tni; 
        return basis;
    }

    int CalculateNumPoints(CGlyphCoordinate &coord, float prevX, float prevY, float scale = 1.0f)
    {
        int nx = (int)(abs(coord.x - prevX) / 1 * scale) + 1;
        int ny = (int)(abs(coord.y - prevY) / 1 * scale) + 1;
        if(nx > ny) return nx;
        else return ny;
    }

    void BezierCurve(CGlyphCoordinate *coords, int numPoints, CGlyphCoordinate *extra, CRealPoint **resPoints, int &numResPoints)
    {
        bool bStart = false;
        float x, y;
        numResPoints = 0;

        // calculate the number of result points
        for(int i = 0; i< numPoints; i++)
        {
            if((coords[i].bOnCurve & PF_ONCURVE) == PF_ONCURVE)
            {
                x = coords[i].x;
                y = coords[i].y;
                if(!bStart)
                    bStart = true;
                else
                {
                    numResPoints += CalculateNumPoints(coords[i], x, y, 1.0f);
                    break;
                }
            }
            else
            {
                numResPoints += CalculateNumPoints(coords[i], x, y, 1.0f);
                if(bStart)
                {
                    x = coords[i].x;
                    y = coords[i].y;
                }
            }
        }
        if(extra)
            numResPoints += CalculateNumPoints(*extra, x, y, 1.0f);

        // Calculate points on curve
        double step, t;
        t = 0;
        step = (double)1.0 / (numResPoints - 1);

        *resPoints = new CRealPoint[numResPoints];

        int count = 0;
        int nExtra = (extra == NULL? 0 : 1);
        for (int i1 = 0; i1 < numResPoints; i1++)
        { 
            if ((1.0 - t) < 5e-6) 
                t = 1.0;

            for (int i = 0; i < numPoints; i++)
            {
                double basis = Bernstein(numPoints + nExtra - 1, i, t);
                //(*resPoints)[i1].x += (float)(basis * coords[i].x);
                //(*resPoints)[i1].y += (float)(basis * coords[i].y);
                (*resPoints)[count].x += (float)(basis * coords[i].x);
                (*resPoints)[count].y += (float)(basis * coords[i].y);
            }
            if(extra)
            {
                double basis = Bernstein(numPoints + nExtra - 1, numPoints, t);
                (*resPoints)[count].x += (float)(basis * extra->x);
                (*resPoints)[count].y += (float)(basis * extra->y);
            }

            if(count > 0)
            {
                if((*resPoints)[count].x != (*resPoints)[count -1].x || (*resPoints)[count].y != (*resPoints)[count -1].y)
                    count++;
            }
            else count++;

            t += step;
        }

        numResPoints = count;
    }

    
    void BezierCurvePoints(CGlyphCoordinate *coords, std::vector<FF_Font_Factory::CGlyphCoordinate> &contourPoints, int numPoints, float scale)
    {
        bool bStart = false;
        float x, y;
        int numResPoints = 0;

        // calculate the number of result points
        for(int i = 0; i< numPoints; i++)
        {
            if((coords[i].bOnCurve & PF_ONCURVE) == PF_ONCURVE)
            {
                x = coords[i].x;
                y = coords[i].y;
                if(!bStart)
                    bStart = true;
                else
                {
                    numResPoints += CalculateNumPoints(coords[i], x, y, scale);
                    break;
                }
            }
            else
            {
                if(bStart)
                {
                    numResPoints += CalculateNumPoints(coords[i], x, y, scale);
                    x = coords[i].x;
                    y = coords[i].y;
                }
            }
        }

        // Calculate points on curve
        double step, t;
        t = 0;
        step = (double)1.0 / (numResPoints - 1);

        int count = 0;
        for (int i1 = 0; i1 < numResPoints; i1++)
        { 
            CGlyphCoordinate point;
            point.x = point.y = 0.0f;

            if ((1.0 - t) < 5e-6) 
                t = 1.0;

            for (int i = 0; i < numPoints; i++)
            {
                double basis = Bernstein(numPoints-1, i, t);
                point.x += (float)(basis * coords[i].x);
                point.y += (float)(basis * coords[i].y);
            }

            if(count > 0)
            {
                if(contourPoints[count-1].x != point.x || contourPoints[count-1].y != point.y)
                {
                    contourPoints.push_back(point);
                    count++;
                }
            }
            else
            {
                contourPoints.push_back(point);
                count++;
            }

            t += step;
        }

        numResPoints = count;
    }


    CGlyphOutlines* _ConvertOutlinesToPoints(FF_Font_Factory::CGlyphOutlines *pOutlines, float scale)
    {
        CGlyphOutlines* pResOutlines = NULL;
        std::vector<FF_Font_Factory::CGlyphCoordinate> contourPoints;
        unsigned int *pContourNumPoints = NULL;

        if(pOutlines->numContours > 0)
            pContourNumPoints = new unsigned int[pOutlines->numContours];

        int count = 0;
        for (int i = 0; i< pOutlines->numContours; i++)
        {
            pContourNumPoints[i] = 0;
            unsigned int firstOnCurvePoint = count;
            FF_Font_Factory::CGlyphCoordinate prevOnCurvePoint, firstPoint;
            FF_Font_Factory::CGlyphCoordinate arrayPts[3];
            for(unsigned int j = count; pOutlines->pEndPtsOfContours[i]!= 0xFFFF && j <= pOutlines->pEndPtsOfContours[i]; j++)
            {
                if((pOutlines->pCoordinates[j].bOnCurve & FF_Font_Factory::PF_ONCURVE) == FF_Font_Factory::PF_OFFCURVE)
                {
                    if (j == firstOnCurvePoint) // first point is a off-curve point
                    {
                        if (pOutlines->pCoordinates[pOutlines->pEndPtsOfContours[i]].bOnCurve)
                            arrayPts[0] = pOutlines->pCoordinates[pOutlines->pEndPtsOfContours[i]];
                        else
                        {
                            arrayPts[0].bOnCurve = true;
                            arrayPts[0].x = (pOutlines->pCoordinates[j].x + pOutlines->pCoordinates[pOutlines->pEndPtsOfContours[i]].x) / 2;
                            arrayPts[0].y = (pOutlines->pCoordinates[j].y + pOutlines->pCoordinates[pOutlines->pEndPtsOfContours[i]].y) / 2;
                        }
                        arrayPts[1] = pOutlines->pCoordinates[j];
                        if (pOutlines->pCoordinates[j + 1].bOnCurve)
                            arrayPts[2] = pOutlines->pCoordinates[++j];
                        else
                        {
                            arrayPts[2].bOnCurve = true;
                            arrayPts[2].x = (pOutlines->pCoordinates[j].x + pOutlines->pCoordinates[j + 1].x) / 2;
                            arrayPts[2].y = (pOutlines->pCoordinates[j].y + pOutlines->pCoordinates[j + 1].y) / 2;
                        }
                        FF_Font_Factory::BezierCurvePoints(arrayPts, contourPoints, 3, scale);
                        firstPoint = arrayPts[0];
                        prevOnCurvePoint = arrayPts[2];
                    }
                    else
                    {
                        arrayPts[0] = prevOnCurvePoint;
                        arrayPts[1] = pOutlines->pCoordinates[j];
                        if (j == pOutlines->pEndPtsOfContours[i])
                        {
                            if (pOutlines->pCoordinates[count].bOnCurve)
                                arrayPts[2] = pOutlines->pCoordinates[count];
                            else
                            {
                                arrayPts[2].bOnCurve = true;
                                arrayPts[2].x = (pOutlines->pCoordinates[j].x + pOutlines->pCoordinates[count].x) / 2;
                                arrayPts[2].y = (pOutlines->pCoordinates[j].y + pOutlines->pCoordinates[count].y) / 2;
                            }
                        }
                        else
                        {
                            if (pOutlines->pCoordinates[j + 1].bOnCurve)
                                arrayPts[2] = pOutlines->pCoordinates[++j];
                            else
                            {
                                arrayPts[2].bOnCurve = true;
                                arrayPts[2].x = (pOutlines->pCoordinates[j].x + pOutlines->pCoordinates[j+1].x) / 2;
                                arrayPts[2].y = (pOutlines->pCoordinates[j].y + pOutlines->pCoordinates[j+1].y) / 2;
                            }

                        }
                        FF_Font_Factory::BezierCurvePoints(arrayPts, contourPoints, 3, scale);
                        prevOnCurvePoint = arrayPts[2];
                        if (j == pOutlines->pEndPtsOfContours[i])
                            contourPoints.push_back(firstPoint);
                    }
                }
                else
                {
                    contourPoints.push_back(pOutlines->pCoordinates[j]);
                    prevOnCurvePoint = pOutlines->pCoordinates[j];
                    if (j == count)
                        firstPoint = prevOnCurvePoint;
                    else if (j == pOutlines->pEndPtsOfContours[i])
                        contourPoints.push_back(firstPoint);
                }
            }

            pContourNumPoints[i] = (unsigned int)contourPoints.size() - 1;
            count = pOutlines->pEndPtsOfContours[i] + 1;
        }

        if(contourPoints.size() > 0)
        {
            pResOutlines = new CGlyphOutlines();
            pResOutlines->numContours = pOutlines->numContours;
            pResOutlines->numPoints = (unsigned int)contourPoints.size();
            pResOutlines->pEndPtsOfContours = pContourNumPoints;
            pResOutlines->pCoordinates = new CGlyphCoordinate[pResOutlines->numPoints];
            for(int i = 0; i < pResOutlines->numPoints; i++)
                pResOutlines->pCoordinates[i] = contourPoints[i];
        }
        else
        {
            if(pContourNumPoints) delete[] pContourNumPoints;
            contourPoints.clear();
        }

        return pResOutlines;
    }
  
};
