#pragma once

#include "matrix.h"
#include "FeHeader.h"

namespace FF_Font_Factory {

    class __declspec(dllexport) lRect
    {
    public:
        lRect() :left(0), right(0), top(0), bottom(0){};
        int left, right, top, bottom;
    };
    class __declspec(dllexport) CFeRect
    {
    public:
        CFeRect();
        CFeRect(float left, float right, float top, float bottom);

    public:
        operator lRect() const
        {
            lRect result;
            result.left = (int)GetLeft();
            result.right = (int)GetRight();
            result.bottom = (int)GetBottom();
            result.top = (int)GetTop();
            return result;
        }

        CFeRect &operator = (const lRect rect)
        {
            SetLeft((float)rect.left);
            SetRight((float)rect.right);
            SetTop((float)rect.top);
            SetBottom((float)rect.bottom);
            return(*this);
        }

        CFeRect &operator = (const CFeRect rect)
        {
            SetLeft((float)rect.GetLeft());
            SetRight((float)rect.GetRight());
            SetTop((float)rect.GetTop());
            SetBottom((float)rect.GetBottom());
            return(*this);
        }


        void SetLeft(float value){ m_left = value; m_bDirty = true; }
        void SetRight(float value){ m_right = value; m_bDirty = true; }
        void SetBottom(float value){ m_bottom = value; m_bDirty = true; }
        void SetTop(float value){ m_top = value; m_bDirty = true; }
        float GetLeft(void)const{ return m_left; }
        float GetRight(void)const{ return m_right; }
        float GetBottom(void)const{ return m_bottom; }
        float GetTop(void)const{ return m_top; }
        float GetWidth(void) const;
        float GetHeight(void) const;
        float GetArea(void) const;
        CRealPoint GetCenter(void) const;
        CRealPoint GetLL(void) const;
        CRealPoint GetLR(void) const;
        CRealPoint GetUL(void) const;
        CRealPoint GetUR(void) const;
        void Move(float x, float y);
        void Add(const CRealPoint &pt);
        void Set(const CRealPoint &pt);
        void Add(const float x, const float y);
        void Set(const float x, const float y);
        void Add(const CFeRect &Rect);
        bool isOverlapped(const CFeRect *Rect);
        bool isOverlapped(CRealPoint *p1, CRealPoint *p2);
        bool isInside(CRealPoint *p);
        bool isInside(CFeRect *Rect, float tol = 0);
        bool isOutside(CFeRect *Rect);
        bool GetOverlap(CFeRect *rect, CFeRect *overlap);
        void Scale(float percentage);
        bool operator == (const CFeRect &otherrect) const;

        CFeRect &operator -= (float val);
        CFeRect &operator += (float val);

        void TransformBy(const CMatrix3x3 &mat);

        float GetOverlapX(const CFeRect *rect) const;
        float GetOverlapY(const CFeRect *rect) const;
        CRealVector GetDistanceTo(CFeRect &rect) const;
        void TrimTo(const CFeRect &rect);

        void TrimLineTo(CRealPoint *p1, CRealPoint *p2);

        bool AND_me_with(CFeRect &rect);
        bool OR_me_with(CFeRect &rect);
        bool XOR_me_with(CFeRect &rect);
        bool DIFF_me_with(CFeRect &rect);

	public:
		float m_left;
		float m_right;
		float m_top;
		float m_bottom;

    private:
        void CalcWidthAndHeight(void) const
        {
            m_width = (float)fabs(m_right - m_left);
            m_height = (float)fabs(m_top - m_bottom);
            m_bDirty = false;
        }

        mutable bool   m_bDirty;
        mutable float m_width;
        mutable float m_height;
    };
};
