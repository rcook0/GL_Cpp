#pragma once
#include "Mesh3D.hpp"
#include <cmath>
#include <map>
#include <tuple>

// rows = latitude segments (>=2), cols = longitude segments (>=3)
inline Mesh3D make_uv_sphere(int rows, int cols, double radius=1.0) {
    Mesh3D m;
    if (rows < 2) rows = 2;
    if (cols < 3) cols = 3;

    // Build grid of (rows+1) x (cols+1) vertices (wrap last column)
    for (int i=0; i<=rows; ++i) {
        double v = (double)i / rows;            // [0..1]
        double theta = v * M_PI;                // 0..pi (latitude)
        double sinT = std::sin(theta), cosT = std::cos(theta);

        for (int j=0; j<=cols; ++j) {
            double u = (double)j / cols;        // [0..1]
            double phi = u * 2.0 * M_PI;        // 0..2pi (longitude)
            double sinP = std::sin(phi), cosP = std::cos(phi);

            double x = radius * sinT * cosP;
            double y = radius * cosT;
            double z = radius * sinT * sinP;
            m.add_vertex(Point3D(x,y,z));
        }
    }

    auto idx = [&](int i, int j) { return i*(cols+1) + j; };

    // Faces as quads between rings
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j) {
            int v00 = idx(i,   j);
            int v01 = idx(i,   j+1);
            int v10 = idx(i+1, j);
            int v11 = idx(i+1, j+1);

            // Skip degenerates at extreme poles if you prefer triangles,
            // but Newell + quads are fine for this scanline renderer.
            m.add_face({v00, v01, v11, v10});
        }
    }
    return m;
}

// ------------------- Icosphere -------------------

namespace {
    // Hash for edge midpoint lookup
    struct EdgeKey {
        int v0, v1;
        EdgeKey(int a, int b) { v0 = std::min(a,b); v1 = std::max(a,b); }
        bool operator<(const EdgeKey& other) const {
            return std::tie(v0,v1) < std::tie(other.v0,other.v1);
        }
    };
}

// Create an icosahedron base
inline Mesh3D make_icosahedron(double radius=1.0) {
    Mesh3D mesh;
    const double phi = (1.0 + std::sqrt(5.0)) * 0.5;
    const double invLen = 1.0 / std::sqrt(1 + phi*phi);

    std::vector<Point3D> verts = {
        {-1,  phi, 0}, {1,  phi, 0}, {-1, -phi, 0}, {1, -phi, 0},
        {0, -1,  phi}, {0,  1,  phi}, {0, -1, -phi}, {0,  1, -phi},
        { phi, 0, -1}, { phi, 0, 1}, {-phi, 0, -1}, {-phi, 0, 1}
    };
    for (auto& v : verts) {
        Point3D n = v * invLen;        // normalize unit length
        mesh.add_vertex(Point3D(n.x*radius, n.y*radius, n.z*radius));
    }

    std::vector<std::array<int,3>> faces = {
        {0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},
        {1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},
        {3,9,4},{3,4,2},{3,2,6},{3,6,8},{3,8,9},
        {4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}
    };
    for (auto& f : faces) mesh.add_face({f[0],f[1],f[2]});
    return mesh;
}

// Subdivide each triangle of a mesh
inline Mesh3D subdivide_icosphere(const Mesh3D& mesh, int level, double radius) {
    Mesh3D result;
    result.vertices = mesh.vertices;
    result.faces.clear();

    for (int l=0; l<level; ++l) {
        std::map<EdgeKey,int> midpointCache;
        std::vector<Face> newFaces;

        for (const auto& f : (l==0 ? mesh.faces : result.faces)) {
            int v0=f.indices[0], v1=f.indices[1], v2=f.indices[2];

            auto midpoint = [&](int a, int b) {
                EdgeKey key(a,b);
                auto it = midpointCache.find(key);
                if (it!=midpointCache.end()) return it->second;

                Point3D pa = result.vertices[a];
                Point3D pb = result.vertices[b];
                Point3D pm = (pa+pb)*0.5;
                double len = std::sqrt(pm.x*pm.x + pm.y*pm.y + pm.z*pm.z);
                pm = Point3D(pm.x/len*radius, pm.y/len*radius, pm.z/len*radius);
                int idx = (int)result.vertices.size();
                result.vertices.push_back(pm);
                midpointCache[key]=idx;
                return idx;
            };

            int a = midpoint(v0,v1);
            int b = midpoint(v1,v2);
            int c = midpoint(v2,v0);

            newFaces.push_back({{v0,a,c}});
            newFaces.push_back({{v1,b,a}});
            newFaces.push_back({{v2,c,b}});
            newFaces.push_back({{a,b,c}});
        }
        result.faces = newFaces;
    }
    return result;
}

// Make an icosphere with N subdivision levels
inline Mesh3D make_icosphere(int subdivisions=2, double radius=1.0) {
    Mesh3D ico = make_icosahedron(radius);
    if (subdivisions<=0) return ico;
    return subdivide_icosphere(ico, subdivisions, radius);
}
