#pragma once
#include "Point3D.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

// Clamp helpers
inline uint8_t clamp255(double v) {
    return static_cast<uint8_t>(std::max(0.0, std::min(255.0, v)));
}
inline double clamp01(double x){ return std::max(0.0, std::min(1.0, x)); }
inline uint8_t to_u8(double x){ return (uint8_t)std::round(255.0 * clamp01(x)); }


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

// Diffuse only (0..1)
inline double lambert01(const Point3D& n, const Point3D& l) {
    return clamp01(n.normalized().dot(l.normalized()));
}

// Phong (0..1): kd*diff + ks*spec
inline double phong01(const Point3D& nIn,
                      const Point3D& lIn,
                      const Point3D& vIn,
                      double kd=0.7, double ks=0.3, double shininess=16.0) {
    Point3D n = nIn.normalized();
    Point3D l = lIn.normalized();
    Point3D v = vIn.normalized();
    double diff = std::max(0.0, n.dot(l));
    Point3D r = (n * (2.0 * n.dot(l)) - l).normalized();
    double spec = std::pow(std::max(0.0, r.dot(v)), shininess);
    return clamp01(kd*diff + ks*spec);
}

// Gouraud vertex intensities (0..1)
inline std::vector<double> gouraud01(const std::vector<Point3D>& verts,
                                     const std::vector<Point3D>& normals,
                                     const Point3D& lightDir) {
    std::vector<double> I; I.reserve(verts.size());
    for (size_t i=0;i<verts.size();++i) {
        I.push_back(lambert01(normals[i], lightDir));
    }
    return I;
}
