#include "Point2D.hpp"
#include "Point3D.hpp"
#include <iostream>

int main() {
    Point2D a(1,2), b(3,4);
    std::cout << "2D add: " << (a+b) << "\n";
    std::cout << "2D dist: " << a.distance_to(b) << "\n";

    Point3D p(1,0,0), q(0,1,0);
    std::cout << "3D cross: " << p.cross(q) << "\n";
    std::cout << "3D dot: " << p.dot(q) << "\n";

    return 0;
}
