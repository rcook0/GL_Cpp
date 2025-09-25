#pragma once
#include "Point2D.hpp"
#include <array>
#include <iostream>

class Transformation2D {
public:
    // 3x3 matrix, row-major
    std::array<std::array<double, 3>, 3> m;

    Transformation2D();                    // identity
    static Transformation2D identity();

    // Factories
    static Transformation2D translation(double dx, double dy);
    static Transformation2D scaling(double sx, double sy);
    static Transformation2D rotation(double angleRadians);

    // Apply to a point
    Point2D apply(const Point2D& p) const;

    // Composition
    Transformation2D operator*(const Transformation2D& other) const;

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Transformation2D& t);
};
