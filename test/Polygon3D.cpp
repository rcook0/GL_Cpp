#include "Polygon3D.hpp"
#include <iostream>

int main() {
    Polygon3D tri({
        Point3D(0,0,0),
        Point3D(1,0,0),
        Point3D(0,1,0)
    });

    std::cout << tri << "\n";
    std::cout << "Centroid: " << tri.centroid() << "\n";
    std::cout << "Normal: " << tri.normal() << "\n";
    std::cout << "Area: " << tri.area() << "\n";
}
