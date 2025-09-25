#pragma once
#include <cmath>
#include <iostream>

class Point2D {
public:
    double x, y;

    Point2D(double x=0.0, double y=0.0);

    // Operators
    Point2D operator+(const Point2D& other) const;
    Point2D operator-(const Point2D& other) const;
    Point2D operator*(double factor) const;
    Point2D operator/(double divisor) const;

    // Utilities
    double dot(const Point2D& other) const;
    double length() const;
    Point2D normalized() const;
    double distance_to(const Point2D& other) const;

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Point2D& p);
};
