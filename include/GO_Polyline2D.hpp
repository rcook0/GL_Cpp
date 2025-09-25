#pragma once
#include "IGraphicObject2D.hpp"
#include "Drawing2D.hpp"
#include <vector>

class GO_Polyline2D : public IGraphicObject2D {
public:
    std::vector<Point2D> pts;
    explicit GO_Polyline2D(std::vector<Point2D> pts): pts(std::move(pts)) {}
    void draw(RasterBuffer<uint8_t>& rb, uint8_t intensity=255) const override {
        Drawing2D d(rb); d.polyline(pts, intensity);
    }
};
