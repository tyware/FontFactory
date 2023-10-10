#include "stdafx.h"

#include "RealPoint.h"
#include "MathMacro.h"
#include "assert.h"

namespace FF_Font_Factory {

	///////////////////////////////////////
	// CRealVector
	///////////////////////////////////////
    CRealVector::CRealVector()
	{
        length = NULL; i = 0; j = 0; dirty = true;
    }

    CRealVector::CRealVector(float iin, float jin){
        length = NULL; i = iin; j = jin; dirty = true;
    }

    CRealVector::CRealVector(const CRealPoint &p0, const CRealPoint &p1)
	{
	   i = p1.x - p0.x; 
	   j = p1.y - p0.y;
	   dirty = true;
	}

    float CRealVector::I()const{ return i; }
    float CRealVector::J()const{ return j; }

    void CRealVector::Set(float iin, float jin)
    {
        i = iin; j = jin;
        dirty = true;
    }

    float CRealVector::Length()
    {
        if (dirty)
        {
            length = (float)sqrt((i * i) + (j * j));
            dirty = false;
        }
        return length;
    }

    double CRealVector::DblLength()
    {
        return sqrt(((double)i * (double)i) + ((double)j * (double)j));
    }

    void CRealVector::SetLength(float newlength)
	{
	   float oldlength = Length();
	   float scaler;
	   if(FEqual(oldlength,0.0))
	   {
		  i = newlength;
		  j = 0.0;
	   }
	   else
	   {
		  scaler = newlength / oldlength;
		  i *= scaler;
		  j *= scaler;
	   }   
	   dirty = true;
	}

    float CRealVector::Angle()
    {
        return (float)atan2(j, i);
    }

    double CRealVector::DblAngle()
    {
        return atan2((double)j, (double)i);
    }

    void CRealVector::SetAngle(float angle)
    {
        float length = Length();
        i = (float)cos(angle) * length;
        j = (float)sin(angle) * length;
        dirty = true;
    }

    void CRealVector::Rotate(float angle)
    {
        SetAngle(Angle() + angle);
        dirty = true;
    }

    CRealVector &CRealVector::operator = (const CRealVector &p)
    {
        i = p.I();
        j = p.J();
        dirty = true;
        return (*this);
    }

    CRealVector CRealVector::operator + (const CRealVector &p)
    {
        return CRealVector(i + p.I(), j + p.J());
    }

    CRealVector CRealVector::operator - (const CRealVector &p)
    {
        return CRealVector(i - p.I(), j - p.J());
    }

    CRealVector CRealVector::operator * (float d)
    {
        CRealVector result = *this;

        float length = Length();
        result.SetLength(length * d);
        return result;
    }

    bool CRealVector::operator == (const CRealVector &p)
    {
        return(i == p.i);
    }

    bool CRealVector::operator != (const CRealVector &p)
    {
        return(i != p.i);
    }

    float CRealVector::CrossProduct(const CRealVector &other)
	{
	   return I() * other.J() - other.I() * J();   
	}

	///////////////////////////////////////
	// CRealVector
	///////////////////////////////////////
	CRealPoint::CRealPoint(){ x = 0; y = 0; }
	CRealPoint::CRealPoint(float xin, float yin) : x(xin), y(yin){};

	void CRealPoint::AddX(float inx) { x += inx;};
	void CRealPoint::AddY(float iny) { y += iny;};
    void CRealPoint::Set(float xin, float yin){ x = xin; y = yin; }

    float CRealPoint::DistanceTo(const CRealPoint & pt) const
    {
        return (float)(sqrt(pow(x - pt.x, 2) + pow(y - pt.y, 2)));
    }

    bool CRealPoint::operator == (const CRealPoint &other) const
    {
        if (FEqual(x, other.x) && FEqual(y, other.y)) return true;
        return false;
    }
    bool CRealPoint::operator != (const CRealPoint &other) const
    {
        return(!(*this == other));
    }
    CRealPoint &CRealPoint::operator = (const CRealPoint &p)
    {
        CRealPoint *pp = (CRealPoint *)&p;
        x = pp->x;
        y = pp->y;
        return (*this);
    }
    void CRealPoint::operator += (const CRealPoint &p)
    {
        x = x += p.x;
        y = y += p.y;
    }
    void CRealPoint::operator -= (const CRealPoint &p)
    {
        x = x -= p.x;
        y = y -= p.y;
    }
    CRealPoint CRealPoint::operator + (const CRealPoint &p) const
    {
        CRealPoint pt;
        pt.Set(x + p.x, y + p.y);
        return (pt);
    }
    CRealPoint CRealPoint::operator - (const CRealPoint &p) const
    {
        CRealPoint pt;
        pt.Set(x - p.x, y - p.y);
        return (pt);
    }
    void CRealPoint::operator += (const CRealVector &p)
    {
        x = x += p.I();
        y = y += p.J();
    }
    CRealPoint CRealPoint::operator + (const CRealVector &p) const
    {
        CRealPoint pt;
        pt.Set(x + p.I(), y + p.J());
        return (pt);
    }
    CRealPoint CRealPoint::operator - (const CRealVector &p) const
    {
        CRealPoint pt;
        pt.Set(x - p.I(), y - p.J());
        return (pt);
    }
    CRealPoint CRealPoint::operator * (float f) const
    {
        CRealPoint pt;
        pt.Set(x * f, y * f);
        return (pt);
    }
    CRealPoint CRealPoint::operator / (float f) const
    {
        CRealPoint pt;
        pt.Set(x / f, y / f);
        return (pt);
    }
    void CRealPoint::operator *= (float &f)
    {
        x = x *= f;
        y = y *= f;
    }
    void CRealPoint::operator /= (float &f)
    {
        x = x /= f;
        y = y /= f;
    }

    /////////////////////////////////////////////////
    // CPointList
    /////////////////////////////////////////////////

    CPointList::CPointList()
    {
        m_pPoints = NULL;
        m_pointcount = 0;
        m_pointsallocated = 0;
    }

    CPointList::CPointList(const CPointList &otherList)
    {
        m_pPoints = NULL;
        m_pointcount = 0;
        m_pointsallocated = 0;

        Resize(otherList.GetPointCount());

        m_pointcount = otherList.GetPointCount();

        for (int i = 0; i < m_pointcount; i++)
        {
            m_pPoints[i] = otherList[i];
        }
    }

    CPointList::~CPointList()
    {
        if (m_pPoints != NULL)
        {
            delete[] m_pPoints;
        }
    }

	void CPointList::CopyFrom(CPointList* pts)
	{
		if (pts)
		{
			m_pPoints = new CRealPoint[pts->m_pointsallocated];
			m_pointcount = pts->m_pointcount;
			memcpy(m_pPoints, pts->m_pPoints, pts->m_pointcount * sizeof(CRealPoint));
		}
	}

    void CPointList::AddPoint(const CRealPoint &point)
    {
        AddPoint(&point);
    };

    CRealPoint *CPointList::GetPoints(void)
	{ 
		return m_pPoints;
	}

    void CPointList::SetPoint(int index, const CRealPoint &point)
    {
        m_pPoints[index] = point;
    }

    void CPointList::SetPoint(int index, CRealPoint *pPoint)
    {
        m_pPoints[index] = *pPoint;
    }

    void CPointList::AddPoint(const CRealPoint *pPoint)
    {
        if (m_pointcount + 1 > m_pointsallocated)
        {
            //@@@@@@@@@@@@@@@@@@@@
            // Simon: 400 is too much
            //Resize(m_pointcount + 400);
            Resize(m_pointcount + 100);
        }
        assert(m_pPoints != NULL);
        m_pPoints[m_pointcount] = *pPoint;
        m_pointcount++;
    }

    void CPointList::PopLastPoint(void)
    {
        m_pointcount--;
    }

    void CPointList::Pfloatloc(int numpoints)
    {
        Resize(numpoints);
        m_pointcount = numpoints;
    }

    CRealPoint &CPointList::operator [] (int index) const
    {
        return m_pPoints[index];
    }

    void CPointList::Resize(int newpointcount)
    {
        if (newpointcount > 0)
        {
            if (newpointcount > m_pointsallocated)
            {
                CRealPoint *pPreviousPoints = m_pPoints;
                CRealPoint *pNewPoints = new CRealPoint[newpointcount];

                if (pPreviousPoints != NULL)
                {
                    for (int i = 0; i < m_pointcount; i++)
                    {
                        pNewPoints[i] = pPreviousPoints[i];
                    }
                }

                delete[] m_pPoints;
                m_pPoints = pNewPoints;

                m_pointsallocated = newpointcount;
            }
            else
            {
                m_pointcount = newpointcount;
            }
        }
        else
        {
            Clear();
        }
    }


    bool CPointList::DoesSelfIntersect(bool bClosedShape)
    {
        bool result = false;
        int i, j;

        if ((m_pPoints != NULL) && (m_pointcount > 2))
        {
            for (j = 0; j < m_pointcount; j++)
            {
                int begin = j;
                int end = (j + 1) % m_pointcount;

                /* Make sure the last two points are not coincident */
                if (!(FEqual(m_pPoints[begin].x, m_pPoints[end].x) &&
                    FEqual(m_pPoints[begin].y, m_pPoints[end].y)))
                {
                    // See if the last line segment intersects any of the other line segments.
                    for (i = 0; i < begin; i++)
                    {
                        if (DoEdgesIntersect(&m_pPoints[begin],
                            &m_pPoints[end],
                            &m_pPoints[i],
                            &m_pPoints[i + 1]))
                        {
                            result = true;
                            break;
                        }
                    }

                    // Make sure the segment from the first point to the last point does not intersect anything
                    if (bClosedShape)
                    {
                        for (i = 0; i < (m_pointcount - 2); i++)
                        {
                            if (DoEdgesIntersect(&m_pPoints[m_pointcount - 1],
                                &m_pPoints[0],
                                &m_pPoints[i],
                                &m_pPoints[i + 1]))
                            {
                                result = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        return result;
    }


    bool CPointList::DoEdgesIntersect(CRealPoint *pEdge1Begin,
        CRealPoint *pEdge1End,
        CRealPoint *pEdge2Begin,
        CRealPoint *pEdge2End)
    {
        bool result = false;

        /*
        The equasions of the components of the point of intersection between linea and lineb
        in parameterized form are:

        x = x1 + ua (x2 - x1)
        y = y1 + ua (y2 - y1)

        where linea consists of points 1 and 2, and lineb consists of points 3 and 4

        and where ua and ub are given by:

        (x4 - x3)(y1 - y3) - (y4 - y3)(x1 - x3)
        ua  =  ---------------------------------------
        (y4 - y3)(x2 - x1) - (x4 - x3)(y2 - y1)

        (x2 - x1)(y1 - y3) - (y2 - y1)(x1 - x3)
        ub  =  ---------------------------------------
        (y4 - y3)(x2 - x1) - (x4 - x3)(y2 - y1)

        Note that only ua is needed to substitute into the two primary equasions to retrieve
        the intersection point.

        Now, for our purposes, we only need to know if the edges intersect. We are not
        conserned with the actual intersection point. Therefore, we can simply compute
        ua and ub, and if both of these parameter values are between 0 and 1, then
        the intersection point lies within both line segments (i.e. they intersect).
        Note also that if only one of the parameter values ua or ub is between 0 and 1,
        then that line segment intersects the other line segment's vector, but does not
        intersect the segment itself.
        */

        if (pEdge1Begin != NULL &&
            pEdge1End != NULL &&
            pEdge2Begin != NULL &&
            pEdge2End != NULL)
        {
            /* We do not want to consider edges to intersect if they share endpoints */
            if (*pEdge1Begin != *pEdge2Begin &&
                *pEdge1Begin != *pEdge2End &&
                *pEdge1End != *pEdge2Begin &&
                *pEdge1End != *pEdge2End)
            {
                double ua = 0;
                double ub = 0;
                double divisor = 0;

                divisor = ((((double)pEdge2End->y - (double)pEdge2Begin->y) * ((double)pEdge1End->x - (double)pEdge1Begin->x)) - (((double)pEdge2End->x - (double)pEdge2Begin->x) * ((double)pEdge1End->y - (double)pEdge1Begin->y)));

                if (!FEqual(divisor, 0.0))
                {
                    ua = ((((double)pEdge2End->x - (double)pEdge2Begin->x) * ((double)pEdge1Begin->y - (double)pEdge2Begin->y)) - (((double)pEdge2End->y - (double)pEdge2Begin->y) * ((double)pEdge1Begin->x - (double)pEdge2Begin->x))) / divisor;

                    ub = ((((double)pEdge1End->x - (double)pEdge1Begin->x) * ((double)pEdge1Begin->y - (double)pEdge2Begin->y)) - (((double)pEdge1End->y - (double)pEdge1Begin->y) * ((double)pEdge1Begin->x - (double)pEdge2Begin->x))) / divisor;

                    if (((ua > 0.0) && (ua < 1.0)) && ((ub > 0.0) && (ub < 1.0)))
                    {
                        result = true;
                    }
                }
            }
        }

        return result;
    }


    double CPointList::Area(void)
    {
        double result = 0.0;

        if (m_pPoints != NULL &&
            m_pointcount > 2 &&
            !DoesSelfIntersect(true))// assumes a closed shape
        {
            /* Calculate the area */
            for (int i = 0; i < m_pointcount; i++)
            {
                int j = (i + 1) % m_pointcount;

                result += ((double)m_pPoints[i].x * (double)m_pPoints[j].y) / 20.0;
                result -= ((double)m_pPoints[i].y * (double)m_pPoints[j].x) / 20.0;
            }

            /* The summation above computed twice the area */
            //result /= 2.0;
            result *= 10.0;

            /* Make sure the area is positive */
            if (result < 0)
            {
                result = -result;
            }
        }

        return result;
    }

};
