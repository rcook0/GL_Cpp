#include "Projection3D.hpp"
#include <iostream>

int main() {
    Projection3D ortho(ProjectionType::ORTHOGRAPHIC);
    Projection3D persp(ProjectionType::PERSPECTIVE, 90.0 * M_PI/180.0);

    Point3D p(1, 1, 5);

    std::cout << "Ortho: " << ortho.project(p) << "\n";
    std::cout << "Persp: " << persp.project(p) << "\n";

    return 0;
}
