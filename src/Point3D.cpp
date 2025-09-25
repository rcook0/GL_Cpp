#include "Point3D.hpp"

Point3D::Point3D(double x, double y, double z) : x(x), y(y), z(z) {}

Point3D Point3D::operator+(const Point3D& other) const {
    return Point3D(x + other.x, y + other.y, z + other.z);
}

Point3D Point3D::operator-(const Point3D& other) const {
    return Point3D(x - other.x, y - other.y, z - other.z);
}

Point3D Point3D::operator*(double factor) const {
    return Point3D(x * factor, y * factor, z * factor);
}

Point3D Point3D::operator/(double divisor) const {
    return Point3D(x / divisor, y / divisor, z / divisor);
}

double Point3D::dot(const Point3D& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Point3D Point3D::cross(const Point3D& other) const {
    return Point3D(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

double Point3D::length() const {
    return std::sqrt(x*x + y*y + z*z);
}

Point3D Point3D::normalized() const {
    double len = length();
    return (len > 0) ? Point3D(x/len, y/len, z/len) : Point3D(0,0,0);
}

double Point3D::distance_to(const Point3D& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    double dz = z - other.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

std::ostream& operator<<(std::ostream& os, const Point3D& p) {
    os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}
