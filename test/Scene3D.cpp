#include "Scene3D.hpp"
#include <iostream>

int main() {
    Projection3D proj(ProjectionType::PERSPECTIVE, 90.0 * M_PI/180.0);
    Scene3D scene(proj);

    Polygon3D tri({
        Point3D(0,0,5),
        Point3D(1,0,5),
        Point3D(0,1,5)
    });

    scene.add_object(tri);

    std::cout << scene << "\n";

    auto projected = scene.project_to_2d();
    for (const auto& poly2d : projected) {
        std::cout << "Polygon2D: ";
        for (const auto& p : poly2d) {
            std::cout << p << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
