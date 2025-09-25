#pragma once
#include "Point3D.hpp"
#include "Line3D.hpp"
#include <vector>
#include <iostream>

class Polygon3D {
public:
    std::vector<Point3D> vertices;

    Polygon3D() = default;
    Polygon3D(const std::vector<Point3D>& verts);

    void add_vertex(const Point3D& v);
    size_t size() const;

    Line3D edge(size_t i) const;     // edge from vertex i to i+1 (wraps around)
    Point3D centroid() const;        // average of vertices
    Point3D normal() const;          // approximate normal (first three vertices)

    double area() const;             // simple polygon area (planar assumption)

    // Output
    friend std::ostream& operator<<(std::ostream& os, const Polygon3D& poly);
};
