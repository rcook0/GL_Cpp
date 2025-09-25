#include "Transformation2D.hpp"
#include <cmath>

Transformation2D::Transformation2D() {
    m = {{{1,0,0},{0,1,0},{0,0,1}}};
}

Transformation2D Transformation2D::identity() {
    return Transformation2D();
}

Transformation2D Transformation2D::translation(double dx, double dy) {
    Transformation2D t;
    t.m = {{{1,0,dx},{0,1,dy},{0,0,1}}};
    return t;
}

Transformation2D Transformation2D::scaling(double sx, double sy) {
    Transformation2D t;
    t.m = {{{sx,0,0},{0,sy,0},{0,0,1}}};
    return t;
}

Transformation2D Transformation2D::rotation(double angleRadians) {
    double c = std::cos(angleRadians);
    double s = std::sin(angleRadians);
    Transformation2D t;
    t.m = {{{c,-s,0},{s,c,0},{0,0,1}}};
    return t;
}

Point2D Transformation2D::apply(const Point2D& p) const {
    double xNew = m[0][0]*p.x + m[0][1]*p.y + m[0][2]*1.0;
    double yNew = m[1][0]*p.x + m[1][1]*p.y + m[1][2]*1.0;
    return Point2D(xNew, yNew);
}

Transformation2D Transformation2D::operator*(const Transformation2D& other) const {
    Transformation2D result;
    for (int i=0; i<3; ++i) {
        for (int j=0; j<3; ++j) {
            result.m[i][j] = 0;
            for (int k=0; k<3; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Transformation2D& t) {
    os << "[";
    for (int i=0; i<3; ++i) {
        os << "[";
        for (int j=0; j<3; ++j) {
            os << t.m[i][j];
            if (j<2) os << ", ";
        }
        os << "]";
        if (i<2) os << ", ";
    }
    os << "]";
    return os;
}
