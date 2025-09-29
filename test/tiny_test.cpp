#include "RasterBuffer.hpp"
#include "Drawing2D.hpp"
#include "GO_Polygon2D.hpp"
#include <cmath>

int main() {
    RasterBuffer<uint8_t> rb(256, 256, 1, 0);
    Drawing2D d(rb);

    // Axes
    d.line({0,128}, {255,128}, 128);
    d.line({128,0}, {128,255}, 128);

    // Circle
    d.circle({128,128}, 60, 200);

    // Cubic Bezier
    d.cubic_bezier({20,200},{64,20},{192,236},{236,56}, 64, 255);

    // Triangle polygon
    GO_Polygon2D tri({{30,30},{120,50},{60,120}});
    tri.draw(rb, 255);

    rb.save_ppm("out.ppm");
    return 0;
}
