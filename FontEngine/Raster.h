#pragma once
#include "BaseFont.h"
#include "RealPoint.h"

namespace FF_Font_Factory
{
    void initEdgeTable();
    void storeEdgeInTable(int x1, int y1, int x2, int y2);
    void ScanlineFill(int maxHeight);
};
