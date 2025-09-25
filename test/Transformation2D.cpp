#include "Transformation2D.hpp"
#include <iostream>

int main() {
    Point2D p(1,0);
    Transformation2D rot = Transformation2D::rotation(M_PI/2);
    Transformation2D trans = Transformation2D::translation(2,3);

    Point2D pRot = rot.apply(p);
    Point2D pTrans = trans.apply(pRot);

    std::cout << "Original: " << p << "\n";
    std::cout << "Rotated: " << pRot << "\n";
    std::cout << "Translated: " << pTrans << "\n";

    return 0;
}
