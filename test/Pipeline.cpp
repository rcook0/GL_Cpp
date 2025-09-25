#include "Scene3D.hpp"
#include "View3DParameters.hpp"
#include "Drawing2D.hpp"
#include "RasterBuffer.hpp"
#include <iostream>
#include <cmath>

int main() {
    // --- Camera parameters ---
    View3DParameters params(
        Point3D(0,0,5),    // eye
        Point3D(0,0,0),    // target
        Point3D(0,1,0),    // up
        60.0 * M_PI/180.0, // fov
        1.0,               // aspect
        0.1,               // near
        100.0              // far
    );

    View3D camera = params.make_view();
    Projection3D projection = params.make_projection();

    // --- Scene setup ---
    Scene3D scene(projection);

    // A simple square pyramid
    Polygon3D base({
        Point3D(-1,-1,0),
        Point3D( 1,-1,0),
        Point3D( 1, 1,0),
        Point3D(-1, 1,0)
    });
    Polygon3D side1({ Point3D(-1,-1,0), Point3D(1,-1,0), Point3D(0,0,2) });
    Polygon3D side2({ Point3D(1,-1,0),  Point3D(1, 1,0), Point3D(0,0,2) });
    Polygon3D side3({ Point3D(1, 1,0),  Point3D(-1, 1,0),Point3D(0,0,2) });
    Polygon3D side4({ Point3D(-1, 1,0), Point3D(-1,-1,0),Point3D(0,0,2) });

    scene.add_object(base);
    scene.add_object(side1);
    scene.add_object(side2);
    scene.add_object(side3);
    scene.add_object(side4);

    // --- Apply view transform (camera) ---
    scene.apply_transformation(camera.view_matrix());

    // --- Project into 2D ---
    auto polys2d = scene.project_to_2d();

    // --- Draw into a raster buffer ---
    RasterBuffer<uint8_t> rb(512, 512, 1, 0);
    Drawing2D draw(rb);

    // Simple viewport transform: map [-1,1] → [0,width] and [0,height]
    auto viewport = [&](const Point2D& p) {
        double x = (p.x + 1.0) * 0.5 * rb.width;
        double y = (1.0 - (p.y + 1.0) * 0.5) * rb.height; // flip y
        return Point2D(x,y);
    };

    for (const auto& poly : polys2d) {
        std::vector<Point2D> screenPoly;
        for (auto& v : poly) screenPoly.push_back(viewport(v));
        draw.polygon(screenPoly, 255);
    }

    rb.save_ppm("scene.ppm");
    std::cout << "Scene rendered to scene.ppm\n";
    return 0;
}
