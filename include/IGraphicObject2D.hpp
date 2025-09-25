#pragma once
#include "Point2D.hpp"
#include "RasterBuffer.hpp"

class IGraphicObject2D {
public:
    virtual ~IGraphicObject2D() = default;
    virtual void draw(RasterBuffer<uint8_t>& rb, uint8_t intensity=255) const = 0;
};
