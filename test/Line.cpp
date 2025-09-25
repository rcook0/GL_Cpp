#include "Line2D.hpp"
#include "Line3D.hpp"
#include <iostream>

int main() {
    Line2D l2(Point2D(0,0), Point2D(3,4));
    std::cout << "Line2D: " << l2 << " length=" << l2.length() << "\n";

    Line3D l3(Point3D(0,0,0), Point3D(0,3,4));
    std::cout << "Line3D: " << l3 << " length=" << l3.length() << "\n";
}
