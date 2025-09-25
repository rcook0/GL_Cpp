#pragma once
#include "Polygon3D.hpp"
#include "Projection3D.hpp"
#include "Transformation3D.hpp"
#include <vector>
#include <iostream>

class Scene3D {
public:
    std::vector<Polygon3D> objects;   // all polygons in the scene
    Projection3D projection;          // camera projection

    Scene3D(const Projection3D& proj = Projection3D());

    void add_object(const Polygon3D& poly);

    // Apply a transformation to all objects in the scene
    void apply_transformation(const Transformation3D& t);

    // Project all polygons to 2D
    std::vector<std::vector<Point2D>> project_to_2d() const;

    friend std::ostream& operator<<(std::ostream& os, const Scene3D& scene);
};
