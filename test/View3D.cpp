#include "View3D.hpp"
#include "Scene3D.hpp"
#include <iostream>

int main() {
    View3D camera(Point3D(0,0,5), Point3D(0,0,0), Point3D(0,1,0));
    Transformation3D viewMat = camera.view_matrix();

    Polygon3D tri({
        Point3D(-1,-1,0),
        Point3D(1,-1,0),
        Point3D(0,1,0)
    });

    Scene3D scene(Projection3D(ProjectionType::PERSPECTIVE));
    scene.add_object(tri);

    // Apply the view transformation
    scene.apply_transformation(viewMat);

    auto projected = scene.project_to_2d();
    for (const auto& poly : projected) {
        std::cout << "Projected polygon: ";
        for (const auto& p : poly) std::cout << p << " ";
        std::cout << "\n";
    }

    return 0;
}
