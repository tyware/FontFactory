#include "stdafx.h"
#include "InstrCmd.h"
#include "Instruction.h"
#include <map>
//#include <vector>
#include "TrueType.h"
#include "math.h"

#define MEASURE_DISTANCE_ROUND          0.5f

namespace FF_Font_Factory
{

    namespace FF_Font_Factory_TrueType
    {
        extern  CInstrCommand TT_CommandList;

        int tt_GetNextCmdBytes(CGState *m_pGState)
        {
            Byte n = 0;
            switch(m_pGState->pInstr[m_pGState->nOffset])
            {
            case TT_NPUSHB:
                n = m_pGState->pInstr[m_pGState->nOffset + 1] + 1;
                break;
            case TT_NPUSHW:
                n = m_pGState->pInstr[m_pGState->nOffset + 1] * 2 + 1;
                break;
            case TT_PUSHB1:
                n++;
                break;
            case TT_PUSHB2:
                n += 2;
                break;
            case TT_PUSHB3:
                n += 3;
                break;
            case TT_PUSHB4:
                n += 4;
                break;
            case TT_PUSHB5:
                n += 5;
                break;
            case TT_PUSHB6:
                n += 6;
                break;
            case TT_PUSHB7:
                n += 7;
                break;
            case TT_PUSHB8:
                n += 8;
                break;
            case TT_PUSHW1:
                n += 2;
                break;
            case TT_PUSHW2:
                n += 4;
                break;
            case TT_PUSHW3:
                n += 6;
                break;
            case TT_PUSHW4:
                n += 8;
                break;
            case TT_PUSHW5:
                n += 10;
                break;
            case TT_PUSHW6:
                n += 12;
                break;
            case TT_PUSHW7:
                n += 14;
                break;
            case TT_PUSHW8:
                n += 16;
                break;
            }
            n++;
            return n;
        }

        void tt_MarkTouched(CGState *m_pGState, CGlyphCoordinate *newPt)
        {
            if(m_pGState->freeVec.x != 0)
                newPt->bOnCurve |= PF_TOUCHED_X;
            if (m_pGState->freeVec.y != 0)
                newPt->bOnCurve |= PF_TOUCHED_Y;
        }
        // get point location on project vector
        void tt_GetLocation(TT_Vector &vector, CGlyphCoordinate *point, CGlyphCoordinate *res)
        {
            if (FEqual(vector.x, .0))
            {
                res->x = 0;
                if (vector.y >= 0)
                    res->y = point->y;
                else
                    res->y = -point->y;
            }
            else if (FEqual(vector.y, .0))
            {
                res->y = 0;
                if (vector.x >= 0)
                    res->x = point->x;
                else
                    res->x = -point->x;
            }
            else
            {
                float s = vector.y / vector.x;
                res->x = (point->x + s * point->y) / (s * s + 1);
                res->y = s * res->x;
            }
        }
        void tt_GetLocations(TT_Vector &vector, CGlyphCoordinate *point1, CGlyphCoordinate *point2, CGlyphCoordinate *res1, CGlyphCoordinate *res2)
        {
            tt_GetLocation(vector, point1, res1);
            tt_GetLocation(vector, point2, res2);
        }

        Fixed tt_MeasureDistance(TT_Vector &vector, CGlyphCoordinate *point)
        {
            CGlyphCoordinate pointInProjVector;
            tt_GetLocation(vector, point, &pointInProjVector);
            float value = sqrtf(pointInProjVector.x * pointInProjVector.x + pointInProjVector.y * pointInProjVector.y);

            Fixed v = (Fixed)(64 * value + MEASURE_DISTANCE_ROUND);
            if (vector.x == 0)
            {
                if (vector.y >= 0)
                    return (pointInProjVector.y > 0) ? v : -v;
                else // y < 0
                    return (pointInProjVector.y > 0) ? -v : v;
            }
            else if (vector.y == 0)
            {
                if (vector.x >= 0)
                    return (pointInProjVector.x > 0) ? v : -v;
                else // x < 0
                    return (pointInProjVector.x > 0) ? -v : v;
            }
            else
            {
                float con = pointInProjVector.x * vector.x;
                if (con > 0) return v;
                else if (con < 0) return -v;
                else
                {
                    if (pointInProjVector.y * vector.y > 0) return v;
                    else return -v;
                }
            }
        }
        Fixed tt_MeasureDistance(TT_Vector &vector, CGlyphCoordinate *point1, CGlyphCoordinate *point2)
        {
            CGlyphCoordinate pointInProjVector1, pointInProjVector2;
            tt_GetLocations(vector, point1, point2, &pointInProjVector1, &pointInProjVector2);
            float value = sqrtf((pointInProjVector1.x - pointInProjVector2.x) * (pointInProjVector1.x - pointInProjVector2.x) + (pointInProjVector1.y - pointInProjVector2.y) * (pointInProjVector1.y - pointInProjVector2.y));

            float deltaX = pointInProjVector2.x - pointInProjVector1.x;
            float deltaY = pointInProjVector2.y - pointInProjVector1.y;

            Fixed v = (Fixed)(64 * value + MEASURE_DISTANCE_ROUND);
            if (vector.x == 0)
            {
                if (vector.y >= 0)
                    return (deltaY > 0) ? v : -v;
                else // y < 0
                    return (deltaY > 0) ? -v : v;
            }
            else if (vector.y == 0)
            {
                if (vector.x >= 0)
                    return (deltaX > 0) ? v : -v;
                else // x < 0
                    return (deltaX > 0) ? -v : v;
            }
            else
            {
                float con = deltaX * vector.x;
                if (con > 0) return v;
                else if (con < 0) return -v;
                else
                {
                    if (deltaY * vector.y > 0) return v;
                    else return -v;
                }
            }
        }

        CGlyphCoordinate tt_GetDeltaMovementAlongFreeVector(CGState *m_pGState, Fixed deltaDistance)
        {
            CGlyphCoordinate coord;
            coord.x = coord.y = 0;
            float fDeltaDistance = deltaDistance / 64.0f;

            // calculate the delta moved value along free vector
            if (FEqual(m_pGState->projVec.x, .0)) // y-axis
            {
                if (FEqual(m_pGState->freeVec.y, .0))
                    return coord;
                coord.y = fDeltaDistance;
                if (!FEqual(m_pGState->freeVec.x, .0))
                    coord.x = coord.y / (m_pGState->freeVec.y / m_pGState->freeVec.x);
            }
            else if (FEqual(m_pGState->projVec.y, .0)) // x-axis
            {
                if (FEqual(m_pGState->freeVec.x, .0))
                    return coord;
                coord.x = fDeltaDistance;
                if (!FEqual(m_pGState->freeVec.y, .0))
                    coord.y = coord.x * (m_pGState->freeVec.y / m_pGState->freeVec.x);
            }
            else
            {
                float p = m_pGState->projVec.y / m_pGState->projVec.x;
                if (m_pGState->projVec.x > 0)
                    coord.x = fDeltaDistance / sqrtf(p * p + 1);
                else
                    coord.x = -fDeltaDistance / sqrtf(p * p + 1);
                coord.y = p * coord.x;

                if (FEqual(m_pGState->freeVec.x, .0))
                    coord.x = 0;
                else if (FEqual(m_pGState->freeVec.y, .0))
                    coord.y = 0;
                else
                {
                    float f = m_pGState->freeVec.y / m_pGState->freeVec.x;
                    coord.x = (p * coord.y + coord.x) / (1 + p * f);
                    coord.y = f * coord.x;
                }
            }

            return coord;
        }
        void tt_MovePointAlongFreeVector(CGState *m_pGState, CGlyphCoordinate *point, Fixed deltaDistance)
        {
            CGlyphCoordinate delta = tt_GetDeltaMovementAlongFreeVector(m_pGState, deltaDistance);
            point->x += delta.x;
            point->y += delta.y;

        }


#define ROUND_Compensation_GREY           0 
#define ROUND_Compensation_BLACK          0 
#define ROUND_Compensation_WHITE          0 
        Long tt_RoundByGState(CGState *m_pGState, Long v, TT_DISTANCE_TYPE distanceType) // distanceType: 0 - grey, 1 - black, 2 -white, 3 - none
        {
            if(m_pGState->rndState == TTRS_OFF) return v;

            Long period = 64;      // Fixed26Dot6
            Long phase = 0;        // Fixed26Dot6
            Long threshold = 32;   // Fixed26Dot6
            switch(m_pGState->rndState)
            {
            case TTRS_TO_HALF_GRID:
                phase = 32;
                threshold = 32;
                break;
            case TTRS_TO_DOUBLE_GRID:
                period = 32;
                phase = 0;
                threshold = 16;
                break;
            case TTRS_UP_GRID:
                threshold = period - 1;
                break;
            case TTRS_DOWN_GRID:
                threshold = 0;
                break;
            case TTRS_SUPER:
            case TTRS_SUPER_45:
                period = (Long)(m_pGState->sRndPeriod * 64);
                phase = (Long)(m_pGState->sRndPhase * 64);
                threshold = (Long)(m_pGState->sRndThreshold * 64);
                break;
            case TTRS_TO_GRID:
            default:
                break;
            }

            // step 1: Compensate for the engine characteristics
            if(distanceType == TTDS_GREY) v += ROUND_Compensation_GREY;
            else if (distanceType == TTDS_BLACK) v += ROUND_Compensation_BLACK;
            else if (distanceType == TTDS_WHITE) v += ROUND_Compensation_WHITE;

            // step 2: subtract the phase from value
            Long old_v = v;
            v -= phase;

            // step 3: add threshold to value
            v += threshold;

            // step 4: truncate value to the next lowest periodic value
            v &= 0xFFFFFFC0;

            // step 5: add the phase back to value
                v += phase;

            if(old_v > 0)
            {
                // step 6: if rounding caused a positive number to become negative, set value to the positive round value closest to 0
                if(v < 0) v = period;
            }
            else if(old_v < 0)
            {
                // step 7: if rounding caused a negative number of become positive, set value to the negative round value closest to 0
                if(v > 0) v = -period;
            }

            return v;
        }

        int tt_SVTCA0(CGState *m_pGState) // set Freedom and projection vector to y-axis
        {
            m_pGState->freeVec.x = 0;
            m_pGState->freeVec.y = 1;
            m_pGState->projVec.x = 0;
            m_pGState->projVec.y = 1;
            m_pGState->dualVec.x = 0;
            m_pGState->dualVec.y = 1;
            return 0;
        }
        int tt_SVTCA1(CGState *m_pGState) // set Freedom and projection vector to x-axis
        {
            m_pGState->freeVec.x = 1;
            m_pGState->freeVec.y = 0;
            m_pGState->projVec.x = 1;
            m_pGState->projVec.y = 0;
            m_pGState->dualVec.x = 1;
            m_pGState->dualVec.y = 0;

            return 0;
        }
        int tt_SPVTCA0(CGState *m_pGState)
        {
            m_pGState->projVec.x = 0;
            m_pGState->projVec.y = 1;
            m_pGState->dualVec.x = 0;
            m_pGState->dualVec.y = 1;
            return 0;
        }
        int tt_SPVTCA1(CGState *m_pGState)
        {
            m_pGState->projVec.x = 1;
            m_pGState->projVec.y = 0;
            m_pGState->dualVec.x = 1;
            m_pGState->dualVec.y = 0;
            return 0;
        }
        int tt_SFVTCA0(CGState *m_pGState)
        {
            m_pGState->freeVec.x = 0;
            m_pGState->freeVec.y = 1;
            return 0;
        }
        int tt_SFVTCA1(CGState *m_pGState)
        {
            m_pGState->freeVec.x = 1;
            m_pGState->freeVec.y = 0;
            return 0;
        }
        int tt_SPVTL0(CGState *m_pGState)
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate pt1, pt2;
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  m_pGState->pOutlines->pCoordinates[ptIndex1];
            else
                pt1 = m_pGState->m_pTwilightZone[ptIndex1];

            if(m_pGState->zp2 == TTZP_NORMAL)
                pt2 =  m_pGState->pOutlines->pCoordinates[ptIndex2];
            else
                pt2 = m_pGState->m_pTwilightZone[ptIndex2];

            double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
            m_pGState->projVec.y = (float)sin(angle);
            m_pGState->projVec.x = (float)cos(angle);

            return -1;
        }
        int tt_SPVTL1(CGState *m_pGState)
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate pt1, pt2;
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  m_pGState->pOutlines->pCoordinates[ptIndex1];
            else
                pt1 = m_pGState->m_pTwilightZone[ptIndex1];

            if(m_pGState->zp2 == TTZP_NORMAL)
                pt2 =  m_pGState->pOutlines->pCoordinates[ptIndex2];
            else
                pt2 = m_pGState->m_pTwilightZone[ptIndex2];

            double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x) + PI / 2;
            m_pGState->projVec.y = (float)sin(angle);
            m_pGState->projVec.x = (float)cos(angle);

            return -1;
        }
        int tt_SFVTL0(CGState *m_pGState)
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate pt1, pt2;
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  m_pGState->pOutlines->pCoordinates[ptIndex1];
            else
                pt1 = m_pGState->m_pTwilightZone[ptIndex1];

            if(m_pGState->zp2 == TTZP_NORMAL)
                pt2 =  m_pGState->pOutlines->pCoordinates[ptIndex2];
            else
                pt2 = m_pGState->m_pTwilightZone[ptIndex2];

            double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
            m_pGState->freeVec.y = (float)sin(angle);
            m_pGState->freeVec.x = (float)cos(angle);

            return -1;
        }
        int tt_SFVTL1(CGState *m_pGState)
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate pt1, pt2;
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  m_pGState->pOutlines->pCoordinates[ptIndex1];
            else
                pt1 = m_pGState->m_pTwilightZone[ptIndex1];

            if(m_pGState->zp2 == TTZP_NORMAL)
                pt2 =  m_pGState->pOutlines->pCoordinates[ptIndex2];
            else
                pt2 = m_pGState->m_pTwilightZone[ptIndex2];

            double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x) + PI / 2;
            m_pGState->freeVec.y = (float)sin(angle);
            m_pGState->freeVec.x = (float)cos(angle);

            return -1;
        }
        int tt_SPVFS(CGState *m_pGState)
        {
            F2Dot14 y = (F2Dot14)m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            F2Dot14 x = (F2Dot14)m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->projVec.y = _F2DOT14ToFloat(y);
            m_pGState->projVec.x = _F2DOT14ToFloat(x);

            return -1;
        }
        int tt_SFVFS(CGState *m_pGState)
        {
            F2Dot14 y = (F2Dot14)m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            F2Dot14 x = (F2Dot14)m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->freeVec.y = _F2DOT14ToFloat(y);
            m_pGState->freeVec.x = _F2DOT14ToFloat(x);

            return -1;
        }
        int tt_SFVTPV(CGState *m_pGState)
        {
            m_pGState->freeVec.y = m_pGState->projVec.y;
            m_pGState->freeVec.x = m_pGState->projVec.x;

            return -1;
        }
        int tt_GPV(CGState *m_pGState)
        {
            double angle = atan2(m_pGState->projVec.y, m_pGState->projVec.x);

            float x = (float)cos(angle);
            float y = (float)sin(angle);

            m_pGState->m_stack.push(_FloatToF2DOT14(x));
            m_pGState->m_stack.push(_FloatToF2DOT14(y));

            return -1;
        }
        int tt_GFV(CGState *m_pGState)
        {
            double angle = atan2(m_pGState->freeVec.y, m_pGState->freeVec.x);

            float x = (float)cos(angle);
            float y = (float)sin(angle);

            m_pGState->m_stack.push(_FloatToF2DOT14(x));
            m_pGState->m_stack.push(_FloatToF2DOT14(y));

            return -1;
        }
        int tt_ISECT(CGState *m_pGState)
        {
            ULong aIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong aIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong bIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong bIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate a1, a2, b1, b2, *pt;
            if(m_pGState->zp0 == TTZP_NORMAL)
            {
                a1 =  m_pGState->pOutlines->pCoordinates[aIndex1];
                a2 =  m_pGState->pOutlines->pCoordinates[aIndex2];
            }
            else
            {
                a1 = m_pGState->m_pTwilightZone[aIndex1];
                a2 = m_pGState->m_pTwilightZone[aIndex2];
            }
            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                b1 =  m_pGState->pOutlines->pCoordinates[bIndex1];
                b2 =  m_pGState->pOutlines->pCoordinates[bIndex2];
            }
            else
            {
                b1 = m_pGState->m_pTwilightZone[bIndex1];
                b2 = m_pGState->m_pTwilightZone[bIndex2];
            }

            if(m_pGState->zp2 == TTZP_NORMAL)
                pt =  &m_pGState->pOutlines->pCoordinates[ptIndex];
            else
                pt = &m_pGState->m_pTwilightZone[ptIndex];

            double aAngle = atan2(a2.y - a1.y, a2.x - a1.x);
            double bAngle = atan2(b2.y - b1.y, b2.x - b1.x);
            if(FEqual(aAngle, bAngle))
            {
                pt->x = ((a1.x + a2.x) / 2 + (b1.x + b2.x) / 2) / 2;
                pt->y = ((a1.y + a2.y) / 2 + (b1.y + b2.y) / 2) / 2;
            }
            else
            {
                double aA = tan(aAngle);
                double bA = tan(bAngle);
                double aB = a1.y - aA * a1.x;
                double bB = b1.y - bA * b1.x;
                pt->x = (float)((bB - aB) / (aA - bA));
                pt->y = (float)((aA * bB - bA * aB) / (aA - bA));
            }

            return -1;
        }
        int tt_SRP0(CGState *m_pGState)
        {
            m_pGState->rp0 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_SRP1(CGState *m_pGState)
        {
            m_pGState->rp1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_SRP2(CGState *m_pGState)
        {
            m_pGState->rp2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_SZP0(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(n == 0) m_pGState->zp0 = TTZP_TWILIGHT;
            else if( n == 1) m_pGState->zp0 = TTZP_NORMAL;
            else return -1;

            return -1;
        }
        int tt_SZP1(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(n == 0) m_pGState->zp1 = TTZP_TWILIGHT;
            else if( n == 1) m_pGState->zp1 = TTZP_NORMAL;
            else return -1;

            return -1;
        }
        int tt_SZP2(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(n == 0) m_pGState->zp2 = TTZP_TWILIGHT;
            else if( n == 1) m_pGState->zp2 = TTZP_NORMAL;
            else return -1;

            return -1;
        }
        int tt_SZPS(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(n == 0)
            {
                m_pGState->zp0 = TTZP_TWILIGHT;
                m_pGState->zp1 = TTZP_TWILIGHT;
                m_pGState->zp2 = TTZP_TWILIGHT;
            }
            else if( n == 1)
            {
                m_pGState->zp0 = TTZP_NORMAL;
                m_pGState->zp1 = TTZP_NORMAL;
                m_pGState->zp2 = TTZP_NORMAL;
            }
            else return -1;

            return -1;
        }
        int tt_SLOOP(CGState *m_pGState)
        {
            m_pGState->loop = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            return 0;
        }
        int tt_RTG(CGState *m_pGState)
        {
            m_pGState->rndState = TTRS_TO_GRID;
            return 0;
        }
        int tt_RTHG(CGState *m_pGState)
        {
            m_pGState->rndState = TTRS_TO_HALF_GRID;
            return 0;
        }
        int tt_SMD(CGState *m_pGState)
        {
            Long e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->minDist = e / 64.0f;

            return 0;
        }
        int tt_ELSE(CGState *m_pGState)
        {
            // jump to endif
            int level = 0;
            bool found = false;
            while(!found && m_pGState->nOffset < (Long)m_pGState->nInstr)
            {
                if(level == 0 && m_pGState->pInstr[m_pGState->nOffset] == TT_EIF)
                {
                    // found it
                    found = true;
                }
                else if(m_pGState->pInstr[m_pGState->nOffset] == TT_IF)
                {
                    level++;
                }
                else if(level > 0 && m_pGState->pInstr[m_pGState->nOffset] == TT_EIF)
                {
                    level--;
                }
                m_pGState->nOffset += tt_GetNextCmdBytes(m_pGState);
            }

            //m_pGState->nOffset++;

            return 0;
        }
        int tt_JMPR(CGState *m_pGState)
        {
            Long offset = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->nOffset += (offset - 1);

            return -1;
        }
        int tt_SCVTCI(CGState *m_pGState)
        {
            Long e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->cvtCutin = _FixedToFloat(e);

            return 0;
        }
        int tt_SSWCI(CGState *m_pGState)
        {
            Long e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->singWidCut = _FixedToFloat(e);

            return -1;
        }
        int tt_SSW(CGState *m_pGState)
        {
            Long e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->singWidVal = (float)e; //_FixedToFloat(e); // in FUnit??

            return -1;
        }
        int tt_DUP(CGState *m_pGState)
        {
            m_pGState->m_stack.push(m_pGState->m_stack.top());

            return 0;
        }
        int tt_POP(CGState *m_pGState)
        {
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_CLEAR(CGState *m_pGState)
        {
            m_pGState->m_stack.reset();

            return 0;
        }
        int tt_SWAP(CGState *m_pGState)
        {
            ULong e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->m_stack.push(e1);
            m_pGState->m_stack.push(e2);

            return 0;
        }
        int tt_DEPTH(CGState *m_pGState)
        {
            Long size = (Long) m_pGState->m_stack.size();
             m_pGState->m_stack.push(size);

            return 0;
        }
        int tt_CINDEX(CGState *m_pGState)
        {
            ULong index = m_pGState->m_stack.top();
            ULong e = m_pGState->m_stack.get(index);
            m_pGState->m_stack.pop();
            m_pGState->m_stack.push(e);

            return 0;
        }
        int tt_MINDEX(CGState *m_pGState)
        {
            ULong e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->m_stack.moveToTop(e1);

            return 0;
        }
        int tt_ALIGNPTS(CGState *m_pGState)
        {
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt1, *pt2;
            if(m_pGState->zp0 == TTZP_NORMAL)
                pt2 =  &m_pGState->pOutlines->pCoordinates[ptIndex2];
            else
                pt2 = &m_pGState->m_pTwilightZone[ptIndex2];
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  &m_pGState->pOutlines->pCoordinates[ptIndex1];
            else
                pt1 = &m_pGState->m_pTwilightZone[ptIndex1];

            /////////////////////////////////////////////////////////////////
            Fixed distance = tt_MeasureDistance(m_pGState->projVec, pt1, pt2);
            tt_MovePointAlongFreeVector(m_pGState, pt1, distance / 2);
            tt_MovePointAlongFreeVector(m_pGState, pt2, -distance / 2);

            tt_MarkTouched(m_pGState, pt1);
            tt_MarkTouched(m_pGState, pt2);

            return -1;
        }
        int tt_UTP(CGState *m_pGState)
        {
            ULong ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            CGlyphCoordinate *pt;
            if(m_pGState->zp0 == TTZP_NORMAL)
                pt =  &m_pGState->pOutlines->pCoordinates[ptIndex];
            else
                pt = &m_pGState->m_pTwilightZone[ptIndex];

            if(!FEqual(m_pGState->freeVec.x, 0))
                pt->bOnCurve &= (~PF_TOUCHED_X);
            if(!FEqual(m_pGState->freeVec.y, 0))
                pt->bOnCurve &= (~PF_TOUCHED_Y);

            return -1;
        }
        int tt_CallFunction(CGState *m_pGState, ULong f, Long times)
        {
            std::map<ULong, TT_NestedFunc*>::iterator it = m_pGState->m_pFuncDefs->find(f);
            if(it != m_pGState->m_pFuncDefs->end())
            {
                // store function environment
                TT_NestedFunc *pF = new TT_NestedFunc(m_pGState->nOffset, m_pGState->pInstr, m_pGState->nInstr, it->second->offset, times, false);

                // move the pointer to function
                m_pGState->pInstr = it->second->pInstr;
                m_pGState->nInstr = it->second->nInstr;
                m_pGState->nOffset = it->second->offset;

                m_pGState->nestedFuncs.push_back(pF);
            }
            else
                return -1;

            return 0;
        }
        int tt_LOOPCALL(CGState *m_pGState)
        {
            ULong f = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            
            return tt_CallFunction(m_pGState, f, n);
        }
        int tt_CALL(CGState *m_pGState)
        {
            ULong f = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return tt_CallFunction(m_pGState, f, 1);
        }
        int tt_FDEF(CGState *m_pGState) // parsing function definitions
        {
            ULong f = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(m_pGState->m_pFuncDefs)
            {
                TT_NestedFunc *pF= new TT_NestedFunc(m_pGState->nOffset, m_pGState->pInstr, m_pGState->nInstr, 0, 1, false);
                m_pGState->m_pFuncDefs->insert(std::pair<ULong, TT_NestedFunc*>(f, pF));
            }

            while(m_pGState->pInstr[m_pGState->nOffset++] != TT_ENDF)
            {
                if(m_pGState->nOffset >= (Long)m_pGState->nInstr)
                    return -1;
            }

            return 0;
        }
        int tt_ENDF(CGState *m_pGState) // function executes completed
        {
            if(m_pGState->nestedFuncs.size() == 0) return -1;

            TT_NestedFunc *pF = m_pGState->nestedFuncs[m_pGState->nestedFuncs.size() - 1];
            if(pF->bIsLoopCall && --pF->loopCallTimes > 0)
            {
                // call this funtion again
                m_pGState->nOffset = pF->loopCallOffset;
            }
            else
            {
                // restore function environment
                m_pGState->nInstr = pF->nInstr;
                m_pGState->pInstr = pF->pInstr;
                m_pGState->nOffset = pF->offset;

                delete pF;
                m_pGState->nestedFuncs.pop_back();
            }

            return 0;
        }
        int tt_MDAP0(CGState *m_pGState) // Move Direct Absolute Points : not round the value
        {
            m_pGState->rp0 = m_pGState->rp1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            CGlyphCoordinate *pCoords = NULL;
            if(m_pGState->zp0 == TTZP_NORMAL) // normal zone : points in outlines
            {
                CGlyphOutlines *pOutlines = m_pGState->pOutlines;
                if(!pOutlines || !pOutlines->pCoordinates || m_pGState->rp0 > pOutlines->numPoints || m_pGState->rp0 < 0)
                    return -1;
                pCoords = &pOutlines->pCoordinates[m_pGState->rp0];
            }
            else // Twilight zone : points in twilight zone
            {
                if(m_pGState->m_nTwilightPoints <= 0 || !m_pGState->m_pTwilightZone)
                    return -1;
                pCoords = &m_pGState->m_pTwilightZone[m_pGState->rp0];
            }

            pCoords->bOnCurve |= PF_TOUCHED_X | PF_TOUCHED_Y;

            return 0;
        }
        int tt_MDAP1(CGState *m_pGState) // Move Direct Absolute Points : round the value
        {
            m_pGState->rp0 = m_pGState->rp1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            CGlyphCoordinate *pCoords = NULL;
            if(m_pGState->zp0 == TTZP_NORMAL) // normal zone : points in outlines
            {
                CGlyphOutlines *pOutlines = m_pGState->pOutlines;
                if(!pOutlines || !pOutlines->pCoordinates || m_pGState->rp0 > pOutlines->numPoints || m_pGState->rp0 < 0)
                    return -1;
                pCoords = &pOutlines->pCoordinates[m_pGState->rp0];
            }
            else // Twilight zone : points in twilight zone
            {
                if(m_pGState->m_nTwilightPoints <= 0 || !m_pGState->m_pTwilightZone)
                    return -1;
                pCoords = &m_pGState->m_pTwilightZone[m_pGState->rp0];
            }

            //// round
            Fixed distance = tt_MeasureDistance(m_pGState->projVec, pCoords);
            Fixed delta = tt_RoundByGState(m_pGState, distance, TTDS_NONE) - distance;
            // move
            tt_MovePointAlongFreeVector(m_pGState, pCoords, delta);
            tt_MarkTouched(m_pGState, pCoords);

            return 0;
        }
        void tt_IUP(CGState *m_pGState, int first, int second, int start, int end, bool isX)
        {
            //if(second == first) return;

            int totalPoints;
            if(second > first)
                totalPoints = second - first + 1;
            else
                totalPoints = (end - first + 1) + (second - start + 1);

            totalPoints -= 2;

            CGlyphCoordinate *pOrigCoords = m_pGState->pOrigOutlines->pCoordinates;
            CGlyphCoordinate *pCoords = m_pGState->pOutlines->pCoordinates;

            int j = first + 1;
            int loopBack = start - end - 1;
            if(isX)
            {
                for(int i = 0; i< totalPoints; i++, j++)
                {
                    if(j > end) j += loopBack;

                    float diff1 = pOrigCoords[j].x - pOrigCoords[first].x;
                    float diff2 = pOrigCoords[j].x - pOrigCoords[second].x;
                    if(diff2 == 0)
                    {
                        pCoords[j].x = pCoords[second].x;
                    }
                    else
                    {
                        float ratio = diff1 / diff2;
                        if(diff1 == 0)
                        {
                            pCoords[j].x = pCoords[first].x;
                        }
                        else if(ratio < 0) // interpolate linearly
                        {
                            pCoords[j].x = (pCoords[second].x * ratio - pCoords[first].x) / (ratio - 1);
                        }
                        else // shift by the amount the nearest touched point
                        {
                            if(abs(diff1) > abs(diff2))
                                pCoords[j].x += (pCoords[second].x - pOrigCoords[second].x);
                            else
                                pCoords[j].x += (pCoords[first].x - pOrigCoords[first].x);
                        }
                    }
                }
            }
            else
            {
                for(int i = 0; i< totalPoints; i++, j++)
                {
                    if(j > end) j += loopBack;

                    float diff1 = pOrigCoords[j].y - pOrigCoords[first].y;
                    float diff2 = pOrigCoords[j].y - pOrigCoords[second].y;
                    if(diff2 == 0)
                    {
                        pCoords[j].y = pCoords[second].y;
                    }
                    else
                    {
                        float ratio = diff1 / diff2;
                        if(diff1 == 0)
                        {
                            pCoords[j].y = pCoords[first].y;
                        }
                        else if(ratio < 0) // interpolate linearly
                        {
                            pCoords[j].y = (pCoords[second].y * ratio - pCoords[first].y) / (ratio - 1);
                        }
                        else // shift by the amount the nearest touched point
                        {
                            if(abs(diff1) > abs(diff2))
                                pCoords[j].y += (pCoords[second].y - pOrigCoords[second].y);
                            else
                                pCoords[j].y += (pCoords[first].y - pOrigCoords[first].y);
                        }
                    }
                }
            }
        }
        int tt_IUP0(CGState *m_pGState) // y-axis
        {
            if(m_pGState->zp2 != TTZP_NORMAL) return -1;

            int start = 0;
            for(int i = 0; i < m_pGState->pOutlines->numContours; i++)
            {
                int firstTouchedPoint = -1;
                int touchedPoint1 = -1;
                int touchedPoint2 = -1;
                bool looped = false;
                // find the first touched point
                for(unsigned int j = start; j <= m_pGState->pOutlines->pEndPtsOfContours[i]; j++)
                {
                    if((m_pGState->pOutlines->pCoordinates[j].bOnCurve & PF_TOUCHED_Y) == PF_TOUCHED_Y)
                    {
                        if(firstTouchedPoint == -1) firstTouchedPoint = j;
                        if(touchedPoint1 == -1) touchedPoint1 = j;
                        else if(touchedPoint2 == -1) touchedPoint2 = j;
                    }
                    if(touchedPoint1 != -1 && touchedPoint2 != -1)
                    {
                        // interpolate untouched points
                        tt_IUP(m_pGState, touchedPoint1, touchedPoint2, start, m_pGState->pOutlines->pEndPtsOfContours[i], false); 

                        if (firstTouchedPoint == touchedPoint2)
                            break;

                        // next pair of touched points
                        touchedPoint1 = touchedPoint2;
                        touchedPoint2 = -1;
                    }
                   
                    if (touchedPoint1 != -1 && touchedPoint2 == -1 
                        &&j == m_pGState->pOutlines->pEndPtsOfContours[i]
                        && (touchedPoint1 == firstTouchedPoint || firstTouchedPoint != touchedPoint1)) // reach last point and not finished yet
                    {
                        if(looped) break;
                        looped = true;
                        j = start - 1;
                    }
                }
                start = m_pGState->pOutlines->pEndPtsOfContours[i] + 1;
            }

            return 0;
        }
        int tt_IUP1(CGState *m_pGState) // x-axis
        {
            if(m_pGState->zp2 != TTZP_NORMAL) return -1;

            int start = 0;
            for(int i = 0; i < m_pGState->pOutlines->numContours; i++)
            {
                int firstTouchedPoint = -1;
                int touchedPoint1 = -1;
                int touchedPoint2 = -1;
                bool looped = false;
                // find the first touched point
                for(unsigned int j = start; j <= m_pGState->pOutlines->pEndPtsOfContours[i]; j++)
                {
                    if((m_pGState->pOutlines->pCoordinates[j].bOnCurve & PF_TOUCHED_X) == PF_TOUCHED_X)
                    {
                        if(firstTouchedPoint == -1) firstTouchedPoint = j;
                        if(touchedPoint1 == -1) touchedPoint1 = j;
                        else if(touchedPoint2 == -1) touchedPoint2 = j;
                    }
                    if(touchedPoint1 != -1 && touchedPoint2 != -1)
                    {
                        // interpolate untouched points
                        tt_IUP(m_pGState, touchedPoint1, touchedPoint2, start, m_pGState->pOutlines->pEndPtsOfContours[i], true); 

                        if (firstTouchedPoint == touchedPoint2)
                            break;

                        // next pair of touched points
                        touchedPoint1 = touchedPoint2;
                        touchedPoint2 = -1;
                    }

                    if (touchedPoint1 != -1 && touchedPoint2 == -1
                        && j == m_pGState->pOutlines->pEndPtsOfContours[i]
                        && (touchedPoint1 == firstTouchedPoint || firstTouchedPoint != touchedPoint1)) // reach last point and not finished yet
                    {
                        if(looped) break;
                        looped = true;
                        j = start - 1;
                    }
                }
                start = m_pGState->pOutlines->pEndPtsOfContours[i] + 1;
            }

            return 0;
        }
        int tt_SHP(CGState *m_pGState, CGlyphCoordinate &o_rp, CGlyphCoordinate &rp)
        {
            CGlyphCoordinate *point = NULL;

            Fixed deltaRef = tt_MeasureDistance(m_pGState->projVec, &o_rp, &rp);

            for(int i = 0; i<m_pGState->loop; i++)
            {
                Long ptIndex = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(m_pGState->zp2 == TTZP_NORMAL)
                    point = &m_pGState->pOutlines->pCoordinates[ptIndex];
                else
                    point = &m_pGState->m_pTwilightZone[ptIndex];

                Fixed deltaPoint = tt_MeasureDistance(m_pGState->projVec, &m_pGState->pOrigOutlines->pCoordinates[ptIndex], point);
                if(deltaPoint != deltaRef)
                {
                    tt_MovePointAlongFreeVector(m_pGState, point, deltaRef - deltaPoint);
                }
                tt_MarkTouched(m_pGState, point);
            }
            m_pGState->loop = 1;

            return 0;
        }
        int tt_SHP0(CGState *m_pGState)
        {
            CGlyphCoordinate rp2, o_rp2;

            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_rp2 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp2];
                rp2 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp2];
            }
            else
            {
                o_rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
                rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
            }

            return tt_SHP(m_pGState, o_rp2, rp2);
        }
        int tt_SHP1(CGState *m_pGState)
        {
            CGlyphCoordinate rp1, o_rp1;

            if(m_pGState->zp0 == TTZP_NORMAL)
            {
                o_rp1 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp1];
                rp1 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp1];
            }
            else
            {
                o_rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
                rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
            }

            return tt_SHP(m_pGState, o_rp1, rp1);
        }
        int tt_SHC(CGState *m_pGState, CGlyphCoordinate &o_rp, CGlyphCoordinate &rp)
        {
            Long contour = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *point = NULL;

            Fixed deltaRef = tt_MeasureDistance(m_pGState->projVec, &o_rp, &rp);

            unsigned int start = 0;
            if(contour > 0)
                start = m_pGState->pOutlines->pEndPtsOfContours[contour-1] + 1;

            for(unsigned int i = start; i <= m_pGState->pOutlines->pEndPtsOfContours[contour]; i++)
            {
                point = &m_pGState->pOutlines->pCoordinates[i];

                Fixed deltaPoint = tt_MeasureDistance(m_pGState->projVec, &m_pGState->pOrigOutlines->pCoordinates[i], point);
                if(deltaPoint != deltaRef)
                {
                    tt_MovePointAlongFreeVector(m_pGState, point, deltaRef - deltaPoint);
                }
                tt_MarkTouched(m_pGState, point);
            }

            return -1;
        }
        int tt_SHC0(CGState *m_pGState)
        {
            CGlyphCoordinate rp2, o_rp2;

            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_rp2 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp2];
                rp2 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp2];
            }
            else
            {
                o_rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
                rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
            }

            return tt_SHC(m_pGState, o_rp2, rp2);
        }
        int tt_SHC1(CGState *m_pGState)
        {
            CGlyphCoordinate rp1, o_rp1;

            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_rp1 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp1];
                rp1 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp1];
            }
            else
            {
                o_rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
                rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
            }

            return tt_SHC(m_pGState, o_rp1, rp1);
        }
        int tt_SHZ(CGState *m_pGState, CGlyphCoordinate &o_rp, CGlyphCoordinate &rp)
        {
            Long zone = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(zone != TTZP_NORMAL && zone != TTZP_TWILIGHT) return -1;

            CGlyphCoordinate *point = NULL;

            Fixed deltaRef = tt_MeasureDistance(m_pGState->projVec, &o_rp, &rp);

            for(int i = 0; i <= m_pGState->pOutlines->numPoints; i++)
            {
                point = &m_pGState->pOutlines->pCoordinates[i];

                Fixed deltaPoint = tt_MeasureDistance(m_pGState->projVec, &m_pGState->pOrigOutlines->pCoordinates[i], point);
                if(deltaPoint != deltaRef)
                {
                    tt_MovePointAlongFreeVector(m_pGState, point, deltaRef - deltaPoint);
                }
                tt_MarkTouched(m_pGState, point);
            }

            return -1;
        }
        int tt_SHZ0(CGState *m_pGState)
        {
            CGlyphCoordinate rp2, o_rp2;

            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_rp2 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp2];
                rp2 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp2];
            }
            else
            {
                o_rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
                rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
            }

            return tt_SHZ(m_pGState, o_rp2, rp2);
        }
        int tt_SHZ1(CGState *m_pGState)
        {
            CGlyphCoordinate rp1, o_rp1;

            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_rp1 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp1];
                rp1 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp1];
            }
            else
            {
                o_rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
                rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
            }

            return tt_SHZ(m_pGState, o_rp1, rp1);
        }
        int tt_SHPIX(CGState *m_pGState)
        {
            Long pixelAmount = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *point = NULL;
            for(int i = 0; i<m_pGState->loop; i++)
            {
                Long ptIndex = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(m_pGState->zp2 == TTZP_NORMAL)
                    point = &m_pGState->pOutlines->pCoordinates[ptIndex];
                else
                    point = &m_pGState->m_pTwilightZone[ptIndex];

                if(pixelAmount != 0)
                {
                    double freeVecAngle = atan2(m_pGState->freeVec.y, m_pGState->freeVec.x);
                    double deltaX = pixelAmount * cos(freeVecAngle) / 64;
                    double deltaY = pixelAmount * sin(freeVecAngle) / 64;
                    point->x += (float)deltaX;
                    point->y += (float)deltaY;

                    //tt_MovePointAlongFreeVector(m_pGState, point, pixelAmount);
                }
                tt_MarkTouched(m_pGState, point);
                //point->bOnCurve |= PF_TOUCHED;
            }
            m_pGState->loop = 1;

            return 0;
        }
        int tt_IP(CGState *m_pGState)
        {
            CGlyphCoordinate rp1, rp2, o_rp1, o_rp2;
            CGlyphCoordinate *point = NULL, *origPoint = NULL;

            if(m_pGState->zp0 == TTZP_NORMAL)
            {
                o_rp1 =  m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp1];
                rp1 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp1];
            }
            else
            {
                o_rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
                rp1 = m_pGState->m_pTwilightZone[m_pGState->rp1];
            }
            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_rp2 = m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp2];
                rp2 = m_pGState->pOutlines->pCoordinates[m_pGState->rp2];
            }
            else
            {
                o_rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
                rp2 = m_pGState->m_pTwilightZone[m_pGState->rp2];
            }

            ////////////////////////////////////////////////////
            for (int i = 0; i<m_pGState->loop; i++)
            {
                int slot = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if (m_pGState->zp2 == TTZP_NORMAL)
                {
                    origPoint = &m_pGState->pOrigOutlines->pCoordinates[slot];
                    point = &m_pGState->pOutlines->pCoordinates[slot];
                }
                else
                {
                    origPoint = &m_pGState->m_pTwilightZone[slot];
                    point = &m_pGState->m_pTwilightZone[slot];
                }

                //CGlyphCoordinate oldCoord = *point;

                Fixed d1 = tt_MeasureDistance(m_pGState->dualVec, &o_rp1, origPoint);
                Fixed d2 = tt_MeasureDistance(m_pGState->dualVec, origPoint, &o_rp2);
                Fixed d0p = tt_MeasureDistance(m_pGState->projVec, &rp1, &rp2);
                Fixed d1p = (Fixed)(1.0f * (d0p * d1) / (d1 + d2)); // expected distance from rp1 to point
                Fixed cur_d1p = tt_MeasureDistance(m_pGState->projVec, &rp1, point); // current distance from rp1 to point

                tt_MovePointAlongFreeVector(m_pGState, point, d1p - cur_d1p);

                tt_MarkTouched(m_pGState, point);
            }
            m_pGState->loop = 1; 


            ////////////////////////////////////////////////////
            /*CGlyphCoordinate oloc1, oloc2, nloc1, nloc2;
            
            double projectAngle = atan2(m_pGState->projVec.y, m_pGState->projVec.x);
            double projectA = tan(projectAngle);
            double perpendicularA = tan(projectAngle + PI/2);
            double freeVecAngle = atan2(m_pGState->freeVec.y, m_pGState->freeVec.x);
            double freeVecA = tan(freeVecAngle);

            tt_GetLocation(&o_rp1, &oloc1, projectAngle);
            tt_GetLocation(&o_rp2, &oloc2, projectAngle);

            if(oloc1 == oloc2) return 0;

            tt_GetLocation(&rp1, &nloc1, projectAngle);
            tt_GetLocation(&rp2, &nloc2, projectAngle);

            for(int i = 0; i<m_pGState->loop; i++)
            {
                int slot = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(m_pGState->zp2 == TTZP_NORMAL)
                {
                    origPoint = &m_pGState->pOrigOutlines->pCoordinates[slot];
                    point = &m_pGState->pOutlines->pCoordinates[slot];
                }
                else
                {
                    origPoint = &m_pGState->m_pTwilightZone[slot];
                    point = &m_pGState->m_pTwilightZone[slot];
                }

                //CGlyphCoordinate oldCoord = *point;

                Fixed d1 = tt_MeasureDistance(m_pGState->dualVec, origPoint, &o_rp1);
                Fixed d2 = tt_MeasureDistance(m_pGState->dualVec, origPoint, &o_rp2);
                float ratio = 1.0f * abs(d1) / (abs(d2) + abs(d1));

                CRealPoint pt = Interpolate(CRealPoint(nloc1.x, nloc1.y), CRealPoint(nloc2.x, nloc2.y), ratio);

                if(freeVecA != perpendicularA)
                {
                    double perpendicularB = pt.y - perpendicularA * pt.x;
                    double freeVecB = point->y - freeVecA * point->x;
                    point->x = (float)((perpendicularB - freeVecB) / (freeVecA - perpendicularA));
                    point->y = (float)((freeVecA * perpendicularB - perpendicularA * freeVecB ) / (freeVecA - perpendicularA));
                }

                tt_MarkTouched(m_pGState, point);
                //point->bOnCurve |= PF_TOUCHED;
            }
            m_pGState->loop = 1;*/

            return 0;
        }
        int tt_MSIRP(CGState *m_pGState, bool bSetRp0)
        {
            Long new_distance = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate rp0;
            CGlyphCoordinate *point = NULL;

            if(m_pGState->zp0 == TTZP_NORMAL)
                rp0 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp0];
            else
                rp0 = m_pGState->m_pTwilightZone[m_pGState->rp0];
            if(m_pGState->zp1 == TTZP_NORMAL)
                point =  &m_pGState->pOutlines->pCoordinates[ptIndex];
            else
                point = &m_pGState->m_pTwilightZone[ptIndex];

            // get distance between point and rp0
            Fixed distance = tt_MeasureDistance(m_pGState->projVec, &rp0, point);

            // move the point along the freedom vector
            if(distance != new_distance)
            {
                tt_MovePointAlongFreeVector(m_pGState, point, new_distance - distance);
            }
            tt_MarkTouched(m_pGState, point);

            // set state variables
            if(bSetRp0) m_pGState->rp0 = ptIndex;

            return 0;
        }
        int tt_MSIRP0(CGState *m_pGState)
        {
            return tt_MSIRP(m_pGState, false);
        }
        int tt_MSIRP1(CGState *m_pGState)
        {
            return tt_MSIRP(m_pGState, true);
        }
        int tt_ALIGNRP(CGState *m_pGState)
        {
            ULong ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt, *rp0;
            if(m_pGState->zp0 == TTZP_NORMAL)
                rp0 = &m_pGState->pOutlines->pCoordinates[m_pGState->rp0];
            else
                rp0 = &m_pGState->m_pTwilightZone[m_pGState->rp0];
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt = &m_pGState->pOutlines->pCoordinates[ptIndex];
            else
                pt = &m_pGState->m_pTwilightZone[ptIndex];
            
            Fixed distance = tt_MeasureDistance(m_pGState->projVec, pt, rp0);
            tt_MovePointAlongFreeVector(m_pGState, pt, distance);

            tt_MarkTouched(m_pGState, pt);

            return -1;
        }
        int tt_RTDG(CGState *m_pGState)
        {
            m_pGState->rndState = TTRS_TO_DOUBLE_GRID;

            return 0;
        }
        int tt_MIAP0(CGState *m_pGState) // with no round
        {
            Long cvtIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *point = NULL, *o_point = NULL;

            if (m_pGState->zp0 == TTZP_NORMAL)
            {
                point = &m_pGState->pOutlines->pCoordinates[ptIndex];
                o_point = &m_pGState->pOrigOutlines->pCoordinates[ptIndex];
            }
            else
            {
                point = &m_pGState->m_pTwilightZone[ptIndex];
                o_point = point;
            }

            // get distance between point and origin
            Fixed distance = tt_MeasureDistance(m_pGState->projVec, o_point);

            // move the point along the freedom vector so that the distance from p to the current position of rp0 
            // is equal to the distance stated in the referenced CVT entry
            if(distance != m_pGState->pCvt[cvtIndex])
            {
                //CGlyphCoordinate oldCoord = *point;
                tt_MovePointAlongFreeVector(m_pGState, point, m_pGState->pCvt[cvtIndex] - distance);
            }
            tt_MarkTouched(m_pGState, point);
            //point->bOnCurve |= PF_TOUCHED;

            // set state variables
            m_pGState->rp0 = m_pGState->rp1 = ptIndex;

            return 0;
        }
        int tt_MIAP1(CGState *m_pGState) // with round
        {
            Long cvtIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *point = NULL, *o_point = NULL;

            if (m_pGState->zp0 == TTZP_NORMAL)
            {
                point = &m_pGState->pOutlines->pCoordinates[ptIndex];
                o_point = &m_pGState->pOrigOutlines->pCoordinates[ptIndex];
            }
            else
            {
                point = &m_pGState->m_pTwilightZone[ptIndex];
                o_point = point;
            }

            // get distance between point and origin
            Fixed distance = tt_MeasureDistance(m_pGState->projVec, o_point);

            // cut-in test
            Fixed new_distance = distance;
            Long cvtValue = m_pGState->pCvt[cvtIndex];
            if(m_pGState->autoFlip && (new_distance * m_pGState->pCvt[cvtIndex]) < 0)
                cvtValue = -cvtValue;
            if( abs(new_distance - cvtValue) <= (Long)(m_pGState->cvtCutin * 64)) // cut-in test sucess: use cvt value
            {
                new_distance = cvtValue;

                if (abs(new_distance - (Long)(m_pGState->singWidVal * 64)) < (Long)(m_pGState->singWidCut * 64))
                {
                    if (new_distance>=0)
                        new_distance = (Long)(m_pGState->singWidVal * 64);
                    else
                        new_distance = -(Long)(m_pGState->singWidVal * 64);
                }
            }
            else // cut-in test failed: use outline value - no change
            {
                // do nothing
            }

            new_distance = tt_RoundByGState(m_pGState, new_distance, TTDS_NONE);

            // move the point along the freedom vector so that the distance from p to the current position of rp0 
            // is equal to the distance stated in the referenced CVT entry
            if(distance != new_distance)
            {
                //CGlyphCoordinate oldCoord = *point;
                tt_MovePointAlongFreeVector(m_pGState, point, new_distance - distance);
            }
            tt_MarkTouched(m_pGState, point);
            //point->bOnCurve |= PF_TOUCHED;

            // set state variables
            m_pGState->rp0 = m_pGState->rp1 = ptIndex;

            return 0;
        }
        int tt_NPushB(CGState *m_pGState)
        {
            Byte n = m_pGState->pInstr[m_pGState->nOffset++];
            if(m_pGState->nOffset + n > (Long)m_pGState->nInstr) return -1;
            for(Byte i = 0; i < n; i++)
            {
                ULong ele = m_pGState->pInstr[m_pGState->nOffset++];
                m_pGState->m_stack.push(ele);
            }
            return 0;
        }
        int tt_NPUSHW(CGState *m_pGState)
        {
            Byte n = m_pGState->pInstr[m_pGState->nOffset++];
            if(m_pGState->nOffset + 2 * n > (Long)m_pGState->nInstr) return -1;
            for(Byte i = 0; i < n; i++)
            {
                Long ele = (short)((m_pGState->pInstr[m_pGState->nOffset] << 8) + m_pGState->pInstr[m_pGState->nOffset + 1]);
                m_pGState->m_stack.push(ele);
                m_pGState->nOffset += 2;
            }

            return 0;
        }
        int tt_WS(CGState *m_pGState)
        {
            ULong v = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong l = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(l < 0 || l >= m_pGState->m_nStorage) 
                return -1;
            m_pGState->m_pStorage[l] = v;

            return 0;
        }
        int tt_RS(CGState *m_pGState)
        {
            ULong l = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(l < 0 || l >= m_pGState->m_nStorage) 
                return -1;
            m_pGState->m_stack.push(m_pGState->m_pStorage[l]);
            return 0;
        }
        int tt_WCVTP(CGState *m_pGState)
        {
            ULong v = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong l = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(m_pGState->pCvt == NULL || l < 0 || l >= m_pGState->nCvt)
                return -1;
            m_pGState->pCvt[l] = v;

            return 0;
        }
        int tt_RCVT(CGState *m_pGState)
        {
            ULong l = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(m_pGState->pCvt == NULL || l < 0 || l >= m_pGState->nCvt)
                return -1;
            m_pGState->m_stack.push((Long)m_pGState->pCvt[l]);
            //m_pGState->m_stack.push((Long)(m_pGState->pCvt[l] / m_pGState->unitsPerPixel * 64));

            return 0;
        }
        int tt_GC0(CGState *m_pGState)
        {
            ULong index = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt;
            if(m_pGState->zp2 == TTZP_NORMAL)
                pt =  &m_pGState->pOutlines->pCoordinates[index];
            else
                pt = &m_pGState->m_pTwilightZone[index];

            Fixed distance = tt_MeasureDistance(m_pGState->dualVec, pt);

            m_pGState->m_stack.push(distance);

            return 0;
        }
        int tt_GC1(CGState *m_pGState)
        {
            ULong index = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt;
            if(m_pGState->zp2 == TTZP_NORMAL)
                pt =  &m_pGState->pOrigOutlines->pCoordinates[index];
            else
                pt = &m_pGState->m_pTwilightZone[index];

            Fixed distance = tt_MeasureDistance(m_pGState->projVec, pt);

            m_pGState->m_stack.push(distance);

            return 0;
        }
        int tt_SCFS(CGState *m_pGState)
        {
            Fixed new_distance = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong index = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt;
            if(m_pGState->zp2 == TTZP_NORMAL)
                pt =  &m_pGState->pOrigOutlines->pCoordinates[index];
            else
                pt = &m_pGState->m_pTwilightZone[index];

            Fixed distance = tt_MeasureDistance(m_pGState->projVec, pt);
            if(distance != new_distance)
                tt_MovePointAlongFreeVector(m_pGState, pt, new_distance - distance);
            tt_MarkTouched(m_pGState, pt);

            return -1;
        }
        int tt_MD0(CGState *m_pGState) // measure current distance
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt1 = NULL, *pt2 = NULL;
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  &m_pGState->pOutlines->pCoordinates[ptIndex1];
            else
                pt1 = &m_pGState->m_pTwilightZone[ptIndex2];
            if(m_pGState->zp0 == TTZP_NORMAL)
                pt2 =  &m_pGState->pOutlines->pCoordinates[ptIndex2];
            else
                pt2 = &m_pGState->m_pTwilightZone[ptIndex2];

            Fixed value = tt_MeasureDistance(m_pGState->dualVec, pt1, pt2);

            m_pGState->m_stack.push(value);

            return 0;
        }
        int tt_MD1(CGState *m_pGState) // measure original distance
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt1 = NULL, *pt2 = NULL;
            if(m_pGState->zp1 == TTZP_NORMAL)
                pt1 =  &m_pGState->pOrigOutlines->pCoordinates[ptIndex1];
            else
                pt1 = &m_pGState->m_pTwilightZone[ptIndex2];
            if(m_pGState->zp0 == TTZP_NORMAL)
                pt2 =  &m_pGState->pOrigOutlines->pCoordinates[ptIndex2];
            else
                pt2 = &m_pGState->m_pTwilightZone[ptIndex2];

            Fixed value = tt_MeasureDistance(m_pGState->projVec, pt1, pt2);

            m_pGState->m_stack.push(value);

            return 0;
        }
        int tt_MPPEM(CGState *m_pGState)
        {
            m_pGState->m_stack.push(m_pGState->ppem);
            return 0;
        }
        int tt_MPS(CGState *m_pGState)
        {
            m_pGState->m_stack.push((ULong)(m_pGState->fontSize * 64));

            return -1;
        }
        int tt_FLIPON(CGState *m_pGState)
        {
            m_pGState->autoFlip = true;
            return 0;
        }
        int tt_FLIPOFF(CGState *m_pGState)
        {
            m_pGState->autoFlip = false;
            return 0;
        }
        int tt_DEBUG(CGState *m_pGState)
        {
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_LT(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 < e2)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);
                
            return 0;
        }
        int tt_LTEQ(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 <= e2)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);
                
            return 0;
        }
        int tt_GT(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 > e2)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);
                
            return 0;
        }
        int tt_GTEQ(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 >= e2)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);
                
            return 0;
        }
        int tt_EQ(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 == e2)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);
                
            return 0;
        }
        int tt_NEQ(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 != e2)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);
                
            return 0;
        }
        int tt_ODD(CGState *m_pGState)
        {
            Long e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            int value = (int)(tt_RoundByGState(m_pGState, e, TTDS_NONE) / 64.0f);
            
            if((value % 2) == 1) m_pGState->m_stack.push(1);
            else m_pGState->m_stack.push(0);

            return -1;
        }
        int tt_EVEN(CGState *m_pGState)
        {
            Long e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            int value = (int)(tt_RoundByGState(m_pGState, e, TTDS_NONE) / 64.0f);
            
            if((value % 2) == 1) m_pGState->m_stack.push(0);
            else m_pGState->m_stack.push(1);

            return -1;
        }
        int tt_IF(CGState *m_pGState)
        {
            ULong e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e == 0) // jump to else branch or endif
            {
                //m_pGState->nestedIfElse.push_back(0);
                int level = 0;
                bool found = false;
                while(!found && m_pGState->nOffset < (Long)m_pGState->nInstr)
                {
                    if(level == 0 && (m_pGState->pInstr[m_pGState->nOffset] == TT_ELSE || m_pGState->pInstr[m_pGState->nOffset] == TT_EIF))
                    {
                        // found it
                        found = true;
                    }
                    else if(m_pGState->pInstr[m_pGState->nOffset] == TT_IF)
                    {
                        level++;
                    }
                    else if(level > 0 && m_pGState->pInstr[m_pGState->nOffset] == TT_EIF)
                    {
                        level--;
                    }
                    m_pGState->nOffset += tt_GetNextCmdBytes(m_pGState);
                }

                //m_pGState->nOffset++;
            }
            else
            {
                //m_pGState->nestedIfElse.push_back(1);
            }

            return 0;
        }
        int tt_EIF(CGState *m_pGState)
        {
            // do nothing
            return 0;
        }
        int tt_AND(CGState *m_pGState)
        {
            ULong e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 != 0 && e2 != 0)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);

            return 0;
        }
        int tt_OR(CGState *m_pGState)
        {
            ULong e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e1 != 0 || e2 != 0)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);

            return 0;
        }
        int tt_NOT(CGState *m_pGState)
        {
            ULong e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(e == 0)
                m_pGState->m_stack.push(1);
            else
                m_pGState->m_stack.push(0);

            return 0;
        }
        int tt_SDB(CGState *m_pGState)
        {
            m_pGState->deltaBase = (Long)m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_SDS(CGState *m_pGState)
        {
            m_pGState->deltaShift = (Long)m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_ADD(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->m_stack.push(e1 + e2);

            return 0;
        }
        int tt_SUB(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->m_stack.push(e1 - e2);

            return 0;
        }
        int tt_DIV(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->m_stack.push(e1 * 64 / e2);

            return 0;
        }
        int tt_MUL(CGState *m_pGState)
        {
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->m_stack.push(e1 * e2 / 64);

            return 0;
        }
        int tt_ABS(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->m_stack.push(abs(e1));

            return 0;
        }
        int tt_NEG(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            m_pGState->m_stack.push(-e1);

            return 0;
        }
        int tt_FLOOR(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            e1 = (e1 >> 6); 
            e1 = (e1 << 6);
            m_pGState->m_stack.push(e1);


            return 0;
        }
        int tt_CEILING(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if((((ULong)e1) & 0x3F) == 0) m_pGState->m_stack.push(e1);
            else
            {
                e1 = e1 >> 6;
                e1 = e1 << 6;
                m_pGState->m_stack.push(e1);
            }

            return 0;
        }

        int tt_ROUND0(CGState *m_pGState) // round grey
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 = tt_RoundByGState(m_pGState, e1, TTDS_GREY); // distanceType: 0 - grey, 1 - black, 2 -white, 3 - none

            m_pGState->m_stack.push(e1);

            return 0;
        }
        int tt_ROUND1(CGState *m_pGState) // round black
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 = tt_RoundByGState(m_pGState, e1, TTDS_BLACK); // distanceType: 0 - grey, 1 - black, 2 -white, 3 - none

            m_pGState->m_stack.push(e1);

            return 0;
        }
        int tt_ROUND2(CGState *m_pGState) // round white
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 = tt_RoundByGState(m_pGState, e1, TTDS_WHITE); // distanceType: 0 - grey, 1 - black, 2 -white, 3 - none

            m_pGState->m_stack.push(e1);

            return 0;
        }
        int tt_ROUND3(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 = tt_RoundByGState(m_pGState, e1, TTDS_NONE); // distanceType: 0 - grey, 1 - black, 2 -white, 3 - none
            m_pGState->m_stack.push(e1);

            return 0;
        }
        int tt_NROUND0(CGState *m_pGState) // grey
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 += ROUND_Compensation_GREY;

            m_pGState->m_stack.push(e1);

            return -1;
        }
        int tt_NROUND1(CGState *m_pGState) // black
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 += ROUND_Compensation_BLACK;

            m_pGState->m_stack.push(e1);

            return -1;
        }
        int tt_NROUND2(CGState *m_pGState) // white
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 += ROUND_Compensation_WHITE;

            m_pGState->m_stack.push(e1);

            return -1;
        }
        int tt_NROUND3(CGState *m_pGState) // none
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            e1 += ROUND_Compensation_GREY;

            m_pGState->m_stack.push(e1);

            return -1;
        }
        int tt_WCVTF(CGState *m_pGState)
        {
            ULong v = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong l = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            if(m_pGState->pCvt == NULL || l < 0 || l >= m_pGState->nCvt)
                return -1;
            m_pGState->pCvt[l] = (Long)(v / m_pGState->unitsPerPixel);

            return -1;
        }

        Short deltaSteps[16] =  { -8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8 };
        int tt_DELTAP(CGState *m_pGState, int index)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong numPoints;
            CGlyphCoordinate *pCoords;

            if(m_pGState->zp0 == 1) // zone 1
            {
                numPoints = m_pGState->pOutlines->numPoints;
                pCoords = m_pGState->pOutlines->pCoordinates;
            }
            else // twilight zone
            {
                numPoints = m_pGState->m_nTwilightPoints;
                pCoords = m_pGState->m_pTwilightZone;
            }

            for(ULong i = 0; i < n; i++)
            {
                ULong indexPt = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                ULong arg = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(indexPt < 0 || indexPt >= numPoints)
                    continue;
                if((arg >> 4) + index + m_pGState->deltaBase == m_pGState->ppem)
                {
                    UShort selector = arg & 0xF;
                    double step = 1.0 / pow(2,m_pGState->deltaShift) * deltaSteps[selector];

                    //CGlyphCoordinate oldCoord = pCoords[indexPt];

                    tt_MovePointAlongFreeVector(m_pGState, &pCoords[indexPt], (Fixed)(step * 64));

                    tt_MarkTouched(m_pGState, &pCoords[indexPt]);
                    //pCoords[indexPt].bOnCurve |= PF_TOUCHED;
                }
            }

            return 0;
        }
        int tt_DELTAP1(CGState *m_pGState)
        {
            return tt_DELTAP(m_pGState, 0);
        }
        int tt_DELTAP2(CGState *m_pGState)
        {
            return tt_DELTAP(m_pGState, 16);
        }
        int tt_DELTAP3(CGState *m_pGState)
        {
            return tt_DELTAP(m_pGState, 32);
        }

        int tt_DELTAC1(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            for(ULong i = 0; i < n; i++)
            {
                ULong indexCvt = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                ULong arg = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(indexCvt < 0 || indexCvt >= m_pGState->nCvt)
                    return -1;
                if((arg >> 4) + m_pGState->deltaBase == m_pGState->ppem)
                {
                    UShort selector = arg & 0xF;
                    double step = 1.0 / pow(2,m_pGState->deltaShift) * deltaSteps[selector];
                    m_pGState->pCvt[indexCvt] +=  (Long)(step * 64);
                }
            }


            return 0;
        }
        int tt_DELTAC2(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            for(ULong i = 0; i < n; i++)
            {
                ULong indexCvt = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                ULong arg = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(indexCvt < 0 || indexCvt >= m_pGState->nCvt)
                    return -1;
                if((arg >> 4) + 16 + m_pGState->deltaBase == m_pGState->ppem)
                {
                    UShort selector = arg & 0xF;
                    double step = 1.0f / pow(2,m_pGState->deltaShift) * deltaSteps[selector];
                    m_pGState->pCvt[indexCvt] +=  (Long)(step * 64);
                }
            }

            return 0;
        }
        int tt_DELTAC3(CGState *m_pGState)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            for(ULong i = 0; i < n; i++)
            {
                ULong indexCvt = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                ULong arg = m_pGState->m_stack.top();
                m_pGState->m_stack.pop();
                if(indexCvt < 0 || indexCvt >= m_pGState->nCvt)
                    return -1;
                if((arg >> 4) + 32 + m_pGState->deltaBase == m_pGState->ppem)
                {
                    UShort selector = arg & 0xF;
                    double step = 1.0f / pow(2,m_pGState->deltaShift) * deltaSteps[selector];
                    m_pGState->pCvt[indexCvt] +=  (Long)(step * 64);
                }
            }

            return 0;
        }
        int tt_SuperRound(CGState *m_pGState, float unitPeriod)
        {
            ULong n = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            ULong period = (n >> 6) & 3;
            ULong phase = (n >> 4) & 3;
            ULong threshold = ( n & 15);

            switch(period)
            {
            case 0:
                m_pGState->sRndPeriod = 0.5f;
                break;
            case 1:
                m_pGState->sRndPeriod = 1.0f;
                break;
            case 2:
                m_pGState->sRndPeriod = 2.0f;
                break;
            case 3:
                m_pGState->sRndPeriod = 1.0f;
                break;
            }
            
            m_pGState->sRndPeriod *= unitPeriod;

            switch(phase)
            {
            case 0:
                m_pGState->sRndPhase = 0.0f;
                break;
            case 1:
                m_pGState->sRndPhase = 0.25f * m_pGState->sRndPeriod;
                break;
            case 2:
                m_pGState->sRndPhase = 0.5f * m_pGState->sRndPeriod;
                break;
            case 3:
                m_pGState->sRndPhase = 0.75f * m_pGState->sRndPeriod;
                break;
            }
            switch(threshold)
            {
            case 0:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod - 1.0f / 64;
                break;
            case 1:
                m_pGState->sRndThreshold = - m_pGState->sRndPeriod * 3 / 8;
                break;
            case 2:
                m_pGState->sRndThreshold = - m_pGState->sRndPeriod * 2 / 8;
                break;
            case 3:
                m_pGState->sRndThreshold = - m_pGState->sRndPeriod * 1 / 8;
                break;
            case 4:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 0 / 8;
                break;
            case 5:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 1 / 8;
                break;
            case 6:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 2 / 8;
                break;
            case 7:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 3 / 8;
                break;
            case 8:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 4 / 8;
                break;
            case 9:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 5 / 8;
                break;
            case 10:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 6 / 8;
                break;
            case 11:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 7 / 8;
                break;
            case 12:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 8 / 8;
                break;
            case 13:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 9 / 8;
                break;
            case 14:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 10 / 8;
                break;
            case 15:
                m_pGState->sRndThreshold = m_pGState->sRndPeriod * 11 / 8;
                break;
            }

            return 0;
        }
        int tt_SROUND(CGState *m_pGState)
        {
            return tt_SuperRound(m_pGState, 1.0f);
        }
        int tt_S45ROUND(CGState *m_pGState)
        {
            return tt_SuperRound(m_pGState, 1.4142f);
        }
        int tt_JROT(CGState *m_pGState)
        {
            ULong b = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long offset = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(b != 0)
                m_pGState->nOffset += (offset - 1);

            return 0;
        }
        int tt_JROF(CGState *m_pGState)
        {
            ULong b = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long offset = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(b == 0)
                m_pGState->nOffset += (offset - 1);

            return 0;
        }
        int tt_ROFF(CGState *m_pGState)
        {
            m_pGState->rndState = TTRS_OFF;

            return 0;
        }
        int tt_RUTG(CGState *m_pGState)
        {
            m_pGState->rndState = TTRS_UP_GRID;

            return 0;
        }
        int tt_RDTG(CGState *m_pGState)
        {
            m_pGState->rndState = TTRS_DOWN_GRID;

            return 0;
        }
        int tt_SANGW(CGState *m_pGState) // not supported any more in true type font specs
        {
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_AA(CGState *m_pGState) // not supported any more in true type font specs
        {
            m_pGState->m_stack.pop();
            return 0;
        }
        int tt_FLIPPT(CGState *m_pGState)
        {
            ULong index =  m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pt;
            if(m_pGState->zp0 == TTZP_NORMAL)
                pt =  &m_pGState->pOutlines->pCoordinates[index];
            else
                pt = &m_pGState->m_pTwilightZone[index];

            if((pt->bOnCurve & PF_ONCURVE) == PF_ONCURVE) pt->bOnCurve &= (~PF_ONCURVE);
            else pt->bOnCurve |= PF_ONCURVE;

            return 0;
        }
        int tt_FLIPRGON(CGState *m_pGState)
        {
            ULong h =  m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong l =  m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pCoordinates;
            if(m_pGState->zp0 == TTZP_NORMAL)
                pCoordinates = m_pGState->pOutlines->pCoordinates;
            else
                pCoordinates = m_pGState->m_pTwilightZone;

            for(ULong i = l; i <= h; i++)
                pCoordinates[i].bOnCurve |= PF_ONCURVE;

            return 0;
        }
        int tt_FLIPRGOFF(CGState *m_pGState)
        {
            ULong h =  m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong l =  m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate *pCoordinates;
            if(m_pGState->zp0 == TTZP_NORMAL)
                pCoordinates = m_pGState->pOutlines->pCoordinates;
            else
                pCoordinates = m_pGState->m_pTwilightZone;

            for(ULong i = l; i <= h; i++)
                pCoordinates[i].bOnCurve &= (~PF_ONCURVE);

            return 0;
        }
        int tt_SCANCTRL(CGState *m_pGState)
        {
            ULong e = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            ULong threshold = e & 0x7F;

            if(m_pGState->ppem < (Long)threshold) m_pGState->scanControl = true;
            else m_pGState->scanControl = false;

            // TODO: Check bit 8~13

            return 0;
        }
        int tt_SDPVTL0(CGState *m_pGState)
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate pt1, pt2, opt1, opt2;
            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                opt1 = m_pGState->pOrigOutlines->pCoordinates[ptIndex1];
                pt1 =  m_pGState->pOutlines->pCoordinates[ptIndex1];
            }
            else
            {
                opt1 = m_pGState->m_pTwilightZone[ptIndex1];
                pt1 = m_pGState->m_pTwilightZone[ptIndex1];
            }

            if(m_pGState->zp2 == TTZP_NORMAL)
            {
                opt2 = m_pGState->pOrigOutlines->pCoordinates[ptIndex2];
                pt2 =  m_pGState->pOutlines->pCoordinates[ptIndex2];
            }
            else
            {
                opt2 = m_pGState->m_pTwilightZone[ptIndex2];
                pt2 = m_pGState->m_pTwilightZone[ptIndex2];
            }

            double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x);
            m_pGState->projVec.y = (float)sin(angle);
            m_pGState->projVec.x = (float)cos(angle);

            angle = atan2(opt2.y - opt1.y, opt2.x - opt1.x);
            m_pGState->dualVec.y = (float)sin(angle);
            m_pGState->dualVec.x = (float)cos(angle);

            return 0;
        }
        int tt_SDPVTL1(CGState *m_pGState)
        {
            ULong ptIndex1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong ptIndex2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate pt1, pt2, opt1, opt2;
            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                opt1 = m_pGState->pOrigOutlines->pCoordinates[ptIndex1];
                pt1 =  m_pGState->pOutlines->pCoordinates[ptIndex1];
            }
            else
            {
                opt1 = m_pGState->m_pTwilightZone[ptIndex1];
                pt1 = m_pGState->m_pTwilightZone[ptIndex1];
            }

            if(m_pGState->zp2 == TTZP_NORMAL)
            {
                opt2 = m_pGState->pOrigOutlines->pCoordinates[ptIndex2];
                pt2 =  m_pGState->pOutlines->pCoordinates[ptIndex2];
            }
            else
            {
                opt2 = m_pGState->m_pTwilightZone[ptIndex2];
                pt2 = m_pGState->m_pTwilightZone[ptIndex2];
            }

            double angle = atan2(pt2.y - pt1.y, pt2.x - pt1.x) + PI / 2;
            m_pGState->projVec.y = (float)sin(angle);
            m_pGState->projVec.x = (float)cos(angle);

            angle = atan2(opt2.y - opt1.y, opt2.x - opt1.x) + PI / 2;
            m_pGState->dualVec.y = (float)sin(angle);
            m_pGState->dualVec.x = (float)cos(angle);

            return 0;
        }
        int tt_GETINFO(CGState *m_pGState)
        {
            ULong r = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            ULong v = 0;
            if (r & 1) v |= 35; // font engine version number: 35   GDI (Win 98/2000)         v1.7  (UN)SCALED_COMPONENT_OFFSET bits in composite glyphs
            if(r & 2) v |= 0; // current glyph rotated: no        TODO: result - bit 8
            if(r & 4) v |= 0; // current glyph stretched: no        TODO: result - bit 9
            if (r & 32)         // ask for grayscale
                v |= 0;// 0x1000;

            m_pGState->m_stack.push(v);

            return 0;
        }
        int tt_Call_IDEF(CGState *m_pGState)
        {
            ULong f = 0x100 + m_pGState->curOpCode;

            std::map<ULong, TT_NestedFunc*>::iterator it = m_pGState->m_pFuncDefs->find(f);
            if(it != m_pGState->m_pFuncDefs->end())
            {
                // store function environment
                TT_NestedFunc *pF = new TT_NestedFunc(m_pGState->nOffset, m_pGState->pInstr, m_pGState->nInstr, it->second->offset, 1, true);

                // move the pointer to function
                m_pGState->pInstr = it->second->pInstr;
                m_pGState->nInstr = it->second->nInstr;
                m_pGState->nOffset = it->second->offset;

                m_pGState->nestedFuncs.push_back(pF);
            }
            else
                return -1;

            return 0;
        }
        int tt_IDEF(CGState *m_pGState)
        {
            ULong opCode = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(m_pGState->m_pFuncDefs)
            {
                TT_NestedFunc *pF= new TT_NestedFunc(m_pGState->nOffset, m_pGState->pInstr, m_pGState->nInstr, 0, 1, true);
                m_pGState->m_pFuncDefs->insert(std::pair<ULong, TT_NestedFunc*>(0x100 + opCode, pF));

                TT_CommandList.OpProcs[opCode] = tt_Call_IDEF;
            }

            return -1;
        }

        int tt_ROLL(CGState *m_pGState)
        {
            ULong e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            ULong e3 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            m_pGState->m_stack.push(e2);
            m_pGState->m_stack.push(e1);
            m_pGState->m_stack.push(e3);

            return 0;
        }
        int tt_MAX(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(e1 > e2) m_pGState->m_stack.push(e1);
            else m_pGState->m_stack.push(e2);

            return 0;
        }
        int tt_MIN(CGState *m_pGState)
        {
            Long e1 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long e2 = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(e1 > e2) m_pGState->m_stack.push(e2);
            else m_pGState->m_stack.push(e1);

            return 0;
        }
        int tt_SCANTYPE(CGState *m_pGState)
        {
            m_pGState->scanType = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            return 0;
        }
        int tt_INSTCTRL(CGState *m_pGState) // this command can only be excuted in CVT program
        {
            Long selector = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long value = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            if(selector == TTIC_NOT_RUN_GLYPH_INSTRUCTS)
            {
                // value 0: run glyph instruction
                // value 1: not run
                if(value == 0)
                    m_pGState->instrControl &= (~1);
                else
                    m_pGState->instrControl |= 1;
            }
            else if(selector == TTIC_IGNORE_CVT_CHANGES)
            {
                // value 0: use current value
                // value 1: use default value
                if(value == 0)
                    m_pGState->instrControl &= (~2);
                else
                    m_pGState->instrControl |= 2;
            }
            else if(selector == 3)
            {
                // not support ClearType
            }

            return 0;
        }
        int tt_PUSHB(CGState *m_pGState, int num)
        {
            if(m_pGState->nOffset + num > (Long)m_pGState->nInstr)
                return 0;

            for(int i = 0; i<num; i++)
            {
                ULong w = m_pGState->pInstr[m_pGState->nOffset++];
                m_pGState->m_stack.push(w);
            }

            return 0;
        }
        int tt_PUSHB1(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 1);
        }
        int tt_PUSHB2(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 2);
        }
        int tt_PUSHB3(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 3);
        }
        int tt_PUSHB4(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 4);
        }
        int tt_PUSHB5(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 5);
        }
        int tt_PUSHB6(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 6);
        }
        int tt_PUSHB7(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 7);
        }
        int tt_PUSHB8(CGState *m_pGState)
        {
            return tt_PUSHB(m_pGState, 8);
        }

        int tt_PUSHW(CGState *m_pGState, int num)
        {
            if(m_pGState->nOffset + 2*num > (Long)m_pGState->nInstr)
                return 0;

            for(int i = 0; i<num; i++)
            {
                Long w = (Short)((m_pGState->pInstr[m_pGState->nOffset] << 8) + m_pGState->pInstr[m_pGState->nOffset+1]);
                m_pGState->m_stack.push(w);
                m_pGState->nOffset += 2;
            }

            return 0;
        }
        int tt_PUSHW1(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 1);
        }
        int tt_PUSHW2(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 2);
        }
        int tt_PUSHW3(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 3);
        }
        int tt_PUSHW4(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 4);
        }
        int tt_PUSHW5(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 5);
        }
        int tt_PUSHW6(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 6);
        }
        int tt_PUSHW7(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 7);
        }
        int tt_PUSHW8(CGState *m_pGState)
        {
            return tt_PUSHW(m_pGState, 8);
        }

        int tt_MDRP(CGState *m_pGState, bool bSetRp0, bool bKeepMinDistance, bool bRound, TT_DISTANCE_TYPE distanceType)
        {
            Long ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate rp0, o_rp0;
            CGlyphCoordinate *point = NULL, *o_point = NULL;

            if(m_pGState->zp0 == TTZP_NORMAL)
            {
                o_rp0 = m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp0];
                rp0 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp0];
            }
            else
            {
                o_rp0 = m_pGState->m_pTwilightZone[m_pGState->rp0];
                rp0 = m_pGState->m_pTwilightZone[m_pGState->rp0];
            }
            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_point = &m_pGState->pOrigOutlines->pCoordinates[ptIndex];
                point =  &m_pGState->pOutlines->pCoordinates[ptIndex];
            }
            else
            {
                o_point = &m_pGState->m_pTwilightZone[ptIndex];
                point = &m_pGState->m_pTwilightZone[ptIndex];
            }

            // get distance between point and rp0
            Fixed o_distance = tt_MeasureDistance(m_pGState->dualVec, &o_rp0, o_point);
            Fixed cur_distance = tt_MeasureDistance(m_pGState->projVec, &rp0, point);

            Fixed distance = o_distance;
            if (abs(abs(distance) - (Long)(m_pGState->singWidVal * 64)) < (Long)(m_pGState->singWidCut * 64))
            {
                if (o_distance >= 0)
                    distance = (Long)(m_pGState->singWidVal * 64);
                else
                    distance = -(Long)(m_pGState->singWidVal * 64);
            }

            if(bRound) 
                distance = tt_RoundByGState(m_pGState, distance, distanceType);
            else
            {
                // add device compensation
            }

            if(bKeepMinDistance)
            {
                Long minDist = (Long)(m_pGState->minDist * 64);
                if (abs(distance) < minDist) distance = (o_distance >= 0) ? minDist : -minDist;
            }


            // move the point along the freedom vector so that the distance from p to the current position of rp0 
            // is equal to orignal distance
            tt_MovePointAlongFreeVector(m_pGState, point, distance - cur_distance);

            tt_MarkTouched(m_pGState, point);

            // set state variables
            m_pGState->rp1 = m_pGState->rp0;
            m_pGState->rp2 = ptIndex;
            if(bSetRp0) m_pGState->rp0 = ptIndex;

            return 0;
        }
        int tt_MDRP00(CGState *m_pGState) // [a b c de] = 0x0 ( 00000)
        {
            return tt_MDRP(m_pGState, false, false, false, TTDS_GREY);
        }
        int tt_MDRP01(CGState *m_pGState) // [a b c de] = 0x1 ( 00001)
        {
            return tt_MDRP(m_pGState, false, false, false, TTDS_BLACK);
        }
        int tt_MDRP02(CGState *m_pGState) // [a b c de] = 0x2 ( 00010)
        {
            return tt_MDRP(m_pGState, false, false, false, TTDS_WHITE);
        }
        int tt_MDRP03(CGState *m_pGState) // [a b c de] = 0x3 ( 00011)
        {
            return tt_MDRP(m_pGState, false, false, false, TTDS_NONE);
        }
        int tt_MDRP04(CGState *m_pGState) // [a b c de] = 0x4 ( 00100)
        {
            return tt_MDRP(m_pGState, false, false, true, TTDS_GREY);
        }
        int tt_MDRP05(CGState *m_pGState) // [a b c de] = 0x5 ( 00101)
        {
            return tt_MDRP(m_pGState, false, false, true, TTDS_BLACK);
        }
        int tt_MDRP06(CGState *m_pGState) // [a b c de] = 0x6 ( 00110)
        {
            return tt_MDRP(m_pGState, false, false, true, TTDS_WHITE);
        }
        int tt_MDRP07(CGState *m_pGState) // [a b c de] = 0x7 ( 00111)
        {
            return tt_MDRP(m_pGState, false, false, true, TTDS_NONE);
        }
        int tt_MDRP08(CGState *m_pGState) // [a b c de] = 0x8 ( 01000)
        {
            return tt_MDRP(m_pGState, false, true, false, TTDS_GREY);
        }
        int tt_MDRP09(CGState *m_pGState) // [a b c de] = 0x9 ( 01001)
        {
            return tt_MDRP(m_pGState, false, true, false, TTDS_BLACK);
        }
        int tt_MDRP0A(CGState *m_pGState) // [a b c de] = 0xA ( 01010)
        {
            return tt_MDRP(m_pGState, false, true, false, TTDS_WHITE);
        }
        int tt_MDRP0B(CGState *m_pGState) // [a b c de] = 0xB ( 01011)
        {
            return tt_MDRP(m_pGState, false, true, false, TTDS_NONE);
        }
        int tt_MDRP0C(CGState *m_pGState) // [a b c de] = 0xC ( 01100)
        {
            return tt_MDRP(m_pGState, false, true, true, TTDS_GREY);
        }
        int tt_MDRP0D(CGState *m_pGState) // [a b c de] = 0xD ( 01101)
        {
            return tt_MDRP(m_pGState, false, true, true, TTDS_BLACK);
        }
        int tt_MDRP0E(CGState *m_pGState) // [a b c de] = 0xE ( 01110)
        {
            return tt_MDRP(m_pGState, false, true, true, TTDS_WHITE);
        }
        int tt_MDRP0F(CGState *m_pGState) // [a b c de] = 0xF ( 01111)
        {
            return tt_MDRP(m_pGState, false, true, true, TTDS_NONE);
        }
        int tt_MDRP10(CGState *m_pGState) // [a b c de] = 0x10 ( 10000)
        {
            return tt_MDRP(m_pGState, true, false, false, TTDS_GREY);
        }
        int tt_MDRP11(CGState *m_pGState) // [a b c de] = 0x11 ( 10001)
        {
            return tt_MDRP(m_pGState, true, false, false, TTDS_BLACK);
        }
        int tt_MDRP12(CGState *m_pGState) // [a b c de] = 0x12 ( 10010)
        {
            return tt_MDRP(m_pGState, true, false, false, TTDS_WHITE);
        }
        int tt_MDRP13(CGState *m_pGState) // [a b c de] = 0x13 ( 10011)
        {
            return tt_MDRP(m_pGState, true, false, false, TTDS_NONE);
        }
        int tt_MDRP14(CGState *m_pGState) // [a b c de] = 0x14 ( 10100)
        {
            return tt_MDRP(m_pGState, true, false, true, TTDS_GREY);
        }
        int tt_MDRP15(CGState *m_pGState) // [a b c de] = 0x15 ( 10101)
        {
            return tt_MDRP(m_pGState, true, false, true, TTDS_BLACK);
        }
        int tt_MDRP16(CGState *m_pGState) // [a b c de] = 0x16 ( 10110)
        {
            return tt_MDRP(m_pGState, true, false, true, TTDS_WHITE);
        }
        int tt_MDRP17(CGState *m_pGState) // [a b c de] = 0x17 ( 10111)
        {
            return tt_MDRP(m_pGState, true, false, true, TTDS_NONE);
        }
        int tt_MDRP18(CGState *m_pGState) // [a b c de] = 0x18 ( 11000)
        {
            return tt_MDRP(m_pGState, true, true, false, TTDS_GREY);
        }
        int tt_MDRP19(CGState *m_pGState) // [a b c de] = 0x19 ( 11001)
        {
            return tt_MDRP(m_pGState, true, true, false, TTDS_BLACK);
        }
        int tt_MDRP1A(CGState *m_pGState) // [a b c de] = 0x1A ( 11010)
        {
            return tt_MDRP(m_pGState, true, true, false, TTDS_WHITE);
        }
        int tt_MDRP1B(CGState *m_pGState) // [a b c de] = 0x1B ( 11011)
        {
            return tt_MDRP(m_pGState, true, true, false, TTDS_NONE);
        }
        int tt_MDRP1C(CGState *m_pGState) // [a b c de] = 0x1C ( 11100)
        {
            return tt_MDRP(m_pGState, true, true, true, TTDS_GREY);
        }
        int tt_MDRP1D(CGState *m_pGState) // [a b c de] = 0x1D ( 11101)
        {
            return tt_MDRP(m_pGState, true, true, true, TTDS_BLACK);
        }
        int tt_MDRP1E(CGState *m_pGState) // [a b c de] = 0x1E ( 11110)
        {
            return tt_MDRP(m_pGState, true, true, true, TTDS_WHITE);
        }
        int tt_MDRP1F(CGState *m_pGState) // [a b c de] = 0x1F ( 11111)
        {
            return tt_MDRP(m_pGState, true, true, true, TTDS_NONE);
        }

        int tt_MIRP(CGState *m_pGState, bool bSetRp0, bool bKeepMinDistance, bool bRoundAndCutInTest, TT_DISTANCE_TYPE distanceType)
        {
            Long cvtIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();
            Long ptIndex = m_pGState->m_stack.top();
            m_pGState->m_stack.pop();

            CGlyphCoordinate rp0, o_rp0;
            CGlyphCoordinate *point = NULL, *o_point = NULL;

            if(m_pGState->zp0 == TTZP_NORMAL)
            {
                o_rp0 = m_pGState->pOrigOutlines->pCoordinates[m_pGState->rp0];
                rp0 =  m_pGState->pOutlines->pCoordinates[m_pGState->rp0];
            }
            else
            {
                o_rp0 = m_pGState->m_pTwilightZone[m_pGState->rp0];
                rp0 = m_pGState->m_pTwilightZone[m_pGState->rp0];
            }
            if(m_pGState->zp1 == TTZP_NORMAL)
            {
                o_point = &m_pGState->pOrigOutlines->pCoordinates[ptIndex];
                point =  &m_pGState->pOutlines->pCoordinates[ptIndex];
            }
            else
            {
                o_point = &m_pGState->m_pTwilightZone[ptIndex];
                point = &m_pGState->m_pTwilightZone[ptIndex];
            }

            // get distance between point and rp0
            Fixed o_distance = tt_MeasureDistance(m_pGState->dualVec, &o_rp0, o_point);
            Fixed cur_distance = tt_MeasureDistance(m_pGState->projVec, &rp0, point);

            // cut-in test
            Fixed new_distance = o_distance;
            Long cvtValue = m_pGState->pCvt[cvtIndex];
            if (m_pGState->autoFlip && o_distance * cvtValue < 0)
                cvtValue = -cvtValue;
            if (bRoundAndCutInTest)
            {
                if( abs(new_distance - cvtValue) < (Long)(m_pGState->cvtCutin * 64)) // cut-in test success: use cvt value
                {
                    new_distance = cvtValue;

                    // width cut-in test
                    if(abs(new_distance - (Long)(m_pGState->singWidVal * 64)) < (Long)(m_pGState->singWidCut * 64))
                        new_distance = (Long)(m_pGState->singWidVal * 64) * (new_distance>=0?1:-1);
 
                    // round
                    new_distance = tt_RoundByGState(m_pGState, new_distance, distanceType);
                }
                else // cut-in test failed: use outline value - no change
                {
                    // do nothing
                }

                //new_distance = tt_RoundByGState(m_pGState, new_distance, distanceType);
            }
            else
            {
                new_distance = cvtValue;
            }

            if(bKeepMinDistance)
            {
                Long minDist = (Long)(m_pGState->minDist * 64);
                if (abs(new_distance) < minDist)
                    new_distance = (o_distance >= 0) ? minDist : -minDist;
            }

            // move the point along the freedom vector so that the distance from p to the current position of rp0 
            // is equal to the distance stated in the referenced CVT entry
            tt_MovePointAlongFreeVector(m_pGState, point, new_distance - cur_distance);

            //if(bRoundAndCutInTest)
            //{
            //    if(m_pGState->projVec.x != 0)
            //        point->x = tt_RoundByGState(m_pGState, (Long)(point->x * 64), distanceType) / 64.0f;
            //    if(m_pGState->projVec.y != 0)
            //        point->y = tt_RoundByGState(m_pGState, (Long)(point->y * 64), distanceType) / 64.0f;
            //}

            tt_MarkTouched(m_pGState, point);

            // set state variables
            m_pGState->rp1 = m_pGState->rp0;
            m_pGState->rp2 = ptIndex;
            if(bSetRp0) m_pGState->rp0 = ptIndex;

            return 0;
        }
        int tt_MIRP0(CGState *m_pGState) // [a b c de] = 0x0 ( 00000)
        {
            return tt_MIRP(m_pGState, false, false, false, TTDS_GREY);
        }
        int tt_MIRP1(CGState *m_pGState) // [a b c de] = 0x1 ( 00001)
        {
            return tt_MIRP(m_pGState, false, false, false, TTDS_BLACK);
        }
        int tt_MIRP2(CGState *m_pGState) // [a b c de] = 0x2 ( 00010)
        {
            return tt_MIRP(m_pGState, false, false, false, TTDS_WHITE);
        }
        int tt_MIRP3(CGState *m_pGState) // [a b c de] = 0x3 ( 00011)
        {
            return tt_MIRP(m_pGState, false, false, false, TTDS_NONE);
        }
        int tt_MIRP4(CGState *m_pGState) // [a b c de] = 0x4 ( 00100)
        {
            return tt_MIRP(m_pGState, false, false, true, TTDS_GREY);
        }
        int tt_MIRP5(CGState *m_pGState) // [a b c de] = 0x5 ( 00101)
        {
            return tt_MIRP(m_pGState, false, false, true, TTDS_BLACK);
        }
        int tt_MIRP6(CGState *m_pGState) // [a b c de] = 0x6 ( 00110)
        {
            return tt_MIRP(m_pGState, false, false, true, TTDS_WHITE);
        }
        int tt_MIRP7(CGState *m_pGState) // [a b c de] = 0x7 ( 00111)
        {
            return tt_MIRP(m_pGState, false, false, true, TTDS_NONE);
        }
        int tt_MIRP8(CGState *m_pGState) // [a b c de] = 0x8 ( 01000)
        {
            return tt_MIRP(m_pGState, false, true, false, TTDS_GREY);
        }
        int tt_MIRP9(CGState *m_pGState) // [a b c de] = 0x9 ( 01001)
        {
            return tt_MIRP(m_pGState, false, true, false, TTDS_BLACK);
        }
        int tt_MIRPA(CGState *m_pGState) // [a b c de] = 0xA ( 01010)
        {
            return tt_MIRP(m_pGState, false, true, false, TTDS_WHITE);
        }
        int tt_MIRPB(CGState *m_pGState) // [a b c de] = 0xB ( 01011)
        {
            return tt_MIRP(m_pGState, false, true, false, TTDS_NONE);
        }
        int tt_MIRPC(CGState *m_pGState) // [a b c de] = 0xC ( 01100),  a = 0, b = 1, c = 1, de = 00: Gray=0, Black=1, White=2
        {
            return tt_MIRP(m_pGState, false, true, true, TTDS_GREY);
        }
        int tt_MIRPD(CGState *m_pGState) // [a b c de] = 0xD ( 01101),  a = 0, b = 1, c = 1, de = 01: Gray=0, Black=1, White=2
        {
            return tt_MIRP(m_pGState, false, true, true, TTDS_BLACK);
        }
        int tt_MIRPE(CGState *m_pGState) // [a b c de] = 0xE ( 01110)
        {
            return tt_MIRP(m_pGState, false, true, true, TTDS_WHITE);
        }
        int tt_MIRPF(CGState *m_pGState) // [a b c de] = 0xF ( 01111)
        {
            return tt_MIRP(m_pGState, false, true, true, TTDS_NONE);
        }
        int tt_MIRP10(CGState *m_pGState) // [a b c de] = 0x10 ( 10000)
        {
            return tt_MIRP(m_pGState, true, false, false, TTDS_GREY);
        }
        int tt_MIRP11(CGState *m_pGState) // [a b c de] = 0x11 ( 10001)
        {
            return tt_MIRP(m_pGState, true, false, false, TTDS_BLACK);
        }
        int tt_MIRP12(CGState *m_pGState) // [a b c de] = 0x12 ( 10010)
        {
            return tt_MIRP(m_pGState, true, false, false, TTDS_WHITE);
        }
        int tt_MIRP13(CGState *m_pGState) // [a b c de] = 0x13 ( 10011)
        {
            return tt_MIRP(m_pGState, true, false, false, TTDS_NONE);
        }
        int tt_MIRP14(CGState *m_pGState) // [a b c de] = 0x14 ( 10100)
        {
            return tt_MIRP(m_pGState, true, false, true, TTDS_GREY);
        }
        int tt_MIRP15(CGState *m_pGState) // [a b c de] = 0x15 ( 10101)
        {
            return tt_MIRP(m_pGState, true, false, true, TTDS_BLACK);
        }
        int tt_MIRP16(CGState *m_pGState) // [a b c de] = 0x16 ( 10110)
        {
            return tt_MIRP(m_pGState, true, false, true, TTDS_WHITE);
        }
        int tt_MIRP17(CGState *m_pGState) // [a b c de] = 0x17 ( 10111)
        {
            return tt_MIRP(m_pGState, true, false, true, TTDS_NONE);
        }
        int tt_MIRP18(CGState *m_pGState) // [a b c de] = 0x18 ( 11000)
        {
            return tt_MIRP(m_pGState, true, true, false, TTDS_GREY);
        }
        int tt_MIRP19(CGState *m_pGState) // [a b c de] = 0x19 ( 11001)
        {
            return tt_MIRP(m_pGState, true, true, false, TTDS_BLACK);
        }
        int tt_MIRP1A(CGState *m_pGState) // [a b c de] = 0x1A ( 11010)
        {
            return tt_MIRP(m_pGState, true, true, false, TTDS_WHITE);
        }
        int tt_MIRP1B(CGState *m_pGState) // [a b c de] = 0x1B ( 11011)
        {
            return tt_MIRP(m_pGState, true, true, false, TTDS_NONE);
        }
        int tt_MIRP1C(CGState *m_pGState) // [a b c de] = 0x1C ( 11100)
        {
            return tt_MIRP(m_pGState, true, true, true, TTDS_GREY);
        }
        int tt_MIRP1D(CGState *m_pGState) // [a b c de] = 0x1D ( 11101)
        {
            return tt_MIRP(m_pGState, true, true, true, TTDS_BLACK);
        }
        int tt_MIRP1E(CGState *m_pGState) // [a b c de] = 0x1E ( 11110)
        {
            return tt_MIRP(m_pGState, true, true, true, TTDS_WHITE);
        }
        int tt_MIRP1F(CGState *m_pGState) // [a b c de] = 0x1F ( 11111)
        {
            return tt_MIRP(m_pGState, true, true, true, TTDS_NONE);
        }
        int tt_Wrong(CGState *m_pGState)
        {
            return 1;
        }

        CInstrCommand::CInstrCommand()
        {
            OpProcs[TT_SVTCA0] = tt_SVTCA0;
            OpProcs[TT_SVTCA1] = tt_SVTCA1;
            OpProcs[TT_SPVTCA0] = tt_SPVTCA0;
            OpProcs[TT_SPVTCA1] = tt_SPVTCA1;
            OpProcs[TT_SFVTCA0] = tt_SFVTCA0;
            OpProcs[TT_SFVTCA1] = tt_SFVTCA1;
            OpProcs[TT_SPVTL0] = tt_SPVTL0;
            OpProcs[TT_SPVTL1] = tt_SPVTL1;
            OpProcs[TT_SFVTL0] = tt_SFVTL0;
            OpProcs[TT_SFVTL1] = tt_SFVTL1;
            OpProcs[TT_SPVFS] = tt_SPVFS;
            OpProcs[TT_SFVFS] = tt_SFVFS;
            OpProcs[TT_SFVTPV] = tt_SFVTPV;
            OpProcs[TT_GPV] = tt_GPV;
            OpProcs[TT_GFV] = tt_GFV;
            OpProcs[TT_ISECT] = tt_ISECT;
            OpProcs[TT_SRP0] = tt_SRP0;
            OpProcs[TT_SRP1] = tt_SRP1;
            OpProcs[TT_SRP2] = tt_SRP2;
            OpProcs[TT_SZP0] = tt_SZP0;
            OpProcs[TT_SZP1] = tt_SZP1;
            OpProcs[TT_SZP2] = tt_SZP2;
            OpProcs[TT_SZPS] = tt_SZPS;
            OpProcs[TT_SLOOP] = tt_SLOOP;
            OpProcs[TT_RTG] = tt_RTG;
            OpProcs[TT_RTHG] = tt_RTHG;
            OpProcs[TT_SMD] = tt_SMD;
            OpProcs[TT_ELSE] = tt_ELSE;
            OpProcs[TT_JMPR] = tt_JMPR;
            OpProcs[TT_SCVTCI] = tt_SCVTCI;
            OpProcs[TT_SSWCI] = tt_SSWCI;
            OpProcs[TT_SSW] = tt_SSW;
            OpProcs[TT_DUP] = tt_DUP;
            OpProcs[TT_POP] = tt_POP;
            OpProcs[TT_CLEAR] = tt_CLEAR;
            OpProcs[TT_SWAP] = tt_SWAP;
            OpProcs[TT_DEPTH] = tt_DEPTH;
            OpProcs[TT_CINDEX] = tt_CINDEX;
            OpProcs[TT_MINDEX] = tt_MINDEX;
            OpProcs[TT_ALIGNPTS] = tt_ALIGNPTS;
            OpProcs[TT_UTP] = tt_UTP;
            OpProcs[TT_LOOPCALL] = tt_LOOPCALL;
            OpProcs[TT_CALL] = tt_CALL;
            OpProcs[TT_FDEF] = tt_FDEF;
            OpProcs[TT_ENDF] = tt_ENDF;
            OpProcs[TT_MDAP0] = tt_MDAP0;
            OpProcs[TT_MDAP1] = tt_MDAP1;
            OpProcs[TT_IUP0] = tt_IUP0;
            OpProcs[TT_IUP1] = tt_IUP1;
            OpProcs[TT_SHP0] = tt_SHP0;
            OpProcs[TT_SHP1] = tt_SHP1;
            OpProcs[TT_SHC0] = tt_SHC0;
            OpProcs[TT_SHC1] = tt_SHC1;
            OpProcs[TT_SHZ0] = tt_SHZ0;
            OpProcs[TT_SHZ1] = tt_SHZ1;
            OpProcs[TT_SHPIX] = tt_SHPIX;
            OpProcs[TT_IP] = tt_IP;
            OpProcs[TT_MSIRP0] = tt_MSIRP0;
            OpProcs[TT_MSIRP1] = tt_MSIRP1;
            OpProcs[TT_ALIGNRP] = tt_ALIGNRP;
            OpProcs[TT_RTDG] = tt_RTDG;
            OpProcs[TT_MIAP0] = tt_MIAP0;
            OpProcs[TT_MIAP1] = tt_MIAP1;
            OpProcs[TT_NPUSHB] = tt_NPushB;
            OpProcs[TT_NPUSHW] = tt_NPUSHW;
            OpProcs[TT_WS] = tt_WS;
            OpProcs[TT_RS] = tt_RS;
            OpProcs[TT_WCVTP] = tt_WCVTP;
            OpProcs[TT_RCVT] = tt_RCVT;
            OpProcs[TT_GC0] = tt_GC0;
            OpProcs[TT_GC1] = tt_GC1;
            OpProcs[TT_SCFS] = tt_SCFS;
            OpProcs[TT_MD0] = tt_MD0;
            OpProcs[TT_MD1] = tt_MD1;
            OpProcs[TT_MPPEM] = tt_MPPEM;
            OpProcs[TT_MPS] = tt_MPS;
            OpProcs[TT_FLIPON] = tt_FLIPON;
            OpProcs[TT_FLIPOFF] = tt_FLIPOFF;
            OpProcs[TT_DEBUG] = tt_DEBUG;
            OpProcs[TT_LT] = tt_LT;
            OpProcs[TT_LTEQ] = tt_LTEQ;
            OpProcs[TT_GT] = tt_GT;
            OpProcs[TT_GTEQ] = tt_GTEQ;
            OpProcs[TT_EQ] = tt_EQ;
            OpProcs[TT_NEQ] = tt_NEQ;
            OpProcs[TT_ODD] = tt_ODD;
            OpProcs[TT_EVEN] = tt_EVEN;
            OpProcs[TT_IF] = tt_IF;
            OpProcs[TT_EIF] = tt_EIF;
            OpProcs[TT_AND] = tt_AND;
            OpProcs[TT_OR] = tt_OR;
            OpProcs[TT_NOT] = tt_NOT;
            OpProcs[TT_DELTAP1] = tt_DELTAP1;
            OpProcs[TT_SDB] = tt_SDB;
            OpProcs[TT_SDS] = tt_SDS;
            OpProcs[TT_ADD] = tt_ADD;
            OpProcs[TT_SUB] = tt_SUB;
            OpProcs[TT_DIV] = tt_DIV;
            OpProcs[TT_MUL] = tt_MUL;
            OpProcs[TT_ABS] = tt_ABS;
            OpProcs[TT_NEG] = tt_NEG;
            OpProcs[TT_FLOOR] = tt_FLOOR;
            OpProcs[TT_CEILING] = tt_CEILING;
            OpProcs[TT_ROUND0] = tt_ROUND0;
            OpProcs[TT_ROUND1] = tt_ROUND1;
            OpProcs[TT_ROUND2] = tt_ROUND2;
            OpProcs[TT_ROUND3] = tt_ROUND3;
            OpProcs[TT_NROUND0] = tt_NROUND0;
            OpProcs[TT_NROUND1] = tt_NROUND1;
            OpProcs[TT_NROUND2] = tt_NROUND2;
            OpProcs[TT_NROUND3] = tt_NROUND3;
            OpProcs[TT_WCVTF] = tt_WCVTF;
            OpProcs[TT_DELTAP2] = tt_DELTAP2;
            OpProcs[TT_DELTAP3] = tt_DELTAP3;
            OpProcs[TT_DELTAC1] = tt_DELTAC1;
            OpProcs[TT_DELTAC2] = tt_DELTAC2;
            OpProcs[TT_DELTAC3] = tt_DELTAC3;
            OpProcs[TT_SROUND] = tt_SROUND;
            OpProcs[TT_S45ROUND] = tt_S45ROUND;
            OpProcs[TT_JROT] = tt_JROT;
            OpProcs[TT_JROF] = tt_JROF;
            OpProcs[TT_ROFF] = tt_ROFF;
            OpProcs[TT_RUTG] = tt_RUTG;
            OpProcs[TT_RDTG] = tt_RDTG;
            OpProcs[TT_SANGW] = tt_SANGW;
            OpProcs[TT_AA] = tt_AA;
            OpProcs[TT_FLIPPT] = tt_FLIPPT;
            OpProcs[TT_FLIPRGON] = tt_FLIPRGON;
            OpProcs[TT_FLIPRGOFF] = tt_FLIPRGOFF;
            OpProcs[TT_SCANCTRL] = tt_SCANCTRL;
            OpProcs[TT_SDPVTL0] = tt_SDPVTL0;
            OpProcs[TT_SDPVTL1] = tt_SDPVTL1;
            OpProcs[TT_GETINFO] = tt_GETINFO;
            OpProcs[TT_IDEF] = tt_IDEF;
            OpProcs[TT_ROLL] = tt_ROLL;
            OpProcs[TT_MAX] = tt_MAX;
            OpProcs[TT_MIN] = tt_MIN;
            OpProcs[TT_SCANTYPE] = tt_SCANTYPE;
            OpProcs[TT_INSTCTRL] = tt_INSTCTRL;
            OpProcs[TT_PUSHB1] = tt_PUSHB1;
            OpProcs[TT_PUSHB2] = tt_PUSHB2;
            OpProcs[TT_PUSHB3] = tt_PUSHB3;
            OpProcs[TT_PUSHB4] = tt_PUSHB4;
            OpProcs[TT_PUSHB5] = tt_PUSHB5;
            OpProcs[TT_PUSHB6] = tt_PUSHB6;
            OpProcs[TT_PUSHB7] = tt_PUSHB7;
            OpProcs[TT_PUSHB8] = tt_PUSHB8;
            OpProcs[TT_PUSHW1] = tt_PUSHW1;
            OpProcs[TT_PUSHW2] = tt_PUSHW2;
            OpProcs[TT_PUSHW3] = tt_PUSHW3;
            OpProcs[TT_PUSHW4] = tt_PUSHW4;
            OpProcs[TT_PUSHW5] = tt_PUSHW5;
            OpProcs[TT_PUSHW6] = tt_PUSHW6;
            OpProcs[TT_PUSHW7] = tt_PUSHW7;
            OpProcs[TT_PUSHW8] = tt_PUSHW8;
            OpProcs[TT_MDRP00] = tt_MDRP00;
            OpProcs[TT_MDRP01] = tt_MDRP01;
            OpProcs[TT_MDRP02] = tt_MDRP02;
            OpProcs[TT_MDRP03] = tt_MDRP03;
            OpProcs[TT_MDRP04] = tt_MDRP04;
            OpProcs[TT_MDRP05] = tt_MDRP05;
            OpProcs[TT_MDRP06] = tt_MDRP06;
            OpProcs[TT_MDRP07] = tt_MDRP07;
            OpProcs[TT_MDRP08] = tt_MDRP08;
            OpProcs[TT_MDRP09] = tt_MDRP09;
            OpProcs[TT_MDRP0A] = tt_MDRP0A;
            OpProcs[TT_MDRP0B] = tt_MDRP0B;
            OpProcs[TT_MDRP0C] = tt_MDRP0C;
            OpProcs[TT_MDRP0D] = tt_MDRP0D;
            OpProcs[TT_MDRP0E] = tt_MDRP0E;
            OpProcs[TT_MDRP0F] = tt_MDRP0F;
            OpProcs[TT_MDRP10] = tt_MDRP10;
            OpProcs[TT_MDRP11] = tt_MDRP11;
            OpProcs[TT_MDRP12] = tt_MDRP12;
            OpProcs[TT_MDRP13] = tt_MDRP13;
            OpProcs[TT_MDRP14] = tt_MDRP14;
            OpProcs[TT_MDRP15] = tt_MDRP15;
            OpProcs[TT_MDRP16] = tt_MDRP16;
            OpProcs[TT_MDRP17] = tt_MDRP17;
            OpProcs[TT_MDRP18] = tt_MDRP18;
            OpProcs[TT_MDRP19] = tt_MDRP19;
            OpProcs[TT_MDRP1A] = tt_MDRP1A;
            OpProcs[TT_MDRP1B] = tt_MDRP1B;
            OpProcs[TT_MDRP1C] = tt_MDRP1C;
            OpProcs[TT_MDRP1D] = tt_MDRP1D;
            OpProcs[TT_MDRP1E] = tt_MDRP1E;
            OpProcs[TT_MDRP1F] = tt_MDRP1F;
            OpProcs[TT_MIRP0] = tt_MIRP0;
            OpProcs[TT_MIRP1] = tt_MIRP1;
            OpProcs[TT_MIRP2] = tt_MIRP2;
            OpProcs[TT_MIRP3] = tt_MIRP3;
            OpProcs[TT_MIRP4] = tt_MIRP4;
            OpProcs[TT_MIRP5] = tt_MIRP5;
            OpProcs[TT_MIRP6] = tt_MIRP6;
            OpProcs[TT_MIRP7] = tt_MIRP7;
            OpProcs[TT_MIRP8] = tt_MIRP8;
            OpProcs[TT_MIRP9] = tt_MIRP9;
            OpProcs[TT_MIRPA] = tt_MIRPA;
            OpProcs[TT_MIRPB] = tt_MIRPB;
            OpProcs[TT_MIRPC] = tt_MIRPC;
            OpProcs[TT_MIRPD] = tt_MIRPD;
            OpProcs[TT_MIRPE] = tt_MIRPE;
            OpProcs[TT_MIRPF] = tt_MIRPF;
            OpProcs[TT_MIRP10] = tt_MIRP10;
            OpProcs[TT_MIRP11] = tt_MIRP11;
            OpProcs[TT_MIRP12] = tt_MIRP12;
            OpProcs[TT_MIRP13] = tt_MIRP13;
            OpProcs[TT_MIRP14] = tt_MIRP14;
            OpProcs[TT_MIRP15] = tt_MIRP15;
            OpProcs[TT_MIRP16] = tt_MIRP16;
            OpProcs[TT_MIRP17] = tt_MIRP17;
            OpProcs[TT_MIRP18] = tt_MIRP18;
            OpProcs[TT_MIRP19] = tt_MIRP19;
            OpProcs[TT_MIRP1A] = tt_MIRP1A;
            OpProcs[TT_MIRP1B] = tt_MIRP1B;
            OpProcs[TT_MIRP1C] = tt_MIRP1C;
            OpProcs[TT_MIRP1D] = tt_MIRP1D;
            OpProcs[TT_MIRP1E] = tt_MIRP1E;
            OpProcs[TT_MIRP1F] = tt_MIRP1F;

            OpProcs[0x28] = tt_Wrong;
            OpProcs[0x7B] = tt_Wrong;
            OpProcs[0x83] = tt_Wrong;
            OpProcs[0x84] = tt_Wrong;
            OpProcs[0x8F] = tt_Wrong;
            OpProcs[0x90] = tt_Wrong;
            OpProcs[0x91] = tt_Wrong;
            OpProcs[0x92] = tt_Wrong;
            OpProcs[0x93] = tt_Wrong;
            OpProcs[0x94] = tt_Wrong;
            OpProcs[0x95] = tt_Wrong;
            OpProcs[0x96] = tt_Wrong;
            OpProcs[0x97] = tt_Wrong;
            OpProcs[0x98] = tt_Wrong;
            OpProcs[0x99] = tt_Wrong;
            OpProcs[0x9A] = tt_Wrong;
            OpProcs[0x9B] = tt_Wrong;
            OpProcs[0x9C] = tt_Wrong;
            OpProcs[0x9D] = tt_Wrong;
            OpProcs[0x9E] = tt_Wrong;
            OpProcs[0x9F] = tt_Wrong;
            OpProcs[0xA0] = tt_Wrong;
            OpProcs[0xA1] = tt_Wrong;
            OpProcs[0xA2] = tt_Wrong;
            OpProcs[0xA3] = tt_Wrong;
            OpProcs[0xA4] = tt_Wrong;
            OpProcs[0xA5] = tt_Wrong;
            OpProcs[0xA6] = tt_Wrong;
            OpProcs[0xA7] = tt_Wrong;
            OpProcs[0xA8] = tt_Wrong;
            OpProcs[0xA9] = tt_Wrong;
            OpProcs[0xAA] = tt_Wrong;
            OpProcs[0xAB] = tt_Wrong;
            OpProcs[0xAC] = tt_Wrong;
            OpProcs[0xAD] = tt_Wrong;
            OpProcs[0xAE] = tt_Wrong;
            OpProcs[0xAF] = tt_Wrong;

        }
        CInstrCommand::~CInstrCommand()
        {
        }
    };

};

