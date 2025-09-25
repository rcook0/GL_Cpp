#pragma once
#include "Point3D.hpp"
#include "Transformation3D.hpp"
#include <iostream>

class View3D {
public:
    Point3D eye;       // camera position
    Point3D target;    // look-at point
    Point3D up;        // up direction

    View3D(const Point3D& eye = Point3D(0,0,5),
           const Point3D& target = Point3D(0,0,0),
           const Point3D& up = Point3D(0,1,0));

    // Build view transformation (world → camera coordinates)
    Transformation3D view_matrix() const;

    friend std::ostream& operator<<(std::ostream& os, const View3D& v);
};
