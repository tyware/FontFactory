#pragma once

#include "math.h"
#include <algorithm>

//namespace Model {

#undef PI
#define  PI                      ((double)3.141592653589793238462643383279)

#undef PI2
#define  PI2                     ((double)(2.0*PI))   

#if !defined(DEGREES360)
#define DEGREES360               ((double)360.0)
#endif

#if !defined(DEG2RAD)
#define  DEG2RAD(a)              (((double)(a))*(PI/(float)180.0))
#endif

#if !defined(RAD2DEG)
#define  RAD2DEG(a)              (((double)(a))*((double)180.0/PI))
#endif

#if !defined(EPSILON)
#define  EPSILON                 ((double)0.000001)
#endif

#if !defined(BSfloatMAX)
#define  floatMAX               ((double)3.402823466e+38)
#endif

	inline bool FEqual(double x, double y)
	{
		return (fabs((x)-(y)) < EPSILON);
	}

	inline bool FEqualEpsilon(double x, double y, double epsilon)
	{
		return (fabs((x)-(y)) < epsilon);
	}

	inline bool FZero(double x)
	{
		return (fabs(x) < EPSILON);
	}

	inline double FSaneAngleRad(double x)
	{
		return (x < (float)0.0)
			? (PI2 + fmod((x + EPSILON), PI2) - EPSILON)
			: (fmod((x + EPSILON), PI2) - EPSILON);
	}

	inline double FSaneAngleDeg(double x)
	{
		return (x < 0.0)
			? DEGREES360 + fmod((x + EPSILON), DEGREES360) - EPSILON
			: fmod((x + EPSILON), DEGREES360) - EPSILON;
	}

	inline double FAngleRadDiff(double angleA, double angleB)
	{
		double absrad;
		angleA = FSaneAngleRad(angleA);
		angleB = FSaneAngleRad(angleB);

		if (angleA >= 0.0 && angleA < PI)
		{
			absrad = fabs(angleB - angleA);
			if (angleB >= PI)
			{
				angleB -= PI2;
				absrad = __min(fabs(angleB - angleA), absrad);
			}
		}
		else
		{
			absrad = fabs(angleB - angleA);
			if (angleB < PI)
			{
				angleB += PI2;
				absrad = __min(fabs(angleB - angleA), absrad);
			}
		}

		return absrad;
	}

//};
