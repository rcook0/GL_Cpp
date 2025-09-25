#pragma once
#include "Point3D.hpp"
#include <algorithm>
#include <cmath>

// Clamp helper
inline uint8_t clamp255(double v) {
    return static_cast<uint8_t>(std::max(0.0, std::min(255.0, v)));
}

// Lambertian diffuse shading
inline uint8_t lambert(const Point3D& normal, const Point3D& lightDir, uint8_t base=200) {
    double nDotL = std::max(0.0, normal.normalized().dot(lightDir.normalized()));
    return clamp255(base * nDotL);
}

// Phong shading (diffuse + specular)
inline uint8_t phong(const Point3D& normal,
                     const Point3D& lightDir,
                     const Point3D& viewDir,
                     double shininess=16,
                     uint8_t base=200) {
    Point3D n = normal.normalized();
    Point3D l = lightDir.normalized();
    Point3D v = viewDir.normalized();
    Point3D r = (n * (2.0 * n.dot(l)) - l).normalized(); // reflection

    double diff = std::max(0.0, n.dot(l));
    double spec = std::pow(std::max(0.0, r.dot(v)), shininess);

    double intensity = 0.6*diff + 0.4*spec; // weighted sum
    return clamp255(base * intensity);
}

// Gouraud shading (per-vertex, then interpolated during rasterization)
// We return a vector of intensities for each vertex
inline std::vector<uint8_t> gouraud(const std::vector<Point3D>& verts,
                                    const std::vector<Point3D>& normals,
                                    const Point3D& lightDir,
                                    uint8_t base=200) {
    std::vector<uint8_t> intensities;
    intensities.reserve(verts.size());
    for (size_t i=0;i<verts.size();++i) {
        double nDotL = std::max(0.0, normals[i].normalized().dot(lightDir.normalized()));
        intensities.push_back(clamp255(base * nDotL));
    }
    return intensities;
}