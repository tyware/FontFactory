#pragma once

#include <algorithm>
#include "MathMacro.h"
#include "RealPoint.h"
#include <vector>
#include "FeHeader.h"

namespace FF_Font_Factory {

	class FONTENGINE_API CMatrix3x3;

    // *ProtoComment*
    // Multiply the inverse of each matrix.

    FONTENGINE_API CMatrix3x3 MatInvMult(const CMatrix3x3 &matrix0, const CMatrix3x3 &matrix1);

    // *ProtoComment*
    // Multiplication operator for 3x3 matrices

    FONTENGINE_API CMatrix3x3 operator * (const CMatrix3x3 &matrix0, const CMatrix3x3 &matrix1);

    FONTENGINE_API CMatrix3x3 operator * (const CMatrix3x3 &matrix, float s);

    // *ProtoComment*
    // Scalar multiplication operator for 3x3 matrices

    FONTENGINE_API CMatrix3x3 operator * (float s, const CMatrix3x3 &matrix);


    // *ProtoComment*
    // Addition operator for 3x3 matrices

    FONTENGINE_API CMatrix3x3 operator + (const CMatrix3x3 &matrix0, const CMatrix3x3 &matrix1);

    // *ProtoComment*
    // Subtraction operator for 3x3 matrices

    FONTENGINE_API CMatrix3x3 operator - (const CMatrix3x3 &matrix0, const CMatrix3x3 &matrix1);

    // *ProtoComment*
    // Multiplies a point by a matrix and returns a point.

    FONTENGINE_API CRealPoint operator * (const CRealPoint &p, const CMatrix3x3 &m);

    // *ProtoComment*
    // Multiplies a vector by a matrix and returns a vector. Note that the
    // translation portion of the matrix is ignored.

    FONTENGINE_API CRealVector operator * (const CRealVector &v, const CMatrix3x3 &matrix);

    // *ProtoComment*
    // Builds a transformation matrix composed of rotations, translations,
    // and scalings. It is assumed that the rotations are given in radians.
    //
    // Although the result of the function is one 3x3 matrix, the result
    // is actually composed of five matrices in the following order.
    //
    //   $M_{t} * M_{s} * M_{x} * M_{y} * M_{z}$
    //
    // where $M_{t}$ is a translation matrix, $M_{s}$ is a scaling matrix, and
    // $M_{x}$, $M_{y}$, and $M_{z}$ are the rotation matrices about the
    // x-, y-, and z-axis respectively.

    FONTENGINE_API CMatrix3x3 makeXForm(double r = 0.0,
										double tx = 0.0, double ty = 0.0,
										double sx = 1.0, double sy = 1.0);

    FONTENGINE_API double det2x2(double a00, double a01, double a10, double a11);

    FONTENGINE_API CRealPoint Interpolate(CRealPoint &p0, CRealPoint &p1, float t);
    FONTENGINE_API void Interpolate(CRealPoint &p0, CRealPoint &p1, float t, CRealPoint &result);
    FONTENGINE_API void InterpolateCubicBezier(CRealPoint *pPoints, float t, CRealPoint &result);
    FONTENGINE_API bool PointsAreCollinear(const CRealPoint &point1, const CRealPoint &point2, const CRealPoint &point3);
    FONTENGINE_API void ClosestPointOnLineSegment(const CRealPoint & a, const CRealPoint & b, const CRealPoint & p1, CRealPoint &closest);
    FONTENGINE_API float ShortestDistance(const CRealPoint &b0, const CRealPoint &bn, const CRealPoint &mid);
    FONTENGINE_API float AbsAngleDiff(CRealVector &v1, CRealVector &v2);
    FONTENGINE_API bool PerpIntersect(CRealPoint &line1, CRealPoint &line2, CRealPoint &p, CRealPoint &answer);
    FONTENGINE_API void RemoveConsecutiveCoincidentalPoints(CRealPoint *pPoints, size_t numPoints, std::vector<CRealPoint> &cleaned);

    FONTENGINE_API float DotProduct(const CRealVector &v0, const CRealVector &v1);

    FONTENGINE_API bool Compute3PointCircleCenter(CRealPoint &point1, CRealPoint &point2, CRealPoint &point3, CRealPoint &centerPoint);

    FONTENGINE_API bool PointInsideConvex(CRealPoint &testpoint, CRealPoint *points, size_t numpoints);

    FONTENGINE_API void OffsetPoints(const CRealPoint *inPoints, CRealPoint *outPoints, int size, float offset);

	////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
    template <class T>
    void matSwap(T &a, T &b)
    {
        T tmp(a);
        a = b;
        b = tmp;
    }

    class FONTENGINE_API CMatrix3x3
    {

    public:

        // *MemberComment*
        // Default constructor. Does not initialize the elements.

        CMatrix3x3();

        // *MemberComment*
        // Constructor. Sets all elements in the matrix to the specified value.

        CMatrix3x3(double value);

        // *MemberComment*
        // Copy constructor.

        CMatrix3x3(const CMatrix3x3 &rhs);

        // *MemberComment*
        // Assignment operator.

        CMatrix3x3 & operator = (const CMatrix3x3 &rhs);

        // *MemberComment*
        // Equality operator.

        bool operator == (const CMatrix3x3 &other) const;

        // *MemberComment*
        // Inverts the matrix provided that the matrix is nonsingular.

        bool Invert(void);

        bool InvertMat();

        // *MemberComment*
        // Inverts the matrix assuming the matrix is orthogonal.

        void InvertOrthogonal(void);

        // *MemberComment*
        // Return the determinant of the matrix.

        double Determinant(void);

        // *MemberComment*
        // Transposes the matrix.

        void Transpose(void);

        // *MemberComment*
        // Set the matrix to an {\tt Identity} matrix.

        void SetIdentity(void);

        bool IsIdentity(void);

        bool SetInvert(void);

        void Set(float m00, float m01,
				float m10, float m11,
				float m20, float m21);

        void Get(float &m00, float &m01,
				float &m10, float &m11,
				float &m20, float &m21) const;

        bool IsOrthogonal(void);

        float GetRotation() const;

        float ScaleValue(float value) const;

        float RotateValue(float value, bool bFlipDeviceY = false) const;


        void FindAdjoint(double adjoint[3][3]);

        void operator += (CRealVector &p);

        // *MemberComment*
        // Overloaded function call operator. Returns the element at the
        // specified indices. No checking of indices is performed.

        double &operator () (int i, int j);

        // *MemberComment*
        // Overloaded function call operator for const objects. Returns the
        // element at the specified indices. No checking of indices is performed.

        const double &operator () (int i, int j) const;

        void MakeXForm(double r = 0.0,
						double tx = 0.0, double ty = 0.0,
						double sx = 1.0, double sy = 1.0);

        void ExpandToIntegerSize(double width, double height);

        // *VarComment*
        // The elements of the matrix.

        double mat[3][3];

        // *VarComment*
        // The inverse of the matrix (if it exists)

        double inv[3][3];

        void SetDirty(void);
        void SetClean(void);

        mutable bool m_bHaveCalculatedScale;
        mutable bool m_bHaveCalculatedRotation;
        mutable double m_rotation;
        mutable double m_scale;
    }; //CMatrix3x3

};

