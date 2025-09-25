#pragma once
#include "Point3D.hpp"
#include <iostream>

class Line3D {
public:
    Point3D p1, p2;

    Line3D(const Point3D& a = Point3D(), const Point3D& b = Point3D());

    double length() const;
    Point3D midpoint() const;
    Point3D direction() const;         // unit vector from p1 to p2
    Point3D interpolate(double t) const; // linear interpolation: t=0 → p1, t=1 → p2

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Line3D& l);
};
