#pragma once
#include "Point2D.hpp"
#include <iostream>

class Line2D {
public:
    Point2D p1, p2;

    Line2D(const Point2D& a = Point2D(), const Point2D& b = Point2D());

    double length() const;
    Point2D midpoint() const;
    Point2D direction() const;         // unit vector from p1 to p2
    Point2D interpolate(double t) const; // linear interpolation: t=0 → p1, t=1 → p2

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Line2D& l);
};
