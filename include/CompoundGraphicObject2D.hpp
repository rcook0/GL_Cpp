#pragma once
#include "IGraphicObject2D.hpp"
#include <memory>
#include <vector>

class CompoundGraphicObject2D : public IGraphicObject2D {
public:
    void add(std::shared_ptr<IGraphicObject2D> obj) {
        children.push_back(std::move(obj));
    }

    void draw(RasterBuffer<uint8_t>& rb, uint8_t intensity=255) const override {
        for (const auto& ch : children) ch->draw(rb, intensity);
    }

private:
    std::vector<std::shared_ptr<IGraphicObject2D>> children;
};
