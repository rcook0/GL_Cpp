#include "View3DParameters.hpp"
#include "Scene3D.hpp"
#include <iostream>

int main() {
    View3DParameters params(Point3D(0,0,5), Point3D(0,0,0), Point3D(0,1,0),
                            60.0 * M_PI/180.0, 1.0, 0.1, 100.0);

    View3D camera = params.make_view();
    Projection3D proj = params.make_projection();

    Scene3D scene(proj);
    scene.add_object(Polygon3D({
        Point3D(-1,-1,0),
        Point3D(1,-1,0),
        Point3D(0,1,0)
    }));

    // Apply view
    scene.apply_transformation(camera.view_matrix());

    auto projected = scene.project_to_2d();
    for (const auto& poly : projected) {
        std::cout << "Projected poly: ";
        for (const auto& p : poly) std::cout << p << " ";
        std::cout << "\n";
    }

    return 0;
}
