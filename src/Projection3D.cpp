#include "Projection3D.hpp"
#include <cmath>

Projection3D::Projection3D(ProjectionType type, double fovRadians,
                           double nearZ, double farZ)
    : type(type), fov(fovRadians), nearZ(nearZ), farZ(farZ) {}

Point2D Projection3D::project(const Point3D& p) const {
    if (type == ProjectionType::ORTHOGRAPHIC) {
        // Drop z, return x,y
        return Point2D(p.x, p.y);
    } else {
        // Perspective projection
        // Simple pinhole model: x' = (x / z) * scale, y' = (y / z) * scale
        double aspect = 1.0; // square viewport assumption
        double f = 1.0 / std::tan(fov / 2.0);

        if (p.z <= nearZ) {
            // Behind the near plane, just clamp
            return Point2D(0, 0);
        }

        double xProj = (p.x * f) / (aspect * p.z);
        double yProj = (p.y * f) / p.z;
        return Point2D(xProj, yProj);
    }
}

std::ostream& operator<<(std::ostream& os, const Projection3D& proj) {
    if (proj.type == ProjectionType::ORTHOGRAPHIC) {
        os << "Projection3D(ORTHOGRAPHIC)";
    } else {
        os << "Projection3D(PERSPECTIVE, fov=" << proj.fov
           << ", near=" << proj.nearZ << ", far=" << proj.farZ << ")";
    }
    return os;
}
