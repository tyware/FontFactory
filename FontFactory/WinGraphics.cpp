#include "stdafx.h"
#include "WinGraphics.h"
//#include "GdiPlus.h"


CWinGraphics::CWinGraphics(void)
{
}

CWinGraphics::~CWinGraphics(void)
{
}

void CWinGraphics::DrawLine(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, COLORREF color)
{
}

//void DrawPolyline(Gdiplus::Graphics &g, POINT* points, int numberPoints, COLORREF color)
//{
//}
//
//void DrawPolygon(Gdiplus::Graphics &g, POINT* points, int numberPoints, COLORREF color)
//{
//}
//
//void DrawEllipse(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, COLORREF color)
//{
//}
//
//void DrawArc(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color)
//{
//}
//
//void DrawPie(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color)
//{
//}
//
//void DrawChord(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color)
//{
//}
//
//void DrawRoundRect(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, int width, int height, COLORREF color)
//{
//}

void DrawBezier(Gdiplus::Graphics &g, POINT* points, int numPoints, COLORREF color)
{
}
