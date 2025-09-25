#pragma once
#include "Scene3D.hpp"
#include "View3D.hpp"
#include "Drawing2D.hpp"

class Renderer2D {
public:
    RasterBuffer<uint8_t>& rb;
    View3D& camera;
    Projection3D& projection;

    Renderer2D(RasterBuffer<uint8_t>& rb, View3D& cam, Projection3D& proj)
        : rb(rb), camera(cam), projection(proj) {}

    void render(const Scene3D& scene) {
        Drawing2D draw(rb);
        auto viewport = [&](const Point2D& p) {
            double x = (p.x+1.0)*0.5*rb.width;
            double y = (1.0-(p.y+1.0)*0.5)*rb.height;
            return Point2D(x,y);
        };

        for (const auto& poly : scene.objects) {
            if (poly.vertices.size()<3) continue;

            Point3D n = poly.normal();
            if (n.z >= 0) continue; // backface cull

            std::vector<Point2D> pts2d;
            std::vector<double> depths;
            for (auto& v : poly.vertices) {
                Point2D proj2d = projection.project(v);
                pts2d.push_back(viewport(proj2d));
                depths.push_back(v.z);
            }

            draw.fill_polygon_z(pts2d, depths, 200);
            draw.polygon(pts2d, 255);
        }
    }
};
