#pragma once
#include "IGraphicObject2D.hpp"
#include "Drawing2D.hpp"

class GO_Point2D : public IGraphicObject2D {
public:
    Point2D p;
    explicit GO_Point2D(Point2D p): p(p) {}
    void draw(RasterBuffer<uint8_t>& rb, uint8_t intensity=255) const override {
        Drawing2D d(rb); d.point(p, intensity);
    }
};
