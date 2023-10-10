#include "stdafx.h"
#include "Rect.h"


namespace FF_Font_Factory {

    CFeRect::CFeRect()
    {
        m_bDirty = true;
        m_left = m_right = m_bottom = m_top = 0.0;
    }

    CFeRect::CFeRect(float left, float right, float top, float bottom)
    {
        m_left = left;
        m_right = right;
        m_top = top;
        m_bottom = bottom;
    }

    bool CFeRect::operator == (const CFeRect &otherrect) const
    {
        if (FEqual(m_left, otherrect.m_left) &&
            FEqual(m_right, otherrect.m_right) &&
            FEqual(m_bottom, otherrect.m_bottom) &&
            FEqual(m_top, otherrect.m_top))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    float CFeRect::GetWidth(void) const
    {
        if (m_bDirty)
        {
            CalcWidthAndHeight();
        }
        return m_width;
    }

    float CFeRect::GetHeight(void) const
    {
        if (m_bDirty)
        {
            CalcWidthAndHeight();
        }
        return m_height;
    }

    float CFeRect::GetArea(void) const
    {
        float width = GetWidth();
        float height = GetHeight();
        return width * height;
    }

    CRealPoint CFeRect::GetCenter(void) const
    {
        CRealPoint result;
        result.Set((m_left + m_right) / 2.0f, (m_bottom + m_top) / 2.0f);
        return result;
    }

    CRealPoint CFeRect::GetLL(void) const
    {
        CRealPoint result;
        result.Set(m_left, m_bottom);
        return result;
    }

    CRealPoint CFeRect::GetLR(void) const
    {
        CRealPoint result;
        result.Set(m_right, m_bottom);
        return result;
    }

    CRealPoint CFeRect::GetUL(void) const
    {
        CRealPoint result;
        result.Set(m_left, m_top);
        return result;
    }

    CRealPoint CFeRect::GetUR(void) const
    {
        CRealPoint result;
        result.Set(m_right, m_top);
        return result;
    }

    void CFeRect::Set(const CRealPoint &pt)
    {
        m_bDirty = true;
        m_left = m_right = pt.x;
        m_bottom = m_top = pt.y;
    }

    void CFeRect::Set(const float x, const float y)
    {
        m_bDirty = true;
        m_left = m_right = x;
        m_bottom = m_top = y;
    }

    void CFeRect::Move(float x, float y)
    {
        m_left += x;
        m_right += x;
        m_top += y;
        m_bottom += y;
    }

    void CFeRect::Add(const CRealPoint &pt)
    {
        if (pt.x < m_left)
        {
            m_left = pt.x;
        }
        else if (pt.x > m_right)
        {
            m_right = pt.x;
        }
        if (pt.y < m_top)
        {
            m_top = pt.y;
        }
        else if (pt.y > m_bottom)
        {
            m_bottom = pt.y;
        }
        m_bDirty = true;
    }

    void CFeRect::Add(const float x, const float y)
    {
        if (x < m_left)
        {
            m_left = x;
        }
        else if (x > m_right)
        {
            m_right = x;
        }
        if (y < m_top)
        {
            m_top = y;
        }
        else if (y > m_bottom)
        {
            m_bottom = y;
        }
        m_bDirty = true;
    }

    void CFeRect::Add(const CFeRect &Rect)
    {
        if (Rect.m_left < m_left)
        {
            m_left = Rect.m_left;
        }
        if (Rect.m_right > m_right)
        {
            m_right = Rect.m_right;
        }
        if (Rect.m_top < m_top)
        {
            m_top = Rect.m_top;
        }
        if (Rect.m_bottom > m_bottom)
        {
            m_bottom = Rect.m_bottom;
        }
        m_bDirty = true;
    }

    bool CFeRect::isOverlapped(const CFeRect *Rect)
    {
        if (Rect->m_left >= m_right ||
            Rect->m_right <= m_left ||
            Rect->m_bottom <= m_top ||
            Rect->m_top >= m_bottom)
        {
            return false;
        }
        return true;
    }

    bool CFeRect::GetOverlap(CFeRect *rect, CFeRect *overlap)
    {
        bool result = isOverlapped(rect);
        if (result)
        {
            overlap->Set(CRealPoint(__min(rect->m_right, m_right), __min(rect->m_bottom, m_bottom)));
            overlap->Add(CRealPoint(__max(rect->m_left, m_left), __max(rect->m_top, m_top)));
        }
        return result;
    }

    bool CFeRect::isInside(CRealPoint *p)
    {
        if (m_left > p->x ||
            m_right < p->x ||
            m_top > p->y ||
            m_bottom < p->y)
        {
            return false;
        }
        return true;
    }

    bool CFeRect::isInside(CFeRect *Rect, float tol)
    {                                         // added to try and delete clip regions that are equal to a rectangle, causing it to disapear at certain zoom levels.
        if (tol == 0)
            tol = __max(__max(Rect->GetWidth(), Rect->GetHeight()), __max(GetWidth(), GetHeight()))*.00006f;

        if (Rect->m_left >= m_left - tol &&
            Rect->m_right <= m_right + tol &&
            Rect->m_top >= m_top - tol &&
            Rect->m_bottom <= m_bottom + tol)
        {
            return true;
        }
        return false;
    }

    bool CFeRect::isOutside(CFeRect *Rect)
    {
        if (Rect->m_left >= m_right ||
            Rect->m_right <= m_left ||
            Rect->m_top >= m_bottom ||
            Rect->m_bottom <= m_top)
        {
            return true;
        }
        return false;
    }

    bool CFeRect::isOverlapped(CRealPoint *p1, CRealPoint *p2)
    {
        if ((p1->x < m_left   && p2->x < m_left) ||
            (p1->x > m_right  && p2->x > m_right) ||
            (p1->y < m_bottom && p2->y < m_bottom) ||
            (p1->y > m_top    && p2->y > m_top))
        {
            return false;
        }
        return true;
    }

    bool CFeRect::AND_me_with(CFeRect &rect)
    {
        bool result = false;

        // for AND, the rects MUST overlap.
        CFeRect overlap;
        if (GetOverlap(&rect, &overlap))
        {
            // set me to the intersection
            *this = overlap;
            result = true;
        }

        if (result)m_bDirty = true;
        return result;
    }

    bool CFeRect::OR_me_with(CFeRect &rect)
    {
        bool result = false;

        // for OR, one rect MUST completely envelop, or equal, the other
        if (isInside(&rect, 0.0f))
        {
            // he is inside me.
            // so I am already the shape I need to be
            result = true;
        }
        else if (rect.isInside(this, 0.0f))
        {
            // I am inside him.
            // so become him.
            *this = rect;
            result = true;
        }

        if (result)m_bDirty = true;
        return result;
    }

    bool CFeRect::XOR_me_with(CFeRect &rect)
    {
        bool result = false;

        // for XOR, my rect MUST completely envelop, or equal, the other
        if (isInside(&rect, 0.0f))
        {
            // he is inside me.
            // now we must share 3 out of 4 sides
            int numsidesshared = 0;
            bool share_left = FEqual(m_left, rect.m_left);        if (share_left)numsidesshared++;
            bool share_right = FEqual(m_right, rect.m_right);     if (share_right)numsidesshared++;
            bool share_bottom = FEqual(m_bottom, rect.m_bottom);  if (share_bottom)numsidesshared++;
            bool share_top = FEqual(m_top, rect.m_top);           if (share_top)numsidesshared++;

            if (numsidesshared == 3)
            {
                if (!share_left)
                {
                    m_right = rect.m_left;
                }
                else if (!share_right)
                {
                    m_left = rect.m_right;
                }
                else if (!share_bottom)
                {
                    m_top = rect.m_bottom;
                }
                else if (!share_top)
                {
                    m_bottom = rect.m_top;
                }

                result = true;
            }
            else if (numsidesshared == 4)
            {
                result = true;
                m_left = m_right = m_bottom = m_top = 0.0f;
            }
        }

        if (result)m_bDirty = true;
        return result;
    }

    bool CFeRect::DIFF_me_with(CFeRect &rect)
    {
        bool result = false;

        // for DIFF, the other rect MUST clip me, completely, on one side
        // so examine each side

        if (rect.m_right > m_left &&
            rect.m_left < m_left &&
            rect.m_right < m_right &&
            rect.m_bottom >= m_bottom &&
            rect.m_top <= m_top)
        {
            // clipped left side
            m_left = rect.m_right;
            result = true;
        }
        else if (rect.m_left < m_right &&
            rect.m_right > m_right &&
            rect.m_left > m_left &&
            rect.m_bottom >= m_bottom &&
            rect.m_top <= m_top)
        {
            // clipped right side
            m_right = rect.m_left;
            result = true;
        }
        else if (rect.m_top < m_bottom &&
            rect.m_bottom > m_bottom &&
            rect.m_top > m_top &&
            rect.m_right >= m_right &&
            rect.m_left <= m_left)
        {
            // clipped bottom side
            m_bottom = rect.m_top;
            result = true;
        }
        else if (rect.m_bottom > m_top &&
            rect.m_top < m_top &&
            rect.m_bottom < m_bottom &&
            rect.m_right >= m_right &&
            rect.m_left <= m_left)
        {
            // clipped top side
            m_top = rect.m_bottom;
            result = true;
        }
        else if (rect.isInside(this, 0.0f))
        {
            // if he completely clips me
            result = true;
            m_left = m_right = m_bottom = m_top = 0.0f;
        }

        if (result)m_bDirty = true;
        return result;
    }

    CRealVector CFeRect::GetDistanceTo(CFeRect &rect) const
    {
        float x = 0.0;
        float y = 0.0;
        CRealVector result;
        if (m_top < rect.m_bottom || rect.m_top < m_bottom)
        {
            y = __min(fabs(m_top - rect.m_bottom), fabs(m_bottom - rect.m_top));
        }
        if (m_left > rect.m_right || rect.m_left > m_right)
        {
            x = __min(fabs(m_left - rect.m_right), fabs(m_right - rect.m_left));
        }
        result.Set(x, y);
        return result;
    }

    float CFeRect::GetOverlapX(const CFeRect *rect) const
    {
        float result = 0.0;
        if (rect != NULL)
        {
            if (m_right < rect->m_left || rect->m_right < m_left)
            {
                return result;
            }
            result = __min(m_right, rect->m_right) - __max(m_left, rect->m_left);
        }
        return result;
    }

    void CFeRect::TrimTo(const CFeRect &rect)
    {
        if (m_left < rect.GetLeft())
            m_left = rect.GetLeft();
        if (m_right > rect.GetRight())
            m_right = rect.GetRight();
        if (m_bottom > rect.GetBottom())
            m_bottom = rect.GetBottom();
        if (m_top < rect.GetTop())
            m_top = rect.GetTop();
    }

    void CFeRect::TrimLineTo(CRealPoint *p1, CRealPoint *p2)
    {
        // first, trim if line isn't in rect
        if ((p1->x < m_left &&
            p2->x < m_left) ||
            (p1->x > m_right &&
            p2->x > m_right) ||
            (p1->y > m_bottom &&
            p2->y > m_bottom) ||
            (p1->y < m_top &&
            p2->y < m_top))
        {
            // offscreen
            *p2 = *p1;
            return;
        }

        // trim begin point 
        CRealVector tobegin(*p2, *p1);

        // trim begin point against left edge
        if (p1->x < m_left &&
            p2->x > m_left)
        {
            float scale = (p2->x - m_left) / (p2->x - p1->x);
            tobegin.SetLength(tobegin.Length() * scale);

            *p1 = *p2 + tobegin;
        }

        // trim begin point against bottom edge
        if (p1->y > m_bottom &&
            p2->y < m_bottom)
        {
            float scale = (m_bottom - p2->y) / (p1->y - p2->y);
            tobegin.SetLength(tobegin.Length() * scale);

            *p1 = *p2 + tobegin;
        }

        // trim begin point against right edge
        if (p1->x > m_right &&
            p2->x < m_right)
        {
            float scale = (m_right - p2->x) / (p1->x - p2->x);
            tobegin.SetLength(tobegin.Length() * scale);

            *p1 = *p2 + tobegin;
        }

        // trim begin point against top edge
        if (p1->y < m_top &&
            p2->y > m_top)
        {
            float scale = (p2->y - m_top) / (p2->y - p1->y);
            tobegin.SetLength(tobegin.Length() * scale);

            *p1 = *p2 + tobegin;
        }

        // trim end point 
        CRealVector toend(*p1, *p2);

        // trim end point against left edge
        if (p2->x < m_left &&
            p1->x > m_left)
        {
            float scale = (p1->x - m_left) / (p1->x - p2->x);
            toend.SetLength(toend.Length() * scale);

            *p2 = *p1 + toend;
        }

        // trim end point against bottom edge
        if (p2->y > m_bottom &&
            p1->y < m_bottom)
        {
            float scale = (m_bottom - p1->y) / (p2->y - p1->y);
            toend.SetLength(toend.Length() * scale);

            *p2 = *p1 + toend;
        }

        // trim end point against right edge
        if (p2->x > m_right &&
            p1->x < m_right)
        {
            float scale = (m_right - p1->x) / (p2->x - p1->x);
            toend.SetLength(toend.Length() * scale);

            *p2 = *p1 + toend;
        }

        // trim end point against top edge
        if (p2->y < m_top &&
            p1->y > m_top)
        {
            float scale = (m_top - p1->y) / (p2->y - p1->y);
            toend.SetLength(toend.Length() * scale);

            *p2 = *p1 + toend;
        }
    }


    float CFeRect::GetOverlapY(const CFeRect *rect) const
    {
        float result = 0.0;
        if (rect != NULL)
        {
            if (m_bottom < rect->m_top || m_top > rect->m_bottom)
            {
                return result;
            }
            result = __min(m_bottom, rect->m_bottom) - __max(m_top, rect->m_top);
        }
        return result;
    }

    void CFeRect::Scale(float percentage)
    {
        float foo = (((m_right - m_left) * percentage) - (m_right - m_left)) / 2.0f;
        m_left -= foo;
        m_right += foo;
        foo = (((m_bottom - m_top)* percentage) - (m_bottom - m_top)) / 2.0f;
        m_top -= foo;
        m_bottom += foo;
        m_bDirty = true;
    }

    CFeRect &CFeRect::operator -= (float val)
    {
        m_left += val;
        m_right -= val;
        m_top += val;
        m_bottom -= val;
        return(*this);
    }

    CFeRect &CFeRect::operator += (float val)
    {
        m_left -= val;
        m_right += val;
        m_top -= val;
        m_bottom += val;
        return(*this);
    }

    void CFeRect::TransformBy(const CMatrix3x3 &mat)
    {
        CRealPoint p1(m_left, m_bottom);
        CRealPoint p2(m_right, m_bottom);
        CRealPoint p3(m_right, m_top);
        CRealPoint p4(m_left, m_top);
        p1 = p1 * mat;
        p2 = p2 * mat;
        p3 = p3 * mat;
        p4 = p4 * mat;
        m_left = __min(p1.x, __min(p2.x, __min(p3.x, p4.x)));
        m_right = __max(p1.x, __max(p2.x, __max(p3.x, p4.x)));
        m_top = __min(p1.y, __min(p2.y, __min(p3.y, p4.y)));
        m_bottom = __max(p1.y, __max(p2.y, __max(p3.y, p4.y)));
    }

};
