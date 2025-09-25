#include "Scene3D.hpp"

Scene3D::Scene3D(const Projection3D& proj) : projection(proj) {}

void Scene3D::add_object(const Polygon3D& poly) {
    objects.push_back(poly);
}

void Scene3D::apply_transformation(const Transformation3D& t) {
    for (auto& poly : objects) {
        for (auto& v : poly.vertices) {
            v = t.apply(v);
        }
    }
}

std::vector<std::vector<Point2D>> Scene3D::project_to_2d() const {
    std::vector<std::vector<Point2D>> projected;
    for (const auto& poly : objects) {
        if (poly.vertices.size() < 3) continue;

        // --- Backface culling ---
        Point3D n = poly.normal();
        // If normal.z >= 0 → facing away (skip)
        if (n.z >= 0) continue;

        std::vector<Point2D> verts2d;
        for (const auto& v : poly.vertices) {
            verts2d.push_back(projection.project(v));
        }
        projected.push_back(verts2d);
    }
    return projected;
}

std::ostream& operator<<(std::ostream& os, const Scene3D& scene) {
    os << "Scene3D with " << scene.objects.size() << " objects";
    return os;
}
