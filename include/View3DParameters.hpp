#pragma once
#include "View3D.hpp"
#include "Projection3D.hpp"
#include <iostream>

class View3DParameters {
public:
    // Camera
    Point3D eye;
    Point3D target;
    Point3D up;

    // Projection
    double fov;      // field of view in radians
    double aspect;   // aspect ratio (width/height)
    double nearZ;
    double farZ;

    View3DParameters(const Point3D& eye = Point3D(0,0,5),
                     const Point3D& target = Point3D(0,0,0),
                     const Point3D& up = Point3D(0,1,0),
                     double fovRadians = 90.0 * M_PI/180.0,
                     double aspect = 1.0,
                     double nearZ = 0.1,
                     double farZ = 1000.0);

    View3D make_view() const;
    Projection3D make_projection() const;

    friend std::ostream& operator<<(std::ostream& os, const View3DParameters& v);
};
