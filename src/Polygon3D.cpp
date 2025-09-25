#include "Polygon3D.hpp"
#include <cmath>

Polygon3D::Polygon3D(const std::vector<Point3D>& verts) : vertices(verts) {}

void Polygon3D::add_vertex(const Point3D& v) {
    vertices.push_back(v);
}

size_t Polygon3D::size() const {
    return vertices.size();
}

Line3D Polygon3D::edge(size_t i) const {
    if (vertices.empty()) return Line3D();
    Point3D a = vertices[i % vertices.size()];
    Point3D b = vertices[(i+1) % vertices.size()];
    return Line3D(a, b);
}

Point3D Polygon3D::centroid() const {
    if (vertices.empty()) return Point3D();
    double cx=0, cy=0, cz=0;
    for (const auto& v : vertices) {
        cx += v.x;
        cy += v.y;
        cz += v.z;
    }
    double n = static_cast<double>(vertices.size());
    return Point3D(cx/n, cy/n, cz/n);
}

Point3D Polygon3D::normal() const {
    if (vertices.size() < 3) return Point3D(0,0,0);
    Point3D v1 = vertices[1] - vertices[0];
    Point3D v2 = vertices[2] - vertices[0];
    return v1.cross(v2).normalized();
}

double Polygon3D::area() const {
    if (vertices.size() < 3) return 0.0;
    // Project polygon onto plane perpendicular to normal, sum triangle areas
    Point3D n = normal();
    double total = 0.0;
    for (size_t i = 1; i+1 < vertices.size(); i++) {
        Point3D v0 = vertices[0];
        Point3D v1 = vertices[i];
        Point3D v2 = vertices[i+1];
        Point3D crossProd = (v1 - v0).cross(v2 - v0);
        total += 0.5 * n.dot(crossProd.normalized()) * crossProd.length();
    }
    return total;
}

std::ostream& operator<<(std::ostream& os, const Polygon3D& poly) {
    os << "Polygon3D[";
    for (size_t i = 0; i < poly.vertices.size(); ++i) {
        os << poly.vertices[i];
        if (i + 1 < poly.vertices.size()) os << ", ";
    }
    os << "]";
    return os;
}
