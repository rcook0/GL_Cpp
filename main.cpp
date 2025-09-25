#include "Point2D.hpp"
#include "Point3D.hpp"
#include "Line2D.hpp"
#include "Line3D.hpp"
#include "Polygon3D.hpp"
#include <iostream>

int main() {
    Polygon3D tri({
        Point3D(0,0,0),
        Point3D(1,0,0),
        Point3D(0,1,0)
    });
    
    Point2D a(1,2), b(3,4);
    std::cout << "2D add: " << (a+b) << "\n";
    std::cout << "2D dist: " << a.distance_to(b) << "\n";

    Point3D p(1,0,0), q(0,1,0);
    std::cout << "3D cross: " << p.cross(q) << "\n";
    std::cout << "3D dot: " << p.dot(q) << "\n";

    Line2D l2(Point2D(0,0), Point2D(3,4));
    std::cout << "Line2D: " << l2 << " length=" << l2.length() << "\n";

    Line3D l3(Point3D(0,0,0), Point3D(0,3,4));
    std::cout << "Line3D: " << l3 << " length=" << l3.length() << "\n";

    std::cout << tri << "\n";
    std::cout << "Centroid: " << tri.centroid() << "\n";
    std::cout << "Normal: " << tri.normal() << "\n";
    std::cout << "Area: " << tri.area() << "\n";

    Point2D p1(1,0);
    Transformation2D rot = Transformation2D::rotation(M_PI/2);
    Transformation2D trans = Transformation2D::translation(2,3);

    Point2D pRot = rot.apply(p1);
    Point2D pTrans = trans.apply(pRot);

    std::cout << "Original: " << p1 << "\n";
    std::cout << "Rotated: " << pRot << "\n";
    std::cout << "Translated: " << pTrans << "\n";
    
    return 0;
}
