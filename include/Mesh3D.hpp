#pragma once
#include "Point3D.hpp"
#include "Polygon3D.hpp"
#include <vector>
#include <numeric>

struct Face {
    // triangle or quad (N-gon allowed)
    std::vector<int> indices; // indices into Mesh3D::vertices
};

class Mesh3D {
public:
    std::vector<Point3D> vertices;
    std::vector<Face>    faces;

    int add_vertex(const Point3D& v) {
        vertices.push_back(v);
        return (int)vertices.size()-1;
    }

    void add_face(const std::vector<int>& idx) {
        if (idx.size() >= 3) faces.push_back(Face{idx});
    }

    // Face normal (un-normalized; good for area-weighted accumulate)
    Point3D face_normal(const Face& f) const {
        // Newell’s method for robustness on N-gons
        double nx=0, ny=0, nz=0;
        const auto& idx = f.indices;
        size_t n = idx.size();
        for (size_t i=0; i<n; ++i) {
            const Point3D& a = vertices[idx[i]];
            const Point3D& b = vertices[idx[(i+1)%n]];
            nx += (a.y - b.y)*(a.z + b.z);
            ny += (a.z - b.z)*(a.x + b.x);
            nz += (a.x - b.x)*(a.y + b.y);
        }
        return Point3D(nx, ny, nz); // not normalized
    }

    // Area-weighted vertex normals (sum of a#pragma once
#include "Point3D.hpp"
#include "Polygon3D.hpp"
#include <vector>
#include <numeric>

struct Face {
    // triangle or quad (N-gon allowed)
    std::vector<int> indices; // indices into Mesh3D::vertices
};

class Mesh3D {
public:
    std::vector<Point3D> vertices;
    std::vector<Face>    faces;

    int add_vertex(const Point3D& v) {
        vertices.push_back(v);
        return (int)vertices.size()-1;
    }

    void add_face(const std::vector<int>& idx) {
        if (idx.size() >= 3) faces.push_back(Face{idx});
    }

    // Face normal (un-normalized; good for area-weighted accumulate)
    Point3D face_normal(const Face& f) const {
        // Newell’s method for robustness on N-gons
        double nx=0, ny=0, nz=0;
        const auto& idx = f.indices;
        size_t n = idx.size();
        for (size_t i=0; i<n; ++i) {
            const Point3D& a = vertices[idx[i]];
            const Point3D& b = vertices[idx[(i+1)%n]];
            nx += (a.y - b.y)*(a.z + b.z);
            ny += (a.z - b.z)*(a.x + b.x);
            nz += (a.x - b.x)*(a.y + b.y);
        }
        return Point3D(nx, ny, nz); // not normalized
    }

    // Area-weighted vertex normals (sum of adjacent face normals, then normalize)
    std::vector<Point3D> compute_vertex_normals() const {
        std::vector<Point3D> accum(vertices.size(), Point3D(0,0,0));
        for (const auto& f : faces) {
            Point3D fn = face_normal(f);
            for (int vi : f.indices) accum[vi] = accum[vi] + fn;
        }
        for (auto& n : accum) n = n.normalized();
        return accum;
    }

    // Convenience: convert a face to a Polygon3D (expanded positions)
    Polygon3D to_polygon(const Face& f) const {
        std::vector<Point3D> poly;
        poly.reserve(f.indices.size());
        for (int vi : f.indices) poly.push_back(vertices[vi]);
        return Polygon3D(poly);
    }
};
djacent face normals, then normalize)
    std::vector<Point3D> compute_vertex_normals() const {
        std::vector<Point3D> accum(vertices.size(), Point3D(0,0,0));
        for (const auto& f : faces) {
            Point3D fn = face_normal(f);
            for (int vi : f.indices) accum[vi] = accum[vi] + fn;
        }
        for (auto& n : accum) n = n.normalized();
        return accum;
    }

    // Convenience: convert a face to a Polygon3D (expanded positions)
    Polygon3D to_polygon(const Face& f) const {
        std::vector<Point3D> poly;
        poly.reserve(f.indices.size());
        for (int vi : f.indices) poly.push_back(vertices[vi]);
        return Polygon3D(poly);
    }
};
