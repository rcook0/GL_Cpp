#pragma once
#include "Mesh3D.hpp"
#include <cmath>

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
