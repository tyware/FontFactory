#pragma once

#include "math.h"
#include "FeHeader.h"

namespace FF_Font_Factory {

	class FONTENGINE_API CRealPoint;
	class FONTENGINE_API CRealVector
    {
    public:
        CRealVector();
        CRealVector(float iin, float jin);
        CRealVector(const CRealPoint &p0, const CRealPoint &p1);

        float I()const;
        float J()const;
        void Set(float iin, float jin);
        float Length();
        double DblLength();
        void SetLength(float newlength);
        float Angle();
        double DblAngle();
        void SetAngle(float angle);
        void Rotate(float angle);
        CRealVector &operator = (const CRealVector &p);

        CRealVector operator + (const CRealVector &p);
        CRealVector operator - (const CRealVector &p);

        CRealVector operator * (float d);
        bool operator == (const CRealVector &p);
        bool operator != (const CRealVector &p);

        float CrossProduct(const CRealVector &other);

    protected:
        float i;
        float j;
        float length;
        bool dirty;
    }; //CRealVector

	class FONTENGINE_API CRealPoint
	{
	public:
		CRealPoint();
		CRealPoint(float xin, float yin);

		void AddX(float inx);
		void AddY(float iny);
        void Set(float xin, float yin);

        float DistanceTo(const CRealPoint & pt) const;

        bool operator == (const CRealPoint &other) const;
        bool operator != (const CRealPoint &other) const;
        CRealPoint &operator = (const CRealPoint &p);
        void operator += (const CRealPoint &p);
        void operator -= (const CRealPoint &p);
        CRealPoint operator + (const CRealPoint &p) const;
        CRealPoint operator - (const CRealPoint &p) const;
        void operator += (const CRealVector &p);
        CRealPoint operator + (const CRealVector &p) const;
        CRealPoint operator - (const CRealVector &p) const;
        CRealPoint operator * (float f) const;
        CRealPoint operator / (float f) const;
        void operator *= (float &f);
        void operator /= (float &f);

	public:
        float x;
        float y;
	};

    class FONTENGINE_API CPointList
    {
    public:
        CPointList();
        CPointList(const CPointList &otherList);
        virtual ~CPointList();
		void CopyFrom(CPointList* pts);

        void SetPoint(int index, const CRealPoint &point);
        void SetPoint(int index, CRealPoint *pPoint);
        void AddPoint(const CRealPoint &point);

        void AddPoint(const CRealPoint *pPoint);
        void PopLastPoint(void);

        CRealPoint *GetPoints(void);
        int GetPointCount(void) const { return m_pointcount; }

        void Pfloatloc(int numpoints);

        void Clear(void)
        {
            m_pointcount = 0;
        }

        bool operator < (const CPointList& /*t*/) const
        {
            return false;
        }
        bool operator == (const CPointList& /*t*/) const
        {
            return false;
        }

        CRealPoint &operator [] (int index) const;

        bool DoesSelfIntersect(bool bClosedShape);

        double Area(void); // assumes the pointlist is a closed shape

    protected:
        void Resize(int newpointcount);

        bool DoEdgesIntersect(CRealPoint *pEdge1Begin,
                            CRealPoint *pEdge1End,
                            CRealPoint *pEdge2Begin,
                            CRealPoint *pEdge2End);

        CRealPoint *m_pPoints;
        int           m_pointcount;
        int           m_pointsallocated;
    }; //CPointList

};
