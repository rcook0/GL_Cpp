#include "Transformation3D.hpp"
#include <cmath>

Transformation3D::Transformation3D() {
    m = {{{{1,0,0,0}},
          {{0,1,0,0}},
          {{0,0,1,0}},
          {{0,0,0,1}}}};
}

Transformation3D Transformation3D::identity() {
    return Transformation3D();
}

Transformation3D Transformation3D::translation(double dx, double dy, double dz) {
    Transformation3D t;
    t.m = {{{{1,0,0,dx}},
            {{0,1,0,dy}},
            {{0,0,1,dz}},
            {{0,0,0,1}}}};
    return t;
}

Transformation3D Transformation3D::scaling(double sx, double sy, double sz) {
    Transformation3D t;
    t.m = {{{{sx,0,0,0}},
            {{0,sy,0,0}},
            {{0,0,sz,0}},
            {{0,0,0,1}}}};
    return t;
}

Transformation3D Transformation3D::rotation_x(double angleRadians) {
    double c = std::cos(angleRadians);
    double s = std::sin(angleRadians);
    Transformation3D t;
    t.m = {{{{1,0,0,0}},
            {{0,c,-s,0}},
            {{0,s,c,0}},
            {{0,0,0,1}}}};
    return t;
}

Transformation3D Transformation3D::rotation_y(double angleRadians) {
    double c = std::cos(angleRadians);
    double s = std::sin(angleRadians);
    Transformation3D t;
    t.m = {{{{c,0,s,0}},
            {{0,1,0,0}},
            {{-s,0,c,0}},
            {{0,0,0,1}}}};
    return t;
}

Transformation3D Transformation3D::rotation_z(double angleRadians) {
    double c = std::cos(angleRadians);
    double s = std::sin(angleRadians);
    Transformation3D t;
    t.m = {{{{c,-s,0,0}},
            {{s,c,0,0}},
            {{0,0,1,0}},
            {{0,0,0,1}}}};
    return t;
}

Point3D Transformation3D::apply(const Point3D& p) const {
    double xNew = m[0][0]*p.x + m[0][1]*p.y + m[0][2]*p.z + m[0][3];
    double yNew = m[1][0]*p.x + m[1][1]*p.y + m[1][2]*p.z + m[1][3];
    double zNew = m[2][0]*p.x + m[2][1]*p.y + m[2][2]*p.z + m[2][3];
    double w    = m[3][0]*p.x + m[3][1]*p.y + m[3][2]*p.z + m[3][3];
    if (w != 0.0) {
        xNew /= w; yNew /= w; zNew /= w;
    }
    return Point3D(xNew, yNew, zNew);
}

Transformation3D Transformation3D::operator*(const Transformation3D& other) const {
    Transformation3D result;
    for (int i=0; i<4; ++i) {
        for (int j=0; j<4; ++j) {
            result.m[i][j] = 0;
            for (int k=0; k<4; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Transformation3D& t) {
    for (int i=0; i<4; ++i) {
        os << "[";
        for (int j=0; j<4; ++j) {
            os << t.m[i][j];
            if (j<3) os << ", ";
        }
        os << "]";
        if (i<3) os << "\n";
    }
    return os;
}
