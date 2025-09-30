#pragma once
#include <memory>
#include "Point3d.hpp"
#include "Texture2D.hpp"

struct Material {
    Point3D baseColor {1.0, 1.0, 1.0};        // fallback diffuse color (0..1)
    std::shared_ptr<Texture2D> diffuseTex;    // optional color texture

    double kd = 0.7;       // diffuse weight
    double ks = 0.3;       // specular weight
    double shininess = 24; // specular exponent
};
