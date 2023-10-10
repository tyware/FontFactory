#include "stdafx.h"
#include "BaseFont.h"
//#include "TrueTypeFont.h"
#include "TrueType.h"
#include "NonZeroWinding.h"
#include "BezierCurve.h"
#include "Raster.h"

namespace FF_Font_Factory
{
    char* Wchar2Char(const wchar_t* src, bool bFreeSrc)
    {
        size_t len = 0;
        errno_t err = wcstombs_s(&len, NULL, 0, src, len + 1);
        if (err != 0) return NULL;
        char *pChar = new char[len + 1];
        wcstombs_s(&len, pChar, len + 1, src, len + 1);
        if (bFreeSrc) delete[] src;
        return pChar;
    }
    wchar_t* Char2Wchar(const char* src, bool bFreeSrc)
    {
        size_t len = strlen(src);
        mbstowcs_s(&len, NULL, 0, src, len + 1);
        wchar_t *pWchar = new wchar_t[len + 1];
        mbstowcs_s(&len, pWchar, len + 1, src, len + 1);
        if (bFreeSrc) delete[] src;
        return pWchar;
    }


    CBaseFont* CBaseFont::Create(FONT_TYPE fontType)
    {
        CBaseFont *pFont = NULL;
        switch(fontType)
        {
        case FT_TRUETYPE:
            pFont = new FF_Font_Factory_TrueType::CTrueTypeFont();
            break;
        default:
            return NULL;
        }

        if(pFont)
        {
            pFont->m_nFontType = fontType;
        }

        return pFont;
    }

    CBaseFont::CBaseFont()
        : m_nFontType(FT_TRUETYPE), m_pFileName(0), m_pBuffer(0), m_nLength(0), m_nOffset(0),m_pFontName(0), m_nRefCount(0)
    {
    }

    CBaseFont::~CBaseFont()
    {
        //if(fp) fclose(fp);
        if(m_pFileName) delete[] m_pFileName;
        if(m_pBuffer) delete[] m_pBuffer;
        if(m_pFontName) delete[] m_pFontName;
        for(size_t i = 0; i<m_arrayFaces.size(); i++)
        {
            delete m_arrayFaces[i];
        }
        m_arrayFaces.clear();
    }

    BOOL CBaseFont::LoadFontFile(wchar_t *fileName)
    {
        if(m_pFileName) delete[] m_pFileName;
        size_t len = wcslen(fileName) + sizeof(wchar_t);
        m_pFileName = new wchar_t[len];
        wcscpy_s(m_pFileName, len, fileName);

        FILE *fp = NULL;
        _wfopen_s(&fp, fileName, L"rb");
        if(!fp) return false;

        fseek(fp, 0, SEEK_END);
        m_nLength = ftell(fp);

        m_pBuffer = new char[m_nLength];
        fseek(fp, 0, SEEK_SET);
        fread(m_pBuffer, m_nLength, 1, fp);
        fclose(fp);

        m_nOffset = 0;

        return true;
    }

    BOOL CBaseFont::LoadFont(char *buffer, int buflen)
    {
        m_pBuffer = buffer;
        m_nLength = buflen;
        m_nOffset = 0;

        return true;
    }

    CFontFace* CBaseFont::CreateFontFace(int fontSize, int dpi, BYTE flags, int platformId, int encodingId)
    {
        //CFontFace *pFace = new CFontFace(this, fontSize, dpi, flags);

        return NULL;
    }
    void CBaseFont::DeleteFontFace(CFontFace* pFace)
    {
        if(pFace)
        {
            pFace->m_nRefCount--;
            if(pFace->m_nRefCount <= 0)
            {
                for(size_t i=0; i<m_arrayFaces.size(); i++)
                {
                    if(m_arrayFaces[i] == pFace)
                    {
                        m_arrayFaces.erase(m_arrayFaces.begin() + i);
                        delete pFace;
                        return;
                    }
                }
            }
        }
    }

    size_t CBaseFont::ReadBytes(char *buffer, size_t count)
    {
        //if(fp == NULL || count == 0) return 0;
        //return fread_s(buffer, count, 1, count, fp);
        if(m_nOffset >= m_nLength || count == 0) return 0;
        if(m_nOffset + count > m_nLength)
        {
            memcpy_s(buffer, count, &m_pBuffer[m_nOffset], m_nLength - m_nOffset);
            m_nOffset = m_nLength;
            return m_nLength - m_nOffset;
        }
        else
        {
            memcpy_s(buffer, count, &m_pBuffer[m_nOffset], count);
            m_nOffset += count;
            return count;
       }
    }

    int CBaseFont::Seek(long _Offset, int _Origin)
    {
        //if(fp) return fseek(fp, _Offset, _Origin);
        //else return 0;
        switch(_Origin)
        {
        case SEEK_SET:
            m_nOffset = _Offset;
            break;
        case SEEK_CUR:
            m_nOffset += _Offset;
            break;
        case SEEK_END:
            m_nOffset = m_nLength + _Offset;
            break;
        }
        if(_Offset < 0) m_nOffset = 0;
        else if(_Offset > m_nLength) m_nOffset = m_nLength;

        return 0;
    }

    long CBaseFont::Tell()
    {
        //if(fp) return ftell(fp);
        //else return 0;
        return (long)m_nOffset;
    }

    CGlyphBmap* CBaseFont::GenerateGlyphBmap(const float lsb, const CFeRect &bbox, const CGlyphOutlines *pOutlines)
    {
        CGlyphBmap *pBmap = NULL;

        if(!pOutlines || pOutlines->numContours <= 0) return NULL;

        CGlyphOutlines *pPointOutlines = FF_Font_Factory::_ConvertOutlinesToPoints((FF_Font_Factory::CGlyphOutlines*)pOutlines);

        CFeRect rc(bbox);
        //rc.Add(lsb, rect.GetTop());
        
        pBmap = new CGlyphBmap();
        pBmap->width = (int)rc.GetWidth() + 1;
        pBmap->height = (int)rc.GetHeight() + 1;
        pBmap->bwidth = ((((pBmap->width * 8 + 31) / 32) * 32) / 8);
        //pBmap->zeroX = (int)bbox.GetLeft();
        pBmap->zeroY = bbox.GetTop();
        pBmap->zeroX = lsb;
        pBmap->data = new char[pBmap->bwidth * pBmap->height];
        memset(pBmap->data, 255, pBmap->bwidth * pBmap->height);

        float x = bbox.GetLeft();
        float y = bbox.GetTop();

        unsigned char *pData;
        for(int i = 0; i<pBmap->height; i++)
        {
            pData = (unsigned char *)&pBmap->data[i * pBmap->bwidth];
            for(int j = 0; j<pBmap->width; j++)
            {
                CRealPoint point((float)(j + x), (float)(i + y));
                if(CNonZeroWinding::IsInside(point, pPointOutlines))
                    *pData = 0;

                pData++;
            }
        }

        if(pPointOutlines) delete pPointOutlines;

        return pBmap;
    }

    //void* CBaseFont::GetFaceArray()
    //{
    //    return &m_arrayFaces;
    //}

    size_t CBaseFont::GetNumFaces()
    {
        return m_arrayFaces.size();
    }

    CFontFace* CBaseFont::GetFace(size_t index)
    {
        if(index >= 0 && index < m_arrayFaces.size())
            return m_arrayFaces[index];
        return NULL;
    }

    void CBaseFont::AppendFace(CFontFace *pFace)
    {
        m_arrayFaces.push_back(pFace);
    }

    CGlyphOutlines* CBaseFont::ConvertOutlinesToPoints(CGlyphOutlines *pRawOutlines, float scale)
    {
        return _ConvertOutlinesToPoints(pRawOutlines, scale);

    }

};
