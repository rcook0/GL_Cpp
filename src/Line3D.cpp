#include "Line3D.hpp"
#include <cmath>

Line3D::Line3D(const Point3D& a, const Point3D& b) : p1(a), p2(b) {}

double Line3D::length() const {
    return p1.distance_to(p2);
}

Point3D Line3D::midpoint() const {
    return Point3D((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0, (p1.z + p2.z)/2.0);
}

Point3D Line3D::direction() const {
    Point3D v = p2 - p1;
    return v.normalized();
}

Point3D Line3D::interpolate(double t) const {
    return Point3D(
        p1.x + t*(p2.x - p1.x),
        p1.y + t*(p2.y - p1.y),
        p1.z + t*(p2.z - p1.z)
    );
}

std::ostream& operator<<(std::ostream& os, const Line3D& l) {
    os << "[" << l.p1 << " -> " << l.p2 << "]";
    return os;
}
