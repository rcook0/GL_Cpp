#include "Transformation3D.hpp"
#include <iostream>

int main() {
    Point3D p(1,0,0);

    auto rotZ = Transformation3D::rotation_z(M_PI/2);
    auto trans = Transformation3D::translation(0, 0, 5);

    Point3D pRot = rotZ.apply(p);
    Point3D pFinal = trans.apply(pRot);

    std::cout << "Original: " << p << "\n";
    std::cout << "Rotated Z: " << pRot << "\n";
    std::cout << "Rotated+Translated: " << pFinal << "\n";

    return 0;
}
