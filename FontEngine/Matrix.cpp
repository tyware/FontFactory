#include "stdafx.h"

//#include "Model.h"
#include "Matrix.h"
#include "MathMacro.h"

namespace FF_Font_Factory {

	////////////////////////////////////////
	// global functions
	////////////////////////////////////////
    CRealPoint Interpolate(CRealPoint &p0, CRealPoint &p1,float t)
    {
       float x = t * p1.x + (1-t) * p0.x;
       float y = t * p1.y + (1-t) * p0.y;

       return(CRealPoint(x,y));     
    }

    void Interpolate(CRealPoint &p0, CRealPoint &p1,float t,CRealPoint &result)
    {
       result.x = (t * p1.x + (1-t) * p0.x);
       result.y = (t * p1.y + (1-t) * p0.y);   
    }

    void InterpolateCubicBezier(CRealPoint *pPoints, float t, CRealPoint &result)
    {
       //assert(t >=0 && t <= 1);

       float x =   (pPoints[3].x - 3 * pPoints[2].x + 3 * pPoints[1].x - pPoints[0].x) * ::pow(t, 3)  
                    + (3 * pPoints[2].x - 6 * pPoints[1].x + 3 * pPoints[0].x) * pow(t, 2)  
                    + (3 *pPoints[1].x - 3 * pPoints[0].x) * t  
                    + (pPoints[0].x);

       float y =   (pPoints[3].y - 3 * pPoints[2].y + 3 * pPoints[1].y - pPoints[0].y) * ::pow(t, 3)  
                    + (3 * pPoints[2].y - 6 * pPoints[1].y + 3 * pPoints[0].y) * pow(t, 2)  
                    + (3 *pPoints[1].y - 3 * pPoints[0].y) * t  
                    + (pPoints[0].y);

       result.Set(x,y);
    }

	CMatrix3x3 MatInvMult (const CMatrix3x3 &a, const CMatrix3x3 &b)
	{
	  CMatrix3x3 product;

	  int i,j,k;
	  for (i = 0; i < 3; i++)
	  {
		for (j = 0; j < 3; j++)
		{
		  product.mat[i][j] = a.inv[i][0] * b.inv[0][j];

		  for (k = 1; k < 3; k++)
		  {
			product.mat[i][j] += a.inv[i][k] * b.inv[k][j];
		  }
		}
	  }

	  return(product);
	}

	CMatrix3x3 operator * (const CMatrix3x3 &a, const CMatrix3x3 &b)
	{
	  CMatrix3x3 product;

	  int i,j,k;
	  for (i = 0; i < 3; i++)
	  {
		for (j = 0; j < 3; j++)
		{
		  product.mat[i][j] = a.mat[i][0] * b.mat[0][j];

		  for (k = 1; k < 3; k++)
		  {
			product.mat[i][j] += a.mat[i][k] * b.mat[k][j];
		  }
		}
	  }

	  return(product);
	}


	CMatrix3x3 operator + (const CMatrix3x3 &a, const CMatrix3x3 &b)
	{
	  CMatrix3x3 sum;

	  int i,j;
	  for (i = 0; i < 3; i++)
	  {
		for (j = 0; j < 3; j++)
		{
		  sum.mat[i][j] = a.mat[i][j] + b.mat[i][j];
		}
	  }

	  return(sum);
	}


	CMatrix3x3 operator - (const CMatrix3x3 &a, const CMatrix3x3 &b)
	{
	  CMatrix3x3 diff;

	  int i,j;
	  for (i = 0; i < 3; i++)
	  {
		for (j = 0; j < 3; j++)
		{
		  diff.mat[i][j] = a.mat[i][j] - b.mat[i][j];
		}
	  }

	  return(diff);
	}


	CMatrix3x3 operator * (const CMatrix3x3 &a, float s)
	{
	  CMatrix3x3 tmp;

	  int i,j;
	  for (i = 0; i < 3; i++)
	  {
		for (j = 0; j < 3; j++)
		{
		  tmp.mat[i][j] = s * a.mat[i][j];
		}
	  }

	  return(tmp);
	}

	CMatrix3x3 operator * (float s, const CMatrix3x3 &a)
	{
	  CMatrix3x3 tmp;

	  int i,j;
	  for (i = 0; i < 3; i++)
	  {
		for (j = 0; j < 3; j++)
		{
		  tmp.mat[i][j] = s * a.mat[i][j];
		}
	  }

	  return(tmp);
	}

	double det2x2 (double a00, double a01, double a10, double a11)
	{
	  return(a00 * a11 - a01 * a10);
	}

	CRealVector operator * (const CRealVector &p, const CMatrix3x3 &m)
	{
	  return(CRealVector(
				(float)(p.I() * m(0, 0) + p.J() * m(1, 0)),
				(float)(p.I() * m(0, 1) + p.J() * m(1, 1)))
		);
	}


	CRealPoint operator * (const CRealPoint &p, const CMatrix3x3 &m)
	{
		return(CRealPoint(
			  (float)(p.x * m(0, 0) + p.y * m(1, 0) + m(2, 0)),
			  (float)(p.x * m(0, 1) + p.y * m(1, 1) + m(2, 1)))
		);
	}


	CMatrix3x3 makeXForm (double rot,
							 double tx, double ty,
							 double sx, double sy)
	{
	  CMatrix3x3 trans;
	  trans.SetIdentity();

	  trans(2, 0) = tx;
	  trans(2, 1) = ty;

	  CMatrix3x3 scale;
	  scale.SetIdentity();

	  scale(0, 0) = sx;
	  scale(1, 1) = sy;

	  CMatrix3x3 rotate;
	  rotate.SetIdentity();

	  rotate(0, 0) = cos(rot);
	  rotate(0, 1) = sin(rot);
	  rotate(1, 0) = -sin(rot);
	  rotate(1, 1) = cos(rot);

	  //CMatrix3x3 result = trans;
	  //result = result * rotate;
	  //result = result * scale;
      
      // I like scale first, trans last
      CMatrix3x3 result = scale;
      result = result * rotate;
      result = result * trans;

	  return result;
	}

	//////////////////////////////////////////////////////////////////////////////
	// CMatrix3x3
	//////////////////////////////////////////////////////////////////////////////
	CMatrix3x3::CMatrix3x3 (double value)
	{
	   int i,j;
	   for (i = 0; i < 3; i++)
	   {
		  for (j = 0; j < 3; j++)
		  {
			 mat[i][j] = value;
		  }
	   }
	   SetDirty();
	}

	CMatrix3x3::CMatrix3x3 (const CMatrix3x3 &m)
	{   
	   memcpy(mat,m.mat,sizeof(mat));
	   /*for (int i = 0; i < 3; i++)
	   {
		  for (int j = 0; j < 3; j++)
		  {
			 mat[i][j] = m.mat[i][j];
		  }
	   }*/
	   SetDirty();
	}

	CMatrix3x3::CMatrix3x3 ()
	{
		SetIdentity();
   
		SetDirty();
	}

    bool CMatrix3x3::operator == (const CMatrix3x3 &other) const
    {
        int diff = 0;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (!FEqual(mat[i][j], other.mat[i][j]))
                {
                    diff++;
					return false;
                }
            }
        }
        if (diff > 0)return false;
        return true;
    }

    bool CMatrix3x3::IsOrthogonal(void)
    {
        // the 1000.0 stuff is to make up for some 
        // precision errors in 4byte floating point calculations.
        CRealVector  vec(1000.0, 0);

        vec = vec * ((CMatrix3x3)*this);

        double angle = vec.DblAngle();
        if (FEqual(angle, 0.0) ||
            FEqual(angle, PI / 2.0) ||
            FEqual(angle, PI) ||
            FEqual(angle, PI * 2.0) ||
            FEqual(angle, -(PI / 2.0)) ||
            FEqual(angle, -(PI)))
        {
            return true;
        }

        return false;
    }

    float CMatrix3x3::GetRotation() const
    {
        // the 1000.0 stuff is to make up for some 
        // precision errors in 4byte floating point calculations.
        CRealVector  vec(1000.0, 0);

        vec = vec * ((CMatrix3x3)*this);

        return vec.Angle();
    }

    float CMatrix3x3::ScaleValue(float value) const
    {
        if (FEqual(value, 0.0))
        {
            return 0.0;
        }
        else
        {
            if (!m_bHaveCalculatedScale)
            {
                // the 1000.0 stuff added into equations and 
                // then later pulled out is to make up for some 
                // precision errors in 4byte floating point calculations.
                float floatTmp = (float)(1000.0f * (1.0f / (float)sqrt(2.0f)));
                CRealVector tmp(floatTmp, floatTmp);
                tmp = tmp * *this;
                m_scale = (float)(tmp.Length() / 1000.0f);
                m_bHaveCalculatedScale = true;
            }
            return (float)(m_scale * value);
        }
    }

    float CMatrix3x3::RotateValue(float value, bool bFlipDeviceY ) const
    {
        if (!m_bHaveCalculatedRotation)
        {
            // the 1000.0 stuff added into equations and 
            // then later pulled out is to make up for some 
            // precision errors in 4byte floating point calculations.
            CRealPoint tmp1(1000.0, 0.0);
            CRealPoint tmp2(0.0, 0.0);
            tmp1 = tmp1 * *this;
            tmp2 = tmp2 * *this;

            float x = (float)((tmp1.x - tmp2.x) / 1000.0);
            float y = (float)((tmp1.y - tmp2.y) / 1000.0);

            if (bFlipDeviceY)
            {
                y = -y;
            }

            float radius = (float)_hypot(fabs(x), fabs(y));

            if (FZero(x))
            {
                if (y > 0)
                {
                    m_rotation = PI / 2.0f;
                }
                else
                {
                    m_rotation = 3 * PI / 2.0f;
                }
            }
            else if (FZero(y))
            {
                if (x > 0)
                {
                    m_rotation = 0.0;
                }
                else
                {
                    m_rotation = PI;
                }
            }
            else
            {
                m_rotation = (float)atan2(y / radius, x / radius);

                if (m_rotation < 0)
                {
                    m_rotation += 2 * PI;
                }
            }

            m_bHaveCalculatedRotation = true;
        }

        return (float)(m_rotation + value);
    }

    void CMatrix3x3::operator += (CRealVector &p)
    {
        mat[2][0] += p.I();
        mat[2][1] += p.J();
        SetDirty();
    }

    double &CMatrix3x3::operator () (int i, int j)
    {
        return(mat[i][j]);
    };

    const double &CMatrix3x3::operator () (int i, int j) const
    {
        return(mat[i][j]);
    };

    void CMatrix3x3::SetDirty(void)
	{
		m_bHaveCalculatedScale = m_bHaveCalculatedRotation = false;
	}
    void CMatrix3x3::SetClean(void)
	{
		m_bHaveCalculatedScale = m_bHaveCalculatedRotation = true;
	}

	void CMatrix3x3::FindAdjoint(double adjoint[3][3])
	{
	  adjoint[0][0] =  det2x2(mat[1][1], mat[2][1], mat[1][2], mat[2][2]);
	  adjoint[0][1] = -det2x2(mat[1][0], mat[2][0], mat[1][2], mat[2][2]);
	  adjoint[0][2] =  det2x2(mat[1][0], mat[2][0], mat[1][1], mat[2][1]);

	  adjoint[1][0] = -det2x2(mat[0][1], mat[2][1], mat[0][2], mat[2][2]);
	  adjoint[1][1] =  det2x2(mat[0][0], mat[2][0], mat[0][2], mat[2][2]);
	  adjoint[1][2] = -det2x2(mat[0][0], mat[2][0], mat[0][1], mat[2][1]);

	  adjoint[2][0] =  det2x2(mat[0][1], mat[1][1], mat[0][2], mat[1][2]);
	  adjoint[2][1] = -det2x2(mat[0][0], mat[1][0], mat[0][2], mat[1][2]);
	  adjoint[2][2] =  det2x2(mat[0][0], mat[1][0], mat[0][1], mat[1][1]);
	}

	void CMatrix3x3::ExpandToIntegerSize(double width,double height)
	{

	   if(mat[0][0] != 0.0 &&
		  FZero(mat[0][1]))
	   {
		  double targetWidth = fabs(width * mat[0][0]);
		  double targetHeight = fabs(height * mat[1][1]);

		  if (targetWidth > 10 && targetHeight > 10)
		  {
			 double finalHeight = ceil(targetHeight);
			 double targetY = mat[2][1];
			 double finalY;
			 if (mat[1][1] < 0)
			 {
				finalY = ceil(targetY);
			 }
			 else
			 {
				finalY = floor(targetY);
			 }

			 finalHeight += ceil(fabs(targetY - finalY));
			 mat[2][1] = finalY;
			 mat[1][1] *= finalHeight / targetHeight;

			 double finalWidth = ceil(targetWidth);
			 double targetX = mat[2][0];
			 double finalX;
			 if (mat[0][0] < 0)
			 {
				finalX = ceil(targetX);
			 }
			 else
			 {
				finalX = floor(targetX);
			 }

			 finalWidth += ceil(fabs(targetX - finalX));
			 mat[2][0] = finalX;
			 mat[0][0] *= finalWidth / targetWidth;
		  }
	   }
	   else if(FZero(mat[0][0]) &&
		  mat[0][1] != 0.0)
	   {
		  double targetWidth = fabs(height * mat[1][0]);
		  double targetHeight = fabs(width * mat[0][1]);

		  if (targetWidth > 10 && targetHeight > 10)
		  {
			 double finalWidth = ceil(targetWidth);
			 double targetX = mat[2][0];
			 double finalX;
			 if (mat[1][0] < 0)
			 {
				finalX = ceil(targetX);
			 }
			 else
			 {
				finalX = floor(targetX);
			 }

			 finalWidth += ceil(fabs(targetX - finalX) + .5);
			 mat[2][0] = finalX;
			 mat[1][0] *= finalWidth / targetWidth;

			 double finalHeight = ceil(targetHeight);
			 double targetY = mat[2][1];
			 double finalY;
			 if (mat[0][1] < 0)
			 {
				finalY = ceil(targetY);
			 }
			 else
			 {
				finalY = floor(targetY);
			 }

			 finalHeight += ceil(fabs(targetY - finalY) + .5);
			 mat[2][1] = finalY;
			 mat[0][1] *= finalHeight / targetHeight;
		  }
	   }
	}

	void CMatrix3x3::MakeXForm(double rot,
							   double tx, double ty,
							   double sx, double sy)
	{
	   CMatrix3x3 trans;
	   trans.SetIdentity();

	   trans(2, 0) = tx;
	   trans(2, 1) = ty;

	   CMatrix3x3 scale;
	   scale.SetIdentity();

	   scale(0, 0) = sx;
	   scale(1, 1) = sy;

	   CMatrix3x3 rotate;
	   rotate.SetIdentity();

	   rotate(0, 0) = cos(rot);
	   rotate(0, 1) = sin(rot);
	   rotate(1, 0) = -sin(rot);
	   rotate(1, 1) = cos(rot);

	   *this = scale;
	   *this = *this * rotate;
	   *this = *this * trans;
	}

	CMatrix3x3 &CMatrix3x3::operator = (const CMatrix3x3 &rhs)
	{
	  if (this != &rhs)
	  {
		memcpy(mat,rhs.mat,sizeof(mat));
		/*for (int i = 0; i < 3; i++)
		{
		  for (int j = 0; j < 3; j++)
		  {
			mat[i][j] = rhs.mat[i][j];
		  }
		}*/
	  }

	  SetDirty();

	  return(*this);
	}

	void CMatrix3x3::SetIdentity (void)
	{
	   memset(mat,0,sizeof(mat));
	   mat[0][0] = mat[1][1] = mat[2][2] = 1.0;
	  /*for (int i = 0; i < 3; i++)
	  {
		for (int j = 0; j < 3; j++)
		{
		  if (i == j)
		  {
			mat[i][j] = 1.0;
		  }
		  else
		  {
			mat[i][j] = 0.0;
		  }
		}
	  }*/

	  SetClean();
	  m_rotation = 0.0;
	  m_scale = 1.0;   
	}

	bool CMatrix3x3::IsIdentity(void)
	{
	   if(   fabs(mat[0][0]-1.0) < 0.0000001f &&
			 fabs(mat[0][1])     < 0.0000001f &&
			 fabs(mat[0][2])     < 0.0000001f &&
			 fabs(mat[1][0])     < 0.0000001f &&
			 fabs(mat[1][1]-1.0) < 0.0000001f &&
			 fabs(mat[1][2])     < 0.0000001f &&
			 fabs(mat[2][0])     < 0.0000001f &&
			 fabs(mat[2][1])     < 0.0000001f &&
			 fabs(mat[2][2]-1.0) < 0.0000001f )
	   {
		  return true;
	   }
	   return false;
	}        

	void CMatrix3x3::Set(float m00, float m01, 
						 float m10, float m11, 
						 float m20, float m21)
	{
	   mat[0][0] = m00;
	   mat[0][1] = m01;
	   mat[0][2] = 0;

	   mat[1][0] = m10;
	   mat[1][1] = m11;
	   mat[1][2] = 0;

	   mat[2][0] = m20;
	   mat[2][1] = m21;
	   mat[2][2] = 1;

	   SetDirty();
	}

	// Task$ Remove this warning
	//
	#pragma warning(disable : 4244) // conversion loss of data

	void CMatrix3x3::Get(float &m00, float &m01, 
						 float &m10, float &m11, 
						 float &m20, float &m21) const
	{
	   m00 = mat[0][0];
	   m01 = mat[0][1];

	   m10 = mat[1][0];
	   m11 = mat[1][1];

	   m20 = mat[2][0];
	   m21 = mat[2][1];
	}


	bool CMatrix3x3::Invert(void)
	{
	  bool returnVal = true;

	  double det = Determinant();

	  if (fabs(det) > 0.0)
	  {
		double adjoint[3][3];
		FindAdjoint(adjoint);

	   int i,j;
		for (i = 0; i < 3; i++)
		{
		  for (j = 0; j < 3; j++)
		  {
			inv[i][j] = adjoint[j][i] / det;
		  }
		}
	  }
	  else
	  {
		returnVal = false;
	  }

	  SetDirty();

	  return(returnVal);
	}

    bool CMatrix3x3::InvertMat()
    {
        float diet = mat[0][0] * mat[1][1] * mat[2][2] + mat[0][1] * mat[1][2] * mat[2][0] + mat[1][0] * mat[2][1] * mat[0][2]
            - mat[0][2] * mat[1][1] * mat[2][0] - mat[1][2] * mat[2][1] * mat[0][0] - mat[2][2] * mat[0][1] * mat[1][0];
        
        if (FEqual(diet, 0.0f)) return false;

        CMatrix3x3 matT;
        matT(0, 0) = mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1];
        matT(0, 1) = -(mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]);
        matT(0, 2) = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];
        matT(1, 0) = -(mat[0][1] * mat[2][2] - mat[0][2] * mat[2][1]);
        matT(1, 1) = mat[0][0] * mat[2][2] - mat[0][2] * mat[2][0];
        matT(1, 2) = -(mat[0][0] * mat[2][1] - mat[0][1] * mat[2][0]);
        matT(2, 0) = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
        matT(2, 1) = -(mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0]);
        matT(2, 2) = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

        mat[0][0] = matT(0, 0) / diet;
        mat[0][1] = matT(1, 0) / diet;
        mat[0][2] = matT(2, 0) / diet;
        mat[1][0] = matT(0, 1) / diet;
        mat[1][1] = matT(1, 1) / diet;
        mat[1][2] = matT(2, 1) / diet;
        mat[2][0] = matT(0, 2) / diet;
        mat[2][1] = matT(1, 2) / diet;
        mat[2][2] = matT(2, 2) / diet;

        return true;
    }

	bool CMatrix3x3::SetInvert(void)
	{
	  bool returnVal = false;

	  if(Invert())
	  {
		  returnVal = true;
		  memcpy(mat,inv,sizeof(mat));
		  /*for (int i = 0; i < 3; i++)
		  {
			 for (int j = 0; j < 3; j++)
			 {
				mat[i][j] = inv[i][j];
			 }
		  }*/
	  }

	  SetDirty();

	  return(returnVal);
	}

	// DJH: fix this !!!

	void CMatrix3x3::InvertOrthogonal (void)
	{
	  matSwap(mat[0][1], mat[1][0]);
	  matSwap(mat[0][2], mat[2][0]);
	  matSwap(mat[1][2], mat[2][1]);

	  SetDirty();
	}

	void CMatrix3x3::Transpose (void)
	{
	  matSwap(mat[0][1], mat[1][0]);
	  matSwap(mat[0][2], mat[2][0]);
	  matSwap(mat[1][2], mat[2][1]);

	  SetDirty();
	}

	double CMatrix3x3::Determinant (void)
	{
	   return(mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]); 
	}


};
