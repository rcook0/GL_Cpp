#pragma once
#include "Point2D.hpp"
#include "Point3D.hpp"
#include <iostream>

enum class ProjectionType {
    ORTHOGRAPHIC,
    PERSPECTIVE
};

class Projection3D {
public:
    ProjectionType type;
    double fov;      // field of view (radians) for perspective
    double nearZ;    // near clipping plane
    double farZ;     // far clipping plane

    Projection3D(ProjectionType type = ProjectionType::ORTHOGRAPHIC,
                 double fovRadians = 90.0 * M_PI/180.0,
                 double nearZ = 0.1,
                 double farZ = 1000.0);

    Point2D project(const Point3D& p) const;

    friend std::ostream& operator<<(std::ostream& os, const Projection3D& proj);
};
