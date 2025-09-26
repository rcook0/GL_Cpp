#include "Shading.hpp"

class Renderer2D {
public:
    RasterBuffer<uint8_t>& rb;
    View3D& camera;
    Projection3D& projection;
    Point3D lightDir;

    Renderer2D(RasterBuffer<uint8_t>& rb, View3D& cam, Projection3D& proj,
               const Point3D& lightDir=Point3D(0,0,-1))
        : rb(rb), camera(cam), projection(proj), lightDir(lightDir) {}

    void render(const Scene3D& scene, const std::string& mode="lambert") {
        Drawing2D draw(rb);
        auto viewport=[&](const Point2D& p){
            double x=(p.x+1.0)*0.5*rb.width;
            double y=(1.0-(p.y+1.0)*0.5)*rb.height;
            return Point2D(x,y);
        };

        for (const auto& poly : scene.objects) {
            if (poly.vertices.size()<3) continue;

            Point3D n=poly.normal();
            if (n.z>=0) continue; // backface cull

            std::vector<Point2D> pts2d;
            std::vector<double> depths;
            for (auto& v : poly.vertices) {
                Point2D proj2d=projection.project(v);
                pts2d.push_back(viewport(proj2d));
                depths.push_back(v.z);
            }

            if (mode=="lambert") {
                uint8_t intensity=lambert(n, lightDir);
                std::vector<uint8_t> flat(pts2d.size(), intensity);
                draw.fill_polygon_shaded(pts2d, depths, flat);
            }
            else if (mode=="phong") {
                // Approximate view dir as from eye to centroid
                Point3D centroid=poly.centroid();
                Point3D viewDir=(camera.eye-centroid).normalized();
                uint8_t intensity=phong(n, lightDir, viewDir);
                std::vector<uint8_t> flat(pts2d.size(), intensity);
                draw.fill_polygon_shaded(pts2d, depths, flat);
            }
            else if (mode=="gouraud") {
                // Estimate vertex normals ≈ face normal for now
                std::vector<Point3D> normals(poly.vertices.size(), n);
                auto intensities=gouraud(poly.vertices,normals,lightDir);
                draw.fill_polygon_shaded(pts2d, depths, intensities);
            }

            draw.polygon(pts2d, 255); // outline
        }
    }
};
