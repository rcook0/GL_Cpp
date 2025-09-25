#pragma once
#include <cmath>
#include <iostream>

class Point3D {
public:
    double x, y, z;

    Point3D(double x=0.0, double y=0.0, double z=0.0);

    // Operators
    Point3D operator+(const Point3D& other) const;
    Point3D operator-(const Point3D& other) const;
    Point3D operator*(double factor) const;
    Point3D operator/(double divisor) const;

    // Utilities
    double dot(const Point3D& other) const;
    Point3D cross(const Point3D& other) const;
    double length() const;
    Point3D normalized() const;
    double distance_to(const Point3D& other) const;

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Point3D& p);
};
