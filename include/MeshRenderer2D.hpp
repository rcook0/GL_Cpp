#pragma once
#include "Mesh3D.hpp"
#include "View3D.hpp"
#include "Projection3D.hpp"
#include "Drawing2D.hpp"
#include "Shading.hpp"

class MeshRenderer2D {
public:
    RasterBuffer<uint8_t>& rb;
    View3D& camera;
    Projection3D& projection;
    Point3D lightDir;

    MeshRenderer2D(RasterBuffer<uint8_t>& rb, View3D& cam, Projection3D& proj,
                   const Point3D& lightDir = Point3D(0,0,-1))
        : rb(rb), camera(cam), projection(proj), lightDir(lightDir) {}

    // Renders a mesh with Gouraud shading (per-vertex)
    void render_gouraud(const Mesh3D& mesh,
                        const std::vector<Point3D>& vertexNormals,
                        bool drawWire=true) {
        Drawing2D draw(rb);

        // NOTE: We assume the mesh's vertices are already in VIEW space
        // (i.e., world points transformed by camera.view_matrix()).
        // Then we project to NDC and viewport-map.

        auto viewport = [&](const Point2D& p) {
            double x=(p.x+1.0)*0.5*rb.width;
            double y=(1.0-(p.y+1.0)*0.5)*rb.height;
            return Point2D(x,y);
        };

        for (const auto& face : mesh.faces) {
            if (face.indices.size() < 3) continue;

            // Backface culling from face normal
            Polygon3D poly = mesh.to_polygon(face);
            Point3D n = poly.normal();      // view-space normal
            if (n.z >= 0) continue;         // cull away-facing

            // Project and collect per-vertex data
            std::vector<Point2D> pts2d; pts2d.reserve(face.indices.size());
            std::vector<double> depths; depths.reserve(face.indices.size());
            std::vector<Point3D> vpos; vpos.reserve(face.indices.size());
            std::vector<Point3D> vnorm; vnorm.reserve(face.indices.size());

            for (int vi : face.indices) {
                const Point3D& v = mesh.vertices[vi];
                vpos.push_back(v);
                vnorm.push_back(vertexNormals[vi]);

                Point2D p2 = projection.project(v); // NDC
                pts2d.push_back(viewport(p2));
                depths.push_back(v.z);
            }

            // Gouraud intensities per vertex using true vertex normals
            auto intensities = gouraud(vpos, vnorm, lightDir);

            // Fill with depth + shaded intensity
            draw.fill_polygon_shaded(pts2d, depths, intensities);

            if (drawWire) draw.polygon(pts2d, 255);
        }
    }

    void render_phong(const Mesh3D& mesh,
                  const std::vector<Point3D>& vertexNormals,
                  bool drawWire=true,
                  uint8_t base=220, double kd=0.7, double ks=0.3, double shininess=24.0) {
    Drawing2D draw(rb);

    auto viewport=[&](const Point2D& p){
        double x=(p.x+1.0)*0.5*rb.width;
        double y=(1.0-(p.y+1.0)*0.5)*rb.height;
        return Point2D(x,y);
    };

    for (const auto& face : mesh.faces) {
        if (face.indices.size()<3) continue;

        Polygon3D poly = mesh.to_polygon(face);
        Point3D n = poly.normal();        // view-space face normal
        if (n.z >= 0) continue;           // backface cull

        std::vector<Point2D> pts2d;
        std::vector<Point3D> vpos;
        std::vector<Point3D> vnorm;

        for (int vi : face.indices) {
            const Point3D& P = mesh.vertices[vi];
            const Point3D& N = vertexNormals[vi];
            vpos.push_back(P);
            vnorm.push_back(N);
            pts2d.push_back(viewport(projection.project(P)));
        }

        draw.fill_polygon_phong(pts2d, vpos, vnorm,
                                base, kd, ks, shininess, lightDir);

        if (drawWire) draw.polygon(pts2d, 255);
    }
}
};
