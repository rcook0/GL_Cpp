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

// -------- Cube-sphere generator --------
// subdiv = number of subdivisions per cube edge (>=1).
// radius = sphere radius.
inline Mesh3D make_cube_sphere(int subdiv=8, double radius=1.0) {
    Mesh3D mesh;
    if (subdiv < 1) subdiv = 1;

    auto normalize = [&](double x,double y,double z){
        double len = std::sqrt(x*x + y*y + z*z);
        return Point3D(x/len*radius, y/len*radius, z/len*radius);
    };

    // Each face of cube spans [-1,1] in 2D grid.
    // We'll generate vertices face by face, but deduplicate across edges if you like.
    // Here, simpler: each face makes its own verts, so some duplication (fine for rendering).

    auto buildFace = [&](int axis, int dir, int faceIndex) {
        // axis: 0=x,1=y,2=z; dir: +1 or -1
        // faceIndex: 0..5 (for cube map layout later if needed)

        int base = (int)mesh.vertices.size();
        for (int i=0; i<=subdiv; ++i) {
            double u = (double)i/subdiv;
            double uu = -1.0 + 2.0*u;
            for (int j=0; j<=subdiv; ++j) {
                double v = (double)j/subdiv;
                double vv = -1.0 + 2.0*v
                double x=0,y=0,z=0;
                if (axis==0) { x = dir; y=u; z=v; }
                if (axis==1) { y = dir; x=u; z=v; }
                if (axis==2) { z = dir; x=u; y=v; }
                Point3D pos = normalize(x,y,z);
                Point2D tex(u,v);
                mesh.add_vertex(pos, tex);
            }
        }

        for (int i=0; i<subdiv; ++i) {
            for (int j=0; j<subdiv; ++j) {
                int v0 = base + i*(subdiv+1) + j;
                int v1 = v0 + 1;
                int v2 = v0 + (subdiv+1);
                int v3 = v2 + 1;
                mesh.add_face({v0,v1,v3});
                mesh.add_face({v0,v3,v2});
            }
        }
    };

    buildFace(0,1);  buildFace(0,-1);
    buildFace(1,1);  buildFace(1,-1);
    buildFace(2,1);  buildFace(2,-1);

    return mesh;
}

inline Mesh3D make_cube(double w=1.0, double h=1.0, double d=1.0) {
    Mesh3D mesh;
    double x=w*0.5, y=h*0.5, z=d*0.5;

    int v0 = mesh.add_vertex(Point3D(-x,-y,-z), Point2D(0,0));
    int v1 = mesh.add_vertex(Point3D( x,-y,-z), Point2D(1,0));
    int v2 = mesh.add_vertex(Point3D( x, y,-z), Point2D(1,1));
    int v3 = mesh.add_vertex(Point3D(-x, y,-z), Point2D(0,1));

    int v4 = mesh.add_vertex(Point3D(-x,-y, z), Point2D(0,0));
    int v5 = mesh.add_vertex(Point3D( x,-y, z), Point2D(1,0));
    int v6 = mesh.add_vertex(Point3D( x, y, z), Point2D(1,1));
    int v7 = mesh.add_vertex(Point3D(-x, y, z), Point2D(0,1));

    // 12 triangles
    mesh.add_face({v0,v1,v2}); mesh.add_face({v0,v2,v3}); // back
    mesh.add_face({v4,v6,v5}); mesh.add_face({v4,v7,v6}); // front
    mesh.add_face({v0,v4,v5}); mesh.add_face({v0,v5,v1}); // bottom
    mesh.add_face({v3,v2,v6}); mesh.add_face({v3,v6,v7}); // top
    mesh.add_face({v1,v5,v6}); mesh.add_face({v1,v6,v2}); // right
    mesh.add_face({v0,v3,v7}); mesh.add_face({v0,v7,v4}); // left

    return mesh;
}

inline Mesh3D transform_mesh(const Mesh3D& m, const Transformation3D& T) {
    Mesh3D out = m;
    for (auto& v : out.vertices) v = T.apply(v);
    return out;
}

inline Mesh3D make_cube_grid(int M, int N,
                             double cubeW=1.0, double cubeH=1.0, double cubeD=1.0,
                             double spacing=0.0) {
    Mesh3D grid;
    Mesh3D baseCube = make_cube(cubeW, cubeH, cubeD);

    double stepX = cubeW + spacing;
    double stepY = cubeH + spacing;

    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            // translate each cube to its grid position
            Transformation3D T = Transformation3D::translation(
                i*stepX, 0, j*stepY
            );
            Mesh3D cube = transform_mesh(baseCube, T);

            // append vertices + faces into grid mesh
            int base = (int)grid.vertices.size();
            grid.vertices.insert(grid.vertices.end(), cube.vertices.begin(), cube.vertices.end());
            grid.uv.insert(grid.uv.end(), cube.uv.begin(), cube.uv.end());
            for (auto f : cube.faces) {
                for (int& idx : f.indices) idx += base;
                grid.faces.push_back(f);
            }
        }
    }
    return grid;
}


