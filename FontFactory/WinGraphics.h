#pragma once


class CWinGraphics
{
public:
    CWinGraphics(void);
    virtual ~CWinGraphics(void);

public:
	// CWinGraphics class interface
	static void DrawLine(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, COLORREF color);
	//static void DrawPolyline(Gdiplus::Graphics &g, POINT* points, int numberPoints, COLORREF color);
	//static void DrawPolygon(Gdiplus::Graphics &g, POINT* points, int numberPoints, COLORREF color);
	//static void DrawEllipse(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, COLORREF color);
	//static void DrawArc(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	//static void DrawPie(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	//static void DrawChord(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	//static void DrawRoundRect(Gdiplus::Graphics &g, int x1, int y1, int x2, int y2, int width, int height, COLORREF color);
	static void DrawBezier(Gdiplus::Graphics &g, POINT* points, int numPoints, COLORREF color);
};

