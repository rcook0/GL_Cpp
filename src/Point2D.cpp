#include "Point2D.hpp"

Point2D::Point2D(double x, double y) : x(x), y(y) {}

Point2D Point2D::operator+(const Point2D& other) const {
    return Point2D(x + other.x, y + other.y);
}

Point2D Point2D::operator-(const Point2D& other) const {
    return Point2D(x - other.x, y - other.y);
}

Point2D Point2D::operator*(double factor) const {
    return Point2D(x * factor, y * factor);
}

Point2D Point2D::operator/(double divisor) const {
    return Point2D(x / divisor, y / divisor);
}

double Point2D::dot(const Point2D& other) const {
    return x * other.x + y * other.y;
}

double Point2D::length() const {
    return std::sqrt(x*x + y*y);
}

Point2D Point2D::normalized() const {
    double len = length();
    return (len > 0) ? Point2D(x/len, y/len) : Point2D(0,0);
}

double Point2D::distance_to(const Point2D& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx*dx + dy*dy);
}

std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}
