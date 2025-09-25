#pragma once
#include "Point3D.hpp"
#include <array>
#include <iostream>

class Transformation3D {
public:
    // 4x4 matrix, row-major
    std::array<std::array<double, 4>, 4> m;

    Transformation3D();                      // identity
    static Transformation3D identity();

    // Factories
    static Transformation3D translation(double dx, double dy, double dz);
    static Transformation3D scaling(double sx, double sy, double sz);
    static Transformation3D rotation_x(double angleRadians);
    static Transformation3D rotation_y(double angleRadians);
    static Transformation3D rotation_z(double angleRadians);

    // Apply to a point
    Point3D apply(const Point3D& p) const;

    // Composition
    Transformation3D operator*(const Transformation3D& other) const;

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Transformation3D& t);
};
