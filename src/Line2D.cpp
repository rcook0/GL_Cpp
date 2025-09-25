#include "Line2D.hpp"
#include <cmath>

Line2D::Line2D(const Point2D& a, const Point2D& b) : p1(a), p2(b) {}

double Line2D::length() const {
    return p1.distance_to(p2);
}

Point2D Line2D::midpoint() const {
    return Point2D((p1.x + p2.x)/2.0, (p1.y + p2.y)/2.0);
}

Point2D Line2D::direction() const {
    Point2D v = p2 - p1;
    return v.normalized();
}

Point2D Line2D::interpolate(double t) const {
    return Point2D(p1.x + t*(p2.x - p1.x), p1.y + t*(p2.y - p1.y));
}

std::ostream& operator<<(std::ostream& os, const Line2D& l) {
    os << "[" << l.p1 << " -> " << l.p2 << "]";
    return os;
}
