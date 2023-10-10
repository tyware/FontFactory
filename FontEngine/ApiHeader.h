#pragma once

typedef void* FE_Handle;
typedef FE_Handle FE_FontEngine;
typedef FE_Handle FE_Font;
typedef FE_Handle FE_FontFace;
typedef FE_Handle FE_CMap;
typedef FE_Handle FE_Glyph;
typedef FE_Handle FE_DebugInfo;

typedef enum _Font_Type
{
    FT_TRUETYPE = 0,
    FT_TYPE_1_COMPACT,
    FT_TYPE_1,
    FT_TYPE_2,
    FT_TYPE_NONE,
}FONT_TYPE;


typedef enum __FE_ERROR__
{
    FE_ERROR_None,
    FE_ERROR_Load_File_Fail,

} FE_ERROR;

typedef enum __FE_PLATFORM_ID__
{
    FE_PLATFORM_ID_Default = -1,
    FE_PLATFORM_ID_Unicode = 0,
    FE_PLATFORM_ID_Mac = 1,
    FE_PLATFORM_ID_Windows = 3,
}FE_PLATFORM_ID;

typedef enum __FE_UNICODE_ENCODING_ID__
{
    FE_UCS_ENCODING_Default        = 0,
    FE_UCS_ENCODING_V1_1           = 1,
    FE_UCS_ENCODING_ISO            = 2,
    FE_UCS_ENCODING_Unicode2BMP    = 3,
    FE_UCS_ENCODING_Unicode2NonBMP = 4,
    FE_UCS_ENCODING_UVS            = 5,
    FE_UCS_ENCODING_Unicode        = 6,
}FE_UNICODE_ENCODING_ID;

typedef enum __FE_WINDOWS_ENCODING_ID__
{
    FE_WIN_ENCODING_Symbol    = 0,  // symbo
    FE_WIN_ENCODING_UCS2      = 1,  // default
    FE_WIN_ENCODING_ShiftJIS  = 2,
    FE_WIN_ENCODING_CHINESE   = 3,
    FE_WIN_ENCODING_BIG5      = 4,
    FE_WIN_ENCODING_Wansung   = 5,
    FE_WIN_ENCODING_Johab     = 6,
    FE_WIN_ENCODING_UCS4      = 10,
}FE_WINDOWS_ENCODING_ID;

typedef struct __FE_Point__
{
    int bOnCurve;
    float x, y;
} FE_Point;

typedef struct __FE_Outlines__
{
    int numContours;
    unsigned int *pEndPtsOfContours;
    int numPoints;
    FE_Point *pCoordinates; // number of coordinates is the last value of pEndPtsOfContours + 1
} FE_Outlines;

typedef struct __FE_Bitmap__
{
    int width, height, bwidth;
    float zeroX, zeroY;
    char *data;
} FE_Bitmap;

typedef struct __FE_Rect__
{
    //int left, right;
    //int top, bottom;
    float left, right;
    float top, bottom;
}FE_Rect;